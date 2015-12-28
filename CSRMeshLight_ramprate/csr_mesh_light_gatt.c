/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *      csr_mesh_light_gatt.c
 *
 *  DESCRIPTION
 *      Implementation of the CSR Mesh light GATT-related routines
 *
 *****************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <ls_app_if.h>
#include <gap_app_if.h>
#include <gap_types.h>
#include <ls_err.h>
#include <ls_types.h>
#include <mem.h>
#include <panic.h>
#include <gatt.h>
#include <gatt_uuid.h>
#include <timer.h>
#include <debug.h>
#include <random.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#include "csr_mesh_light.h"
#include "csr_mesh_light_gatt.h"
#include "app_gatt_db.h"
#include "app_gatt.h"
#include "appearance.h"
#include "gap_service.h"
#include "mesh_control_service.h"
#include "mesh_control_service_uuids.h"
#include "csr_mesh.h"

#ifdef ENABLE_GATT_OTA_SERVICE
#include "csr_ota_service.h"
#include "gatt_service.h"
#endif /* ENABLE_GATT_OTA_SERVICE */

/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/* This constant is used in the main server app to define array that is 
   large enough to hold the advertisement data.
 */
#define MAX_ADV_DATA_LEN                                  (31)

/* Acceptable shortened device name length that can be sent in advertisement 
 * data 
 */
#define SHORTENED_DEV_NAME_LEN                            (8)

 /* length of Tx Power prefixed with 'Tx Power' AD Type */
#define TX_POWER_VALUE_LENGTH                             (2)

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

static void addDeviceNameToAdvData(uint16 adv_data_len, uint16 scan_data_len);
static void gattSetAdvertParams(bool fast_connection);
static void gattAdvertTimerHandler(timer_id tid);
static uint16 appRandomDelay(void);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      addDeviceNameToAdvData
 *
 *  DESCRIPTION
 *      This function is used to add device name to advertisement or scan 
 *      response data. It follows below steps:
 *      a. Try to add complete device name to the advertisment packet
 *      b. Try to add complete device name to the scan response packet
 *      c. Try to add shortened device name to the advertisement packet
 *      d. Try to add shortened (max possible) device name to the scan 
 *         response packet
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void addDeviceNameToAdvData(uint16 adv_data_len, uint16 scan_data_len)
{

    uint8 *p_device_name = NULL;
    uint16 device_name_adtype_len;

    /* Read device name along with AD Type and its length */
    p_device_name = GapGetNameAndLength(&device_name_adtype_len);

    /* Add complete device name to Advertisement data */
    p_device_name[0] = AD_TYPE_LOCAL_NAME_COMPLETE;

    /* Increment device_name_length by one to account for length field
     * which will be added by the GAP layer. 
     */

    /* Check if Complete Device Name can fit in remaining advertisement 
     * data space 
     */
    if((device_name_adtype_len + 1) <= (MAX_ADV_DATA_LEN - adv_data_len))
    {
        /* Add Complete Device Name to Advertisement Data */
        if (CsrMeshStoreUserAdvData(device_name_adtype_len , p_device_name, 
                      ad_src_advertise) != TRUE)
        {
            ReportPanic(app_panic_set_advert_data);
        }

    }
    /* Check if Complete Device Name can fit in Scan response message */
    else if((device_name_adtype_len + 1) <= (MAX_ADV_DATA_LEN - scan_data_len)) 
    {
        /* Add Complete Device Name to Scan Response Data */
        if (CsrMeshStoreUserAdvData(device_name_adtype_len , p_device_name, 
                      ad_src_scan_rsp) != TRUE)
        {
            ReportPanic(app_panic_set_scan_rsp_data);
        }

    }
    /* Check if Shortened Device Name can fit in remaining advertisement 
     * data space 
     */
    else if((MAX_ADV_DATA_LEN - adv_data_len) >=
            (SHORTENED_DEV_NAME_LEN + 2)) /* Added 2 for Length and AD type 
                                           * added by GAP layer
                                           */
    {
        /* Add shortened device name to Advertisement data */
        p_device_name[0] = AD_TYPE_LOCAL_NAME_SHORT;

       if (CsrMeshStoreUserAdvData(SHORTENED_DEV_NAME_LEN , p_device_name, 
                      ad_src_advertise) != TRUE)
        {
            ReportPanic(app_panic_set_advert_data);
        }

    }
    else /* Add device name to remaining Scan reponse data space */
    {
        /* Add as much as can be stored in Scan Response data */
        p_device_name[0] = AD_TYPE_LOCAL_NAME_SHORT;

       if (CsrMeshStoreUserAdvData(MAX_ADV_DATA_LEN - scan_data_len, 
                                    p_device_name, 
                                    ad_src_scan_rsp) != TRUE)
        {
            ReportPanic(app_panic_set_scan_rsp_data);
        }

    }

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      gattSetAdvertParams
 *
 *  DESCRIPTION
 *      This function is used to set advertisement parameters 
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void gattSetAdvertParams(bool fast_connection)
{
    uint8 advert_data[MAX_ADV_DATA_LEN];
    uint16 length;

    int8 tx_power_level = 0xff; /* Signed value */

    /* Tx power level value prefixed with 'Tx Power' AD Type */
    uint8 device_tx_power[TX_POWER_VALUE_LENGTH] = {
                AD_TYPE_TX_POWER
                };

    uint8 device_appearance[ATTR_LEN_DEVICE_APPEARANCE + 1] = {
                AD_TYPE_APPEARANCE,
                LE8_L(APPEARANCE_UNKNOWN_VALUE),
                LE8_H(APPEARANCE_UNKNOWN_VALUE)
                };

    /* A variable to keep track of the data added to AdvData. The limit is 
     * MAX_ADV_DATA_LEN. GAP layer will add AD Flags to AdvData which 
     * is 3 bytes. Refer BT Spec 4.0, Vol 3, Part C, Sec 11.1.3.
     */
    uint16 length_added_to_adv = 3;

    /* Add UUID list of the services supported by the device */
    length = GetSupported16BitUUIDServiceList(advert_data);

    /* One added for Length field, which will be added to Adv Data by GAP 
     * layer 
     */
    length_added_to_adv += (length + 1);

    if (CsrMeshStoreUserAdvData(length, advert_data, 
                                       ad_src_advertise) != TRUE)
    {
        ReportPanic(app_panic_set_advert_data);
    }

    /* One added for Length field, which will be added to Adv Data by GAP 
     * layer 
     */
    length_added_to_adv += (sizeof(device_appearance) + 1);

    /* Add device appearance to the advertisements */
    if (CsrMeshStoreUserAdvData(ATTR_LEN_DEVICE_APPEARANCE + 1, 
        device_appearance, ad_src_advertise) != TRUE)
    {
        ReportPanic(app_panic_set_advert_data);
    }

    /* Read tx power of the chip */
    if(LsReadTransmitPowerLevel(&tx_power_level) != ls_err_none)
    {
        /* Reading tx power failed */
        ReportPanic(app_panic_read_tx_pwr_level);
    }

    /* Add the read tx power level to device_tx_power 
      * Tx power level value is of 1 byte 
      */
    device_tx_power[TX_POWER_VALUE_LENGTH - 1] = (uint8 )tx_power_level;

    /* One added for Length field, which will be added to Adv Data by GAP 
     * layer 
     */
    length_added_to_adv += (TX_POWER_VALUE_LENGTH + 1);

    /* Add tx power value of device to the advertising data */
    if (CsrMeshStoreUserAdvData(TX_POWER_VALUE_LENGTH, device_tx_power, 
                          ad_src_advertise) != TRUE)
    {
        ReportPanic(app_panic_set_advert_data);
    }

    addDeviceNameToAdvData(length_added_to_adv, 0);

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      gattAdvertTimerHandler
 *
 *  DESCRIPTION
 *      This function is used to handle Idle timer expiry.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void gattAdvertTimerHandler(timer_id tid)
{
    if (g_lightapp_data.gatt_data.app_tid == tid)
    {
        g_lightapp_data.gatt_data.app_tid = TIMER_INVALID;

            /* After the timer expiry switch to fast advertising if device is
             * not connected.
             */
        GattTriggerFastAdverts();
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      appRandomDelay
 *
 *  DESCRIPTION
 *      This function generates a random delay from 0 to 10ms in steps of 50us.
 *
 *  RETURNS
 *      Returns delay in Micro Seconds.
 *
 *---------------------------------------------------------------------------*/
static uint16 appRandomDelay(void)
{
    uint16 rand_num = Random16();
    rand_num = rand_num%201;
    return (rand_num * 50);
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      HandleAccessRead
 *
 *  DESCRIPTION
 *      This function handles read operation on attributes (as received in 
 *      GATT_ACCESS_IND message) maintained by the application and respond 
 *      with the GATT_ACCESS_RSP message.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void HandleAccessRead(GATT_ACCESS_IND_T *p_ind)
{

    /* For the received attribute handle, check all the services that support 
     * attribute 'Read' operation handled by application.
     */

    if(GapCheckHandleRange(p_ind->handle))
    {
        /* Attribute handle belongs to GAP service */
        GapHandleAccessRead(p_ind);
    }
    else if(MeshControlCheckHandleRange(p_ind->handle))
    {
        /* Attribute handle belongs to Mesh Control service */
        MeshControlHandleAccessRead(p_ind);
    }
#ifdef ENABLE_GATT_OTA_SERVICE
    else if(GattCheckHandleRange(p_ind->handle))
    {
        /* Attribute handle belongs to Gatt service */
        GattHandleAccessRead(p_ind);
    }
    else if(OtaCheckHandleRange(p_ind->handle))
    {
        OtaHandleAccessRead(p_ind);
    }
#endif /* ENABLE_GATT_OTA_SERVICE */
    else
    {
        /* Application doesn't support 'Read' operation on received 
         * attribute handle, hence return 'gatt_status_read_not_permitted'
         * status
         */
        GattAccessRsp(p_ind->cid, p_ind->handle, 
                      gatt_status_read_not_permitted,
                      0, NULL);
    }

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      HandleAccessWrite
 *
 *  DESCRIPTION
 *      This function handles Write operation on attributes (as received in 
 *      GATT_ACCESS_IND message) maintained by the application.
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void HandleAccessWrite(GATT_ACCESS_IND_T *p_ind)
{

    /* For the received attribute handle, check all the services that support 
     * attribute 'Write' operation handled by application.
     */

    if(GapCheckHandleRange(p_ind->handle))
    {
        /* Attribute handle belongs to GAP service */
        GapHandleAccessWrite(p_ind);
    }
    else if(MeshControlCheckHandleRange(p_ind->handle))
    {
        /* Attribute handle belongs to Mesh Control service */
        MeshControlHandleAccessWrite(p_ind);
    }
#ifdef ENABLE_GATT_OTA_SERVICE 
    else if(GattCheckHandleRange(p_ind->handle))
    {
        /* Attribute handle belongs to Gatt service */
        GattHandleAccessWrite(p_ind);
    }
    else if(OtaCheckHandleRange(p_ind->handle))
    {
        OtaHandleAccessWrite(p_ind);
    }
#endif /* ENABLE_GATT_OTA_SERVICE */
    else
    {
        /* Application doesn't support 'Write' operation on received 
         * attribute handle, hence return 'gatt_status_write_not_permitted'
         * status
         */
        GattAccessRsp(p_ind->cid, p_ind->handle, 
                      gatt_status_write_not_permitted,
                      0, NULL);
    }

}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GattStartAdverts
 *
 *  DESCRIPTION
 *      This function is used to start undirected advertisements and moves to 
 *      ADVERTISING state.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void GattStartAdverts(bool fast_connection)
{
    /* Variable 'connect_flags' needs to be updated to have peer address type 
     * if Directed advertisements are supported as peer address type will 
     * only be used in that case. We don't support directed advertisements for 
     * this application.
     */

#ifdef USE_STATIC_RANDOM_ADDRESS
    uint16 connect_flags = L2CAP_CONNECTION_SLAVE_UNDIRECTED | 
                      L2CAP_OWN_ADDR_TYPE_RANDOM;
#else
    uint16 connect_flags = L2CAP_CONNECTION_SLAVE_UNDIRECTED | 
                          L2CAP_OWN_ADDR_TYPE_PUBLIC;
#endif /* USE_STATIC_RANDOM_ADDRESS */

    /* Set UCID to INVALID_UCID */
    g_lightapp_data.gatt_data.st_ucid = GATT_INVALID_UCID;

    /* Set advertisement parameters */
    gattSetAdvertParams(fast_connection);

    /* Start GATT connection in Slave role */
    GattConnectReq(NULL, connect_flags);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GattStopAdverts
 *
 *  DESCRIPTION
 *      This function is used to stop on-going advertisements.
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
extern void GattStopAdverts(void)
{
    GattCancelConnectReq();
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GetSupported16BitUUIDServiceList
 *
 *  DESCRIPTION
 *      This function prepares the list of supported 16-bit service UUIDs to be 
 *      added to Advertisement data. It also adds the relevant AD Type to the 
 *      starting of AD array.
 *
 *  RETURNS
 *      Return the size AD Service UUID data.
 *
 *---------------------------------------------------------------------------*/
extern uint16 GetSupported16BitUUIDServiceList(uint8 *p_service_uuid_ad)
{
    uint8 i = 0;

    /* Add 16-bit UUID for supported main service  */
    p_service_uuid_ad[i++] = AD_TYPE_SERVICE_UUID_16BIT_LIST;

    p_service_uuid_ad[i++] = LE8_L(MESH_CONTROL_SERVICE_UUID);
    p_service_uuid_ad[i++] = LE8_H(MESH_CONTROL_SERVICE_UUID);

    return ((uint16)i);

}


/*------------------------------------------------------------*
 *  NAME
 *      GattIsAddressResolvableRandom
 *
 *  DESCRIPTION
 *      This function checks if the address is resolvable random or not.
 *
 *  RETURNS
 *      Boolean - True (Resolvable Random Address) /
 *                     False (Not a Resolvable Random Address)
 *
 *----------------------------------------------------------*/
extern bool GattIsAddressResolvableRandom(TYPED_BD_ADDR_T *p_addr)
{
    if(p_addr->type != L2CA_RANDOM_ADDR_TYPE || 
       (p_addr->addr.nap & BD_ADDR_NAP_RANDOM_TYPE_MASK)
                                      != BD_ADDR_NAP_RANDOM_TYPE_RESOLVABLE)
    {
        /* This isn't a resolvable private address */
        return FALSE;
    }
    return TRUE;
} 

/*----------------------------------------------------------------------------*
 *  NAME
 *      GattTriggerFastAdverts
 *
 *  DESCRIPTION
 *      This function is used to trigger fast advertisements 
 *
 *  RETURNS
 *      Nothing
 *
 *---------------------------------------------------------------------------*/
extern void GattTriggerFastAdverts(void)
{
    CSR_MESH_USER_ADV_PARAMS_T adv_params;

    /* Reset existing advertising data */
    CsrMeshStoreUserAdvData(0, NULL, ad_src_advertise);
    CsrMeshStoreUserAdvData(0, NULL, ad_src_scan_rsp);

#ifdef USE_STATIC_RANDOM_ADDRESS
    /* Restore the Random Address of the Bluetooth Device */
    MemCopy(&adv_params.bd_addr.addr, &g_lightapp_data.random_bd_addr, 
                                                            sizeof(BD_ADDR_T));
    adv_params.bd_addr.type = L2CA_RANDOM_ADDR_TYPE;
#else
    adv_params.bd_addr.type = L2CA_PUBLIC_ADDR_TYPE;
#endif /* USE_STATIC_RANDOM_ADDRESS */

    /* Set GAP peripheral params */
    adv_params.role = gap_role_peripheral;
    adv_params.bond = gap_mode_bond_no;
    adv_params.connect_mode = gap_mode_connect_undirected;
    adv_params.discover_mode = gap_mode_discover_general;
    adv_params.security_mode = gap_mode_security_unauthenticate;

    g_lightapp_data.gatt_data.advert_timer_value = 
                                             ADVERT_INTERVAL + appRandomDelay();

    /* If GATT bearer is enabled send connectable advert */
    if(g_lightapp_data.bearer_data.bearerEnabled & BLE_GATT_SERVER_BEARER_MASK)
    {
        /* Set advertisement parameters */
        gattSetAdvertParams(TRUE);
        /* Send a connectable advertisement */
        CsrMeshSendUserAdvert(&adv_params);

        /* Restart the advertising timer */
        TimerDelete(g_lightapp_data.gatt_data.app_tid);
        g_lightapp_data.gatt_data.app_tid = TimerCreate(
                                   g_lightapp_data.gatt_data.advert_timer_value,
                                                    TRUE, gattAdvertTimerHandler);
    }
}


