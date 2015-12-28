/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *      gunilamp_hw.c
 *
 *  DESCRIPTION
 *      This file implements the colour and intensity control of Gunilamp. 
 *
 *****************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <uart.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "gunilamp_hw.h"
#include "user_config.h"

#ifdef GUNILAMP
/*============================================================================*
 *  Private data
 *============================================================================*/
/* UART TX and RX Buffers */
UART_DECLARE_BUFFER(GunilampUartTxBuffer, UART_BUF_SIZE_BYTES_32);
UART_DECLARE_BUFFER(GunilampUartRxBuffer, UART_BUF_SIZE_BYTES_32);

/* Maximum Size of the command */
#define MAX_LAMP_COMMAND_SIZE (8)

/* Clear Command */
static uint8 clear_cmd[7] = {0x85, 0x03, 0x41, 0x00, 0x00, 0x0d, 0x0a};

/*============================================================================*
 *  Private Function Declaration
 *============================================================================*/
/* Uart Receive call back */
static uint16 gunilampUartDataRxCallback(void* p_data, uint16 data_count,
                                         uint16* p_num_additional_words);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      gunilampUartDataRxCallback
 *
 *  DESCRIPTION
 *      This callback is issued when data is received over UART. This is same
 *      as UartDataRxCallback in normal applications.
 *
 *  RETURNS
 *      The number of words processed, return data_count if all of the received
 *      data had been processed (or if application don't care about the data).
 *
 *----------------------------------------------------------------------------*/
static uint16 gunilampUartDataRxCallback(void* p_data, uint16 data_count,
                                         uint16* p_num_additional_words)
{
    /* Application needs 3 additional data to be received */
    *p_num_additional_words = 3;

    return data_count;
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      gunilampBlinkEnable
 *
 *  DESCRIPTION
 *      This function enables or disables blinking on Gunilamp.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void gunilampBlinkEnable(bool enable)
{
    /* Enable or Disable Blinking */
    clear_cmd[3] = (enable == TRUE)?0x20:0x00;

    UartWriteBlocking(clear_cmd, sizeof(clear_cmd));
}

/*-----------------------------------------------------------------------------*
 *  NAME
 *      gunilampSetRGBW
 *
 *  DESCRIPTION
 *      This function set color described by RGB and White value on Gunilamp.
 *
 *  RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
static void gunilampSetRGBW(uint8 red, uint8 green, uint8 blue, uint8 white)
{
    uint8 lamp_command[MAX_LAMP_COMMAND_SIZE];

    /* Gunilamp command */
    lamp_command[0] = 0x86;  /* command */
    lamp_command[1] = 0x04;  /* length of lamp_command */
    lamp_command[2] = red;
    lamp_command[3] = green;
    lamp_command[4] = blue;
    lamp_command[5] = white;
    lamp_command[6] = 0x0d;
    lamp_command[7] = 0x0a;

    UartWriteBlocking(lamp_command, MAX_LAMP_COMMAND_SIZE);
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      GuniLampInit
 *
 *  DESCRIPTION
 *      This function initializes the Gunilamp UART interface.
 *
 * PARAMETERS
 *      Nothing.
 *
 * RETURNS
 *      Nothing.
 *----------------------------------------------------------------------------*/
extern void GuniLampInit(void)
{
    /* Initialise UART and configure with
     * default baud rate and port configration.
     */
    UartInit(gunilampUartDataRxCallback, NULL,
             GunilampUartRxBuffer, UART_BUF_SIZE_BYTES_32,
             GunilampUartTxBuffer, UART_BUF_SIZE_BYTES_32,
             uart_data_unpacked);
    
    /* Enable UART */
    UartEnable(TRUE);

    /* UART Rx threshold is set to 3, so that every three bytes
     * received will trigger the rx callback.
     */
    UartRead(3, 0);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GuniLampControl
 *
 *  DESCRIPTION
 *      This function sends the control command to the GuniLamp controller
 *      with the rqruired parameters
 *
 * PARAMETERS
 *      red    Red light intensity
 *      green  Green light intensity
 *      blue   Blue light intensity
 *      white  White light intensity
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern void GuniLampControl(uint8 red, uint8 green, uint8 blue, uint8 white)
{
    /* Disable Blinking */
    gunilampBlinkEnable(FALSE);

    /* Set Color */
    gunilampSetRGBW(red, green, blue, white);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      GuniLampBlink
 *
 *  DESCRIPTION
 *      This function sends the control command to the GuniLamp controller
 *      to blink light in desired color.
 *
 * PARAMETERS
 *      red    Red light intensity
 *      green  Green light intensity
 *      blue   Blue light intensity
 *
 * RETURNS
 *      Nothing
 *----------------------------------------------------------------------------*/
extern void GuniLampBlink(uint8 red, uint8 green, uint8 blue)
{
    /* Turn off lamp to avoid blinking while changing colour */
    gunilampSetRGBW(0x00, 0x00, 0x00, 0x00);

    /* Enable Blinking */
    gunilampBlinkEnable(TRUE);

    /* Set Color */
    gunilampSetRGBW(red, green, blue, 0x00);
}
#endif

