/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 * FILE
 *      user_config.h
 *
 * DESCRIPTION
 *      This file contains definitions which will enable customization of the
 *      application.
 *
 ******************************************************************************/

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

/*=============================================================================*
 *  Public Definitions
 *============================================================================*/
/* Application version */
#define APP_MAJOR_VERSION   (1)
#define APP_MINOR_VERSION   (3)

/* Application NVM version. This version is used to keep the compatibility of
 * NVM contents with the application version. This value needs to be modified
 * only if the new version of the application has a different NVM structure
 * than the previous version (such as number of groups supported) that can
 * shift the offsets of the currently stored parameters.
 * If the application NVM version has changed, it could still read the values
 * from the old Offsets and store into new offsets.
 * This application currently erases all the NVM values if the NVM version has
 * changed.
 */
#define APP_NVM_VERSION     (4)

#define CSR_MESH_LIGHT_PID  (0x1060)

/* Vendor ID for CSR */
#define APP_VENDOR_ID       (0x0A12)

/* Product ID. */
#define APP_PRODUCT_ID      (CSR_MESH_LIGHT_PID)

/* Version Number. */
#define APP_VERSION          (((uint32)(APP_PRODUCT_ID    & 0xFF) << 24) | \
                              ((uint32)(APP_NVM_VERSION   & 0xFF) << 16) | \
                              ((uint32)(APP_MAJOR_VERSION & 0xFF) << 8)  | \
                              ((uint32)(APP_MINOR_VERSION & 0xFF)))

/* Enable this if NVM offset compatablity is required with CSRmesh Verisons 1.2
 * and before.
 * Some the NVM parameters used by the earlier versions have been removed. 
 * To be able to read the NVM data stored by earlier versions upon an application
 * update NVM offsets need to be adjusted.
 */
#define NVM_BACKWARD_COMPATIBILITY

/* Number of model groups supported */
#define MAX_MODEL_GROUPS     (4)

/* Macro to enable GATT OTA SERVICE */
/* Over-the-Air Update is supported only on EEPROM */
#ifdef NVM_TYPE_EEPROM

#define ENABLE_GATT_OTA_SERVICE

/* Enable firmware model support */
#define ENABLE_FIRMWARE_MODEL

#endif /* NVM_TYPE_EEPROM */

/* Macro to enable Data Model support */
#define ENABLE_DATA_MODEL

/* Enable Static Random Address. */
/* #define USE_STATIC_RANDOM_ADDRESS */

/* Select target hardware. By default the application builds for the controls
 * on the IOT Demo Board. Uncomment the GUNILAMP to build the application for
 * Gunilamp
 */
/* #define GUNILAMP */

/* Enable support for Colour Temperature */
/* #define COLOUR_TEMP_ENABLED */

/* Enable application debug logging on UART */
#define DEBUG_ENABLE

/* SW2 button on IOT board will be used for Association removal
 * and restart when USE_ASSOCIATION_REMOVAL_KEY is defined.
 */
#if (!defined(GUNILAMP) && !defined(DEBUG_ENABLE))
#define USE_ASSOCIATION_REMOVAL_KEY
#endif

/* Enable fast PWM using PIO controller instead of Hardware PWM */
/* #define ENABLE_FAST_PWM */

/* Enables Authorization Code on Device. */
/* #define USE_AUTHORIZATION_CODE */

/* Enable battery model support */
/* #define ENABLE_BATTERY_MODEL */

#endif /* __USER_CONFIG_H__ */

