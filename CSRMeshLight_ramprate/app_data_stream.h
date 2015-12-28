/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *      app_data_stream.h
 *
 *  DESCRIPTION
 *      Header definitions for application data stream implementation
 *
 *****************************************************************************/

#ifndef __APP_DATA_STREAM_H__
#define __APP_DATA_STREAM_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <bt_event_types.h>

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include "user_config.h"

/*============================================================================*
 *  CSRmesh Header Files
 *============================================================================*/
#include <csr_mesh.h>
#include <stream_model.h>


#ifdef ENABLE_DATA_MODEL
/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/* Application protocol codes used to exchange device data over the
 * data stream model
 */
typedef enum
{
    CSR_DEVICE_INFO_REQ = 0x01,
    CSR_DEVICE_INFO_RSP = 0x02,
    CSR_DEVICE_INFO_SET = 0x03,
    CSR_DEVICE_INFO_RESET = 0x04
}APP_DATA_STREAM_CODE_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
/* Initialises the application data stream protocol */
extern void AppDataStreamInit(uint16 *group_id_list, uint16 num_groups);

/* This function handles the CSR_MESH_DATA_STREAM_FLUSH message.*/
extern void handleCSRmeshDataStreamFlushInd(CSR_MESH_STREAM_EVENT_T *p_event);

/* This function handles the CSR_MESH_DATA_BLOCK_IND message */
extern void handleCSRmeshDataBlockInd(CSR_MESH_STREAM_EVENT_T *p_event);

/* This function handles the CSR_MESH_DATA_STREAM_DATA_IND message */
extern void handleCSRmeshDataStreamDataInd(CSR_MESH_STREAM_EVENT_T *p_event);

/* This function handles the CSR_MESH_DATA_STREAM_SEND_CFM message */
extern void handleCSRmeshDataStreamSendCfm(CSR_MESH_STREAM_EVENT_T *p_event);

#endif /* ENABLE_DATA_MODEL */

#endif /* __APP_DATA_STREAM_H__ */

