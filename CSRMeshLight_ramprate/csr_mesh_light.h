/*****************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *      csr_mesh_light.h
 *
 *  DESCRIPTION
 *      This file defines a simple implementation of CSRmesh light device.
 *
 *****************************************************************************/

#ifndef __CSR_MESH_LIGHT_H__
#define __CSR_MESH_LIGHT_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <bt_event_types.h>
#include <timer.h>

/*============================================================================*
 *  CSRmesh Header Files
 *============================================================================*/
#include <csr_mesh.h>
#include <light_model.h>
#include <power_model.h>
#include <firmware_model.h>
#include <bearer_model.h>
#include <attention_model.h>
#ifdef ENABLE_BATTERY_MODEL
#include <battery_model.h>
#endif /* ENABLE_BATTERY_MODEL */

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*============================================================================*
 *  Public Data Types
 *============================================================================*/

/* CSRmesh device association state */
typedef enum
{

    /* Application Initial State */
    app_state_not_associated = 0,

    /* Application state association started */
    app_state_association_started,

    /* Application state associated */
    app_state_associated,

} app_association_state;

typedef enum
{

    /* Application Initial State */
    app_state_init = 0,

    /* Enters when fast undirected advertisements are configured */
    app_state_advertising,

    /* Enters when device is not connected and not advertising */
    app_state_idle,

    /* Enters when connection is established with the host */
    app_state_connected,

    /* Enters when disconnect is initiated by the application */
    app_state_disconnecting,

} app_state;


/* GATT Service Data Structure */
typedef struct
{
    /* TYPED_BD_ADDR_T of the host to which device is connected */
    TYPED_BD_ADDR_T                con_bd_addr;

    /* Track the UCID as Clients connect and disconnect */
    uint16                         st_ucid;

    /* Boolean flag indicates whether the device is temporary paired or not */
    bool                           paired;

    /* Value for which advertisement timer needs to be started */
    uint32                         advert_timer_value;

    /* When device is not connected, switch to fast advertisement mode,
     * after idle time.
     */
    timer_id                       app_tid;

    /* Variable to store the current connection interval being used. */
    uint16                         conn_interval;

    /* Variable to store the current slave latency. */
    uint16                         conn_latency;

    /* Variable to store the current connection time-out value. */
    uint16                         conn_timeout;

    /* Variable to keep track of number of connection
     * parameter update requests made.
     */
    uint8                          num_conn_update_req;

    /* Store timer id for Connection Parameter Update timer in Connected
     * state
     */
    timer_id                       con_param_update_tid;

    /* Connection Parameter Update timer value. Upon a connection, it's started
     * for a period of TGAP_CPP_PERIOD, upon the expiry of which it's restarted
     * for TGAP_CPC_PERIOD. When this timer is running, if a GATT_ACCESS_IND is
     * received, it means, the central device is still doing the service discov-
     * -ery procedure. So, the connection parameter update timer is deleted and
     * recreated. Upon the expiry of this timer, a connection parameter update
     * request is sent to the central device.
     */
    uint32                         cpu_timer_value;

} APP_GATT_SERVICE_DATA_T;

/* CSRmesh Light application data structure */
typedef struct
{
    /* Application GATT data */
    APP_GATT_SERVICE_DATA_T        gatt_data;

    /* Current state of application */
    app_state                      state;

    /* CSRmesh Association State */
    app_association_state           assoc_state;

    /* CSRmesh transmit device id advertise timer id */
    timer_id                       mesh_device_id_advert_tid;

    /* Local Device's Random Bluetooth Device Address. */
#ifdef USE_STATIC_RANDOM_ADDRESS
    BD_ADDR_T                      random_bd_addr;
#endif /* USE_STATIC_RANDOM_ADDRESS */

    /* Light state will reflect the state of light hardware */
    LIGHT_MODEL_STATE_DATA_T       light_state;

    /* Power State */
    POWER_MODEL_STATE_DATA_T       power;

    /* Attention Model State */
    ATTENTION_MODEL_STATE_DATA_T   attn_data;

    /* Firmware Version Information */
#ifdef ENABLE_FIRMWARE_MODEL
    FIRMWARE_MODEL_STATE_DATA_T    fw_version;
#endif

    /* Bearer Model Data */
    BEARER_MODEL_STATE_DATA_T      bearer_data;

    /* Battery Level Data */
#ifdef ENABLE_BATTERY_MODEL
    BATTERY_MODEL_STATE_DATA_T     battery_data;
#endif

    /* Timer to check for RGB change and store on NVM */
    timer_id                       nvm_tid;
} CSRMESH_LIGHT_DATA_T;

/*============================================================================*
 *  Public Data
 *============================================================================*/

/* CSR mesh light application specific data */
extern CSRMESH_LIGHT_DATA_T g_lightapp_data;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function is used to set the state of the application */
extern void AppSetState(app_state new_state);

/* This function contains handling of short button press */
extern void HandleShortButtonPress(void);

/* This function generate random delays */
extern uint16 AppRandomDelay(void);

#endif /* __CSR_MESH_LIGHT_H__ */

