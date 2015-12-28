/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *    battery_hw.c
 *
 * DESCRIPTION
 *    This file defines routines for reading current Battery level
 *
 ******************************************************************************/

/*=============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <gatt.h>
#include <battery.h>

/*=============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "battery_hw.h"

/*=============================================================================*
 *  Private Definitions
 *============================================================================*/

/* Battery Level Full */
#define BATTERY_LEVEL_FULL                          (100)

/* Battery critical level, in percents */
#define BATTERY_CRITICAL_LEVEL                      (10)

/* Battery minimum and maximum voltages in mV */
#define BATTERY_FULL_BATTERY_VOLTAGE                (3000)          /* 3.0V */
#define BATTERY_FLAT_BATTERY_VOLTAGE                (1800)          /* 1.8V */

/*=============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*-----------------------------------------------------------------------------*
 *  NAME
 *      ReadBatteryLevel
 *
 *  DESCRIPTION
 *      This function reads the battery level 
 *
 *  RETURNS
 *      Battery Level in percent
 *
 *----------------------------------------------------------------------------*/
extern uint8 ReadBatteryLevel(void)
{
    uint32 bat_voltage;
    uint32 bat_level;

    /* Read battery voltage and level it with minimum voltage */
    bat_voltage = BatteryReadVoltage();

    /* Level the read battery voltage to the minimum value */
    if(bat_voltage < BATTERY_FLAT_BATTERY_VOLTAGE)
    {
        bat_voltage = BATTERY_FLAT_BATTERY_VOLTAGE;
    }

    bat_voltage -= BATTERY_FLAT_BATTERY_VOLTAGE;

    /* Get battery level in percent */
    bat_level = (bat_voltage * 100) / (BATTERY_FULL_BATTERY_VOLTAGE - 
                                                  BATTERY_FLAT_BATTERY_VOLTAGE);

    /* Check the precision errors */
    if(bat_level > 100)
    {
        bat_level = 100;
    }

    return (uint8)bat_level;
}


