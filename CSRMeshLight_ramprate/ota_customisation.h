/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *      ota_customisation.h
 *
 *  DESCRIPTION
 *      Customisation requirements for the CSR OTAU functionality.
 *
 *****************************************************************************/

#ifndef __OTA_CUSTOMISATION_H__
#define __OTA_CUSTOMISATION_H__

/*=============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "user_config.h"

#ifdef ENABLE_GATT_OTA_SERVICE
/* ** CUSTOMISATION **
 * The following header file names may need altering to match your application.
 */

#include "app_gatt.h"
#include "app_gatt_db.h"
#include "csr_mesh_light.h"

/*=============================================================================*
 *  Private Definitions
 *============================================================================*/

/* ** CUSTOMISATION **
 * Change these definitions to match your application.
 */
#define CONNECTION_CID      g_lightapp_data.gatt_data.st_ucid
#define IS_PAIRED           g_lightapp_data.gatt_data.paired
#define CONN_CENTRAL_ADDR   g_lightapp_data.gatt_data.con_bd_addr
#define DEVICE_RANDOM_ADDR  g_lightapp_data.random_bd_addr

#endif /* ENABLE_GATT_OTA_SERVICE */

#endif /* __OTA_CUSTOMISATION_H__ */

