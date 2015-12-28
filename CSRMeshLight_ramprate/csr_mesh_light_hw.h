/*****************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *      csr_mesh_light_hw.h
 *
 *  DESCRIPTION
 *      This file defines a interface to abstract hardware specifics of light.
 *
 *****************************************************************************/

#ifndef __CSR_MESH_LIGHT_HW_H__
#define __CSR_MESH_LIGHT_HW_H__

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function initializes the light hardware, like PIO, interface etc. */
extern void LightHardwareInit(void);

/* Controls the light device power. */
extern void LightHardwarePowerControl(bool power_on);

/* Controls the color of the light. */
extern bool LightHardwareSetColor(uint8 red, uint8 green, uint8 blue);

/* Controls the brightness of the light. */
extern void LightHardwareSetLevel(uint8 red, uint8 green, uint8 blue, 
                                  uint8 level);

/* Controls the colour temperature. */
extern bool LightHardwareSetColorTemp(uint16 temp);

/* Controls the blink colour and duration of light. */
extern bool LightHardwareSetBlink(uint8 red, uint8 green, uint8 blue,
                                  uint8 on_time, uint8 off_time);
#endif /* __CSR_MESH_LIGHT_HW_H__ */

