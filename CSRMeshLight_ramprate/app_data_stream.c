/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *    app_data_stream.c
 *
 * DESCRIPTION
 *    This file implements a simple protocol over the data model to exchange
 *    device information.
 *    The protocol:
 *       | CODE | LEN (1 or 2 Octets| Data (LEN Octets)|
 *       CODE Defined by APP_DATA_STREAM_CODE_T
 *       LEN - if MS Bit of First octet is 1, then len is 2 octets
 *       if(data[0] & 0x80) LEN = data[0]
 *
 ******************************************************************************/

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <gatt.h>
#include <timer.h>
#include <mem.h>

/*============================================================================*
 *  CSR Mesh Header Files
 *============================================================================*/
#include <csr_mesh.h>
#include <stream_model.h>

/*=============================================================================*
 *  Local Header Files
*============================================================================*/
#include "app_data_stream.h"

#ifdef  ENABLE_DATA_MODEL
/*=============================================================================*
 *  Private Definitions
 *============================================================================*/

#define DEVICE_INFO_STRING               "CSRmesh Light example application\r\n" \
                                         "Supported Models:\r\n" \
                                         "  Light Model\r\n" \
                                         "  Power Model\r\n" \
                                         "  Attention Model\r\n" \
                                         "  Data Model"

/* Data stream send retry wait time */
#define STREAM_SEND_RETRY_TIME            (500 * MILLISECOND)

/* Data stream received timeout value */
#define RX_STREAM_TIMEOUT                 (5 * SECOND)

/* Max number of retries */
#define MAX_SEND_RETRIES                  (3)

/*=============================================================================*
 *  Private Data
 *============================================================================*/
/* String to give a brief description of the application */
static uint8 device_info[256];

/* Device info length */
static uint8 device_info_length;

/* Stream bytes sent tracker */
static uint16 tx_stream_offset = 0;

/* Stream send retry timer */
static timer_id stream_send_retry_tid = TIMER_INVALID;

/* Stream send retry counter */
static uint16 stream_send_retry_count = 0;

/* Current Rx Stream offset */
static uint16 rx_stream_offset = 0;

/* Rx Stream status flag */
static bool rx_stream_in_progress = FALSE;

/* Rx stream timeout tid */
static timer_id rx_stream_timeout_tid;

static APP_DATA_STREAM_CODE_T current_stream_code;


/*=============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
static void streamSendRetryTimer(timer_id tid);
static void sendNextPacket(void);

/*=============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      streamSendRetryTimer
 *
 *  DESCRIPTION
 *      Timer handler to retry sending next packet
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void streamSendRetryTimer(timer_id tid)
{
    if( tid == stream_send_retry_tid )
    {
        stream_send_retry_tid = TIMER_INVALID;
        stream_send_retry_count++;
        if( stream_send_retry_count < MAX_SEND_RETRIES )
        {
            StreamResendLastData();
            stream_send_retry_tid =  TimerCreate(STREAM_SEND_RETRY_TIME, TRUE,
                                                          streamSendRetryTimer);
        }
        else
        {
            stream_send_retry_count = 0;
            StreamFlush();
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      rxStreamTimeoutHandler
 *
 *  DESCRIPTION
 *      Timer handler to handle rx stream timeout
 *
 *  RETURNS
 *      Nothing.
 *
 *---------------------------------------------------------------------------*/
static void rxStreamTimeoutHandler(timer_id tid)
{
    if( tid == rx_stream_timeout_tid )
    {
        /* Reset the stream */
        rx_stream_timeout_tid = TIMER_INVALID;
        rx_stream_in_progress = FALSE;
        StreamReset();

    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      sendNextPacket
 *
 *  DESCRIPTION
 *      Forms a stream data packet with the current counter and sends it to
 *      the stream receiver
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void sendNextPacket(void)
{
    uint16 data_pending, len;

    /* Stop retry timer */
    stream_send_retry_count = 0;
    TimerDelete(stream_send_retry_tid);
    stream_send_retry_tid = TIMER_INVALID;

    data_pending = device_info_length+2 - tx_stream_offset;

    if( data_pending )
    {
        len = (data_pending > STREAM_DATA_BLOCK_SIZE_MAX)? STREAM_DATA_BLOCK_SIZE_MAX:data_pending;

        /* Send the next packet */
        StreamSendData(&device_info[tx_stream_offset], len);
        tx_stream_offset += len;

        stream_send_retry_tid = TimerCreate(100 * MILLISECOND, TRUE,
                                                       streamSendRetryTimer);
    }
    else
    {
        /* Send flush to indicate end of stream */
        StreamFlush();
    }
}

/*=============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      AppDataStreamInit
 *
 *  DESCRIPTION
 *      This function initializes the stream Model.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void AppDataStreamInit(uint16 *group_id_list, uint16 num_groups)
{
    /* Model initialisation */
    StreamModelInit(group_id_list, num_groups);

    /* Reset timers */
    stream_send_retry_tid = TIMER_INVALID;
    rx_stream_timeout_tid = TIMER_INVALID;

    /* Reset the device info */
    device_info_length = sizeof(DEVICE_INFO_STRING);
    device_info[0] = CSR_DEVICE_INFO_RSP;
    device_info[1] = device_info_length;

    MemCopy(&device_info[2], DEVICE_INFO_STRING, sizeof(DEVICE_INFO_STRING));
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleCSRmeshDataStreamFlushInd
 *
 *  DESCRIPTION
 *      This function handles the CSR_MESH_DATA_STREAM_FLUSH message.
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void handleCSRmeshDataStreamFlushInd(CSR_MESH_STREAM_EVENT_T *p_event)
{
    rx_stream_offset = 0;

    if( rx_stream_in_progress == FALSE )
    {
        /* Start the stream timeout timer */
        TimerDelete(rx_stream_timeout_tid);
        rx_stream_timeout_tid = TimerCreate(RX_STREAM_TIMEOUT, TRUE,
                                                        rxStreamTimeoutHandler);
    }
    else
    {
        /* End of stream */
        rx_stream_in_progress = FALSE;
        TimerDelete(rx_stream_timeout_tid);
        rx_stream_timeout_tid = TIMER_INVALID;
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleCSRmeshDataBlockInd
 *
 *  DESCRIPTION
 *      This function handles the CSR_MESH_DATA_BLOCK_IND message
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void handleCSRmeshDataBlockInd(CSR_MESH_STREAM_EVENT_T *p_event)
{
    switch(p_event->data[0])
    {
        case CSR_DEVICE_INFO_REQ:
        {
            /* Set the source device ID as the stream target device */
            StreamStartSender(p_event->common_data.source_id );
            tx_stream_offset = 0;
            /* Set the opcode to CSR_DEVICE_INFO_RSP */
            device_info[0] = CSR_DEVICE_INFO_RSP;

            /* start sending the data */
            sendNextPacket();
        }
        break;

        case CSR_DEVICE_INFO_RESET:
        {
            /* Reset the device info */
            device_info_length = sizeof(DEVICE_INFO_STRING);
            device_info[0] = CSR_DEVICE_INFO_RSP;
            device_info[1] = device_info_length;
            MemCopy(&device_info[2], DEVICE_INFO_STRING,
                                                   sizeof(DEVICE_INFO_STRING));
        }
        break;

        default:
        break;
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleCSRmeshDataStreamDataInd
 *
 *  DESCRIPTION
 *      This function handles the CSR_MESH_DATA_STREAM_DATA_IND message
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void handleCSRmeshDataStreamDataInd(CSR_MESH_STREAM_EVENT_T *p_event)
{
    /* Restart the stream timeout timer */
    TimerDelete(rx_stream_timeout_tid);
    rx_stream_timeout_tid = TimerCreate(RX_STREAM_TIMEOUT, TRUE,
                                                    rxStreamTimeoutHandler);

    /* Set stream_in_progress flag to TRUE */
    rx_stream_in_progress = TRUE;

    if( rx_stream_offset == 0 )
    {
        /* If the stream offset is 0. The data[0] will be the CODE */
        switch(p_event->data[0])
        {
            case CSR_DEVICE_INFO_REQ:
            {
                /* Set the source device ID as the stream target device */
                StreamStartSender(p_event->common_data.source_id);
                tx_stream_offset = 0;
                /* Set the stream code to CSR_DEVICE_INFO_RSP */
                device_info[0] = CSR_DEVICE_INFO_RSP;
                /* Start sending */
                sendNextPacket();
            }
            break;

            case CSR_DEVICE_INFO_RESET:
            {
                /* Reset the device info */
                device_info_length = sizeof(DEVICE_INFO_STRING);
                device_info[0] = CSR_DEVICE_INFO_RSP;
                device_info[1] = device_info_length;
                MemCopy(&device_info[2], DEVICE_INFO_STRING,
                                                    sizeof(DEVICE_INFO_STRING));
            }
            break;

            case CSR_DEVICE_INFO_SET:
            {
                /* CSR_DEVICE_INFO_SET is received. Store the code, length and
                 * the data into the device_info array in the format received
                 */
                current_stream_code = CSR_DEVICE_INFO_SET;
                device_info_length = p_event->data[1];
                MemCopy(device_info, p_event->data, p_event->data_len);
                rx_stream_offset = p_event->data_len;
            }
            break;
            default:
            break;
        }
    }
    else
    {
        if( current_stream_code == CSR_DEVICE_INFO_SET
            && rx_stream_offset + p_event->data_len < sizeof(device_info) )
        {
            MemCopy(&device_info[rx_stream_offset], p_event->data,
                                                            p_event->data_len);
            rx_stream_offset += p_event->data_len;
        }

        /* No other CODE is handled currently */
    }
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      handleCSRmeshDataStreamSendCfm
 *
 *  DESCRIPTION
 *      This function handles the CSR_MESH_DATA_STREAM_SEND_CFM message
 *
 *  RETURNS
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void handleCSRmeshDataStreamSendCfm(CSR_MESH_STREAM_EVENT_T *p_event)
{
    /* Send next block if it is not end of string */
    sendNextPacket();
}
#endif /* ENABLE_DATA_MODEL */

