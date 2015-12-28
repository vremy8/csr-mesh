/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *      battery_hw.h
 *
 *  DESCRIPTION
 *      Header definitions for battery access routines
 *
 *****************************************************************************/

#ifndef __BATTERY_HW_H__
#define __BATTERY_HW_H__

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/

#include <types.h>
#include <bt_event_types.h>

/*=============================================================================*
 *  Public Definitions
 *============================================================================*/

/* Battery Level Full */
#define BATTERY_LEVEL_FULL                          (100)

/* Battery critical level, in percent */
#define BATTERY_CRITICAL_LEVEL                      (10)

/* Battery minimum and maximum voltages in mV */
#define BATTERY_FULL_BATTERY_VOLTAGE                (3000)          /* 3.0V */
#define BATTERY_FLAT_BATTERY_VOLTAGE                (1800)          /* 1.8V */

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/

/* This function reads the battery level */
extern uint8 ReadBatteryLevel(void);

#endif /* __BATTERY_HW_H__ */

