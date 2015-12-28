/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *      csr_mesh_light_gatt.h
 *
 *  DESCRIPTION
 *      Header file for CSRmesh Light GATT-related routines
 *
 ******************************************************************************/

#ifndef __CSR_MESH_LIGHT_GATT_H__
#define __CSR_MESH_LIGHT_GATT_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <time.h>
#include <gatt_prim.h>

/*============================================================================*
 *  Public Definitions
 *============================================================================*/
/* Connectable advert interval.
 * Adjust the interval to run on a gross timebase of 1250ms to allow CSRmesh
 * to complete atleast one scan interval between conn-ads.
 * The scan duty cycles set will be more accurate when the time spent in the 
 * conn-ad (5ms) and the random jitter variation are accounted for in the
 * advertising interval.
 */
#define GATT_ADVERT_GROSS_INTERVAL  (1245 * MILLISECOND)
#define GATT_ADVERT_RANDOM          (10 * MILLISECOND)

#define ADVERT_INTERVAL                (GATT_ADVERT_GROSS_INTERVAL \
                                     - (GATT_ADVERT_RANDOM/2))

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function handles read operation on attributes (as received in 
 * GATT_ACCESS_IND message) maintained by the application
 */
extern void HandleAccessRead(GATT_ACCESS_IND_T *p_ind);

/* This function handles Write operation on attributes (as received in 
 * GATT_ACCESS_IND message) maintained by the application.
 */
extern void HandleAccessWrite(GATT_ACCESS_IND_T *p_ind);

/* This function is used to start undirected advertisements and moves to 
 * ADVERTISING state
 */
extern void GattStartAdverts(bool fast_connection);

/* This function is used to stop on-going advertisements */
extern void GattStopAdverts(void);

/* This function prepares the list of supported 16-bit service UUIDs to be 
 * added to Advertisement data
 */
extern uint16 GetSupported16BitUUIDServiceList(uint8 *p_service_uuid_ad);

/* This function checks if the address is resolvable random or not */
extern bool GattIsAddressResolvableRandom(TYPED_BD_ADDR_T *p_addr);

/* This function is used to trigger fast advertisements */
extern void GattTriggerFastAdverts(void);

#endif /* __CSR_MESH_LIGHT_GATT_H__ */
