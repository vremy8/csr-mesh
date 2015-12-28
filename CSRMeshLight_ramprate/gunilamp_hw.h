/******************************************************************************
 *  Copyright (C) Cambridge Silicon Radio Limited 2014
 *
 *  FILE
 *      gunilamp_hw.h
 *
 *  DESCRIPTION
 *      This file defines all the function which interact with Gunilamp
 *      hardware to control colour, intensity and power of Gunilamp.
 *
 *  NOTES
 *
 ******************************************************************************/
#ifndef __GUNILAMP_HW_H__
#define __GUNILAMP_HW_H__

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function initializes the Gunilamp UART interface. */
extern void GuniLampInit(void);

/* This function sends the control command to the GuniLamp controller
 * with the required parameters.
 */
extern void GuniLampControl(uint8 red, uint8 green, uint8 blue, uint8 white);

/* This function sends command to blink Gunilamp in desired colour */
extern void GuniLampBlink(uint8 red, uint8 green, uint8 blue);

#endif /* __GUNILAMP_HW_H__ */

