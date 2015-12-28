/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *      csr_mesh_light_hw.c
 *
 *  DESCRIPTION
 *      This file implements the CSRmesh light hardware specific functions.
 *
 *  NOTE
 *      Default hardware is always IOT board.
 *
 *****************************************************************************/

/*============================================================================*
 *  SDK Header Files
 *============================================================================*/
#include <types.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "user_config.h"
#ifdef GUNILAMP
#include "gunilamp_hw.h"
#else /* IOT Board */
#include "iot_hw.h"
#endif

#include "csr_mesh_light_hw.h"

/*============================================================================*
 *  Private Data
 *============================================================================*/
/* Color temperature calculation parameters */
/* Temperature is stored in 500 kelvin units */
#define CCT_TEMP_FACTOR             (500)
#define PACK_CCT_LEVEL(temp, level) (((uint16)level << 8)| \
                                     ((temp/CCT_TEMP_FACTOR) & 0xFF))
#define GET_LEVEL(val)              ((val >> 8) & 0xFF)
#define GET_TEMP(val)               (val & 0xFF)
#define LUT_SIZE(lut)               (sizeof(lut)/sizeof(lut[0]))


#ifdef COLOUR_TEMP_ENABLED
/* Look up tables for color temperature
 * (lower byte is Temperature in 500 kelvin units)
 * and color level (higher byte).
 */

/* LUT for Red */
static const uint16 cct_red_lut[] =
{
    PACK_CCT_LEVEL(  6500, 255),
    PACK_CCT_LEVEL(  7000, 245),
    PACK_CCT_LEVEL(  8000, 227),
    PACK_CCT_LEVEL( 10000, 204),
    PACK_CCT_LEVEL( 12500, 188),
    PACK_CCT_LEVEL( 15000, 179),
    PACK_CCT_LEVEL( 20000, 168),
    PACK_CCT_LEVEL( 25000, 163),
    PACK_CCT_LEVEL( 30000, 159),
    PACK_CCT_LEVEL( 35000, 157),
    PACK_CCT_LEVEL( 40000, 155)
};

/* LUT for Green */
static const uint16 cct_green_lut[] =
{
    PACK_CCT_LEVEL(  1000,  51),
    PACK_CCT_LEVEL(  1500, 109),
    PACK_CCT_LEVEL(  2500, 161),
    PACK_CCT_LEVEL(  3000, 180),
    PACK_CCT_LEVEL(  4000, 209),
    PACK_CCT_LEVEL(  5000, 228),
    PACK_CCT_LEVEL(  6000, 243),
    PACK_CCT_LEVEL(  6500, 249),
    PACK_CCT_LEVEL(  7000, 243),
    PACK_CCT_LEVEL(  9000, 225),
    PACK_CCT_LEVEL( 12000, 211),
    PACK_CCT_LEVEL( 18000, 199),
    PACK_CCT_LEVEL( 25000, 193),
    PACK_CCT_LEVEL( 38000, 188)
};

/* LUT for Blue */
static const uint16 cct_blue_lut[] =
{
    PACK_CCT_LEVEL( 1500,   0),
    PACK_CCT_LEVEL( 2000,  18),
    PACK_CCT_LEVEL( 2500,  72),
    PACK_CCT_LEVEL( 3000, 107),
    PACK_CCT_LEVEL( 4000, 163),
    PACK_CCT_LEVEL( 5000, 206),
    PACK_CCT_LEVEL( 6000, 239),
    PACK_CCT_LEVEL( 6500, 253),
    PACK_CCT_LEVEL( 7000, 255)
};

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
/* Function to convert Color temperature to level from LUT */
static uint8 getLevelFromCCTLut(uint16 temp, const uint16 *color_lut,
                                             uint16 sizeof_lut);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      getLevelFromCCTLut
 *
 *  DESCRIPTION
 *      This function linearly interpolates the color value from LUT and
 *      color temperature.
 *
 *  RETURNS
 *      Level of the colour from 0 - 255.
 *
 *---------------------------------------------------------------------------*/
static uint8 getLevelFromCCTLut(uint16 temp, const uint16 *color_lut,
                                             uint16 sizeof_lut)
{
    uint16 idx;
    int16  x2,y2,x1,y1;
    int32  val;
    uint16 thk = temp/CCT_TEMP_FACTOR;

    for (idx = 0; idx < sizeof_lut; idx++)
    {
        /* Find the temperature just greater than temperature to be set */
        if (thk < GET_TEMP(color_lut[idx]))
        {
            if (0 == idx)
            {
                /* if the temperature is less than first LUT element
                 * then saturate at that value.
                 */
                return GET_LEVEL(color_lut[idx]);
            }
            else
            {
                /* Get the points on interpolation line
                 * Multiply temperature by unit factor to get value
                 * in kelvin.
                 */
                y1 = (int16)GET_LEVEL(color_lut[idx - 1]);
                x1 = (int16)GET_TEMP(color_lut[idx - 1]) * CCT_TEMP_FACTOR;
                y2 = (int16)GET_LEVEL(color_lut[idx]);
                x2 = (int16)GET_TEMP(color_lut[idx]) * CCT_TEMP_FACTOR;

                /* Apply Straight Line interpolation.
                 * y = y1 + ((y2 - y1) * (x -x1))/(x2 - x1)
                 */
                val = ((int32)(y2 - y1))*((int16)temp - x1);
                val = (val)/(x2 - x1) + y1;

                /* return the calculated value */
                return ((uint8)val);
            }
        }
    }

    /* If temperature is greater than last element in LUT,
     * saturate to the highest
     */
    return GET_LEVEL(color_lut[sizeof_lut - 1]);

}

#endif /* COLOUR_TEMP_ENABLED */

/*============================================================================*
 *  Public function definitions
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      LightHardwareInit
 *
 *  DESCRIPTION
 *      This function initializes the light hardware, like PIO, interface etc.
 *
 * PARAMETERS
 *      Nothing.
 *
 * RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void LightHardwareInit(void)
{
#ifdef GUNILAMP
    GuniLampInit();
#else /* IOT Board */
    IOTLightControlDeviceInit();
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      LightHardwarePowerControl
 *
 *  DESCRIPTION
 *      Controls the light device power.
 *
 * PARAMETERS
 *      power_on [in] Turns ON power if TRUE.
 *                    Turns OFF power if FALSE.
 *
 * RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void LightHardwarePowerControl(bool power_on)
{
#ifdef GUNILAMP
    /* There is no control for Gunilamp power.
     * If Power OFF is requested, then set all the intensities to Zero.
     */
    if (power_on == FALSE)
    {
        GuniLampControl(0, 0, 0, 0);
    }
#else /* IOT Board */
    IOTLightControlDevicePower(power_on);
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      LightHardwareSetColor
 *
 *  DESCRIPTION
 *      Controls the color of the light.
 *
 * PARAMETERS
 *      red   [in] 0-255 levels of Red colour component.
 *      green [in] 0-255 levels of Green colour component.
 *      blue  [in] 0-255 levels of Blue colour component.
 *
 * RETURNS
 *      TRUE  if set color is supported by device.
 *      FALSE if it device is a white light or mono-chromatic light.
 *
 *----------------------------------------------------------------------------*/
extern bool LightHardwareSetColor(uint8 red, uint8 green, uint8 blue)
{
    bool status = FALSE;

#ifdef GUNILAMP
    status = TRUE;
    GuniLampControl(red, green, blue, 0);
#else /* IOT Board */
    status = TRUE;
    IOTLightControlDeviceSetColor(red, green, blue);
#endif

    return status;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      LightHardwareSetLevel
 *
 *  DESCRIPTION
 *      Controls the brightness of the light.
 *
 * PARAMETERS
 *      red   [in] 0-255 levels of Red colour component.
 *      green [in] 0-255 levels of Green colour component.
 *      blue  [in] 0-255 levels of Blue colour component.
 *      level [in] 0-255 levels of intensity.
 *
 * RETURNS
 *      Nothing.
 *
 *----------------------------------------------------------------------------*/
extern void LightHardwareSetLevel(uint8 red, uint8 green, uint8 blue, 
                                  uint8 level)
{
#ifdef GUNILAMP
    GuniLampControl(red, green, blue, level);
#else /* IOT Board */
    /* The brightness level is represented through RGB values */
    uint32 temp;
    temp  = (uint32)(red * level)/255;
    red = temp & 0xFF;
    temp  = (uint32)(green * level)/255;
    green = temp & 0xFF;
    temp  = (uint32)(blue * level)/255;
    blue = temp & 0xFF;
    IOTLightControlDeviceSetColor(red, green, blue);
#endif
}

#ifdef COLOUR_TEMP_ENABLED    
/*----------------------------------------------------------------------------*
 *  NAME
 *      LightHardwareSetColorTemp
 *
 *  DESCRIPTION
 *      Controls the colour temperature.
 *
 * PARAMETERS
 *      temp   [in] Colour temperature in kelvin.
 *
 * RETURNS
 *      TRUE  if Colour Temperature setting is supported by device.
 *      FALSE if Colour Temperature setting is NOT supported.
 *
 *----------------------------------------------------------------------------*/
extern bool LightHardwareSetColorTemp(uint16 temp)
{
    uint8  red, green, blue;

    red   = getLevelFromCCTLut(temp, cct_red_lut, LUT_SIZE(cct_red_lut));
    green = getLevelFromCCTLut(temp, cct_green_lut, LUT_SIZE(cct_green_lut));
    blue  = getLevelFromCCTLut(temp, cct_blue_lut, LUT_SIZE(cct_blue_lut));

    return LightHardwareSetColor(red, green, blue);
}
#endif /* COLOUR_TEMP_ENABLED */    

/*----------------------------------------------------------------------------*
 *  NAME
 *      LightHardwareSetBlink
 *
 *  DESCRIPTION
 *      Controls the blink colour and duration of light.
 *
 * PARAMETERS
 *      red      [in] 0-255 levels of Red colour component.
 *      green    [in] 0-255 levels of Green colour component.
 *      blue     [in] 0-255 levels of Blue colour component.
 *      on_time  [in] ON duration in multiples of 16ms.
 *      off_time [in] OFF duration in multiples of 16ms.
 *
 * RETURNS
 *      TRUE  if light blink is supported by device.
 *      FALSE if light blink is NOT supported.
 *
 *----------------------------------------------------------------------------*/
extern bool LightHardwareSetBlink(uint8 red, uint8 green, uint8 blue,
                                  uint8 on_time, uint8 off_time)
{
    bool status = FALSE;

#ifdef GUNILAMP
    /* For Gunilamp on/off time is not supported,
     *  so we still return FALSE.
     */
    GuniLampBlink(red, green, blue);
#else /* IOT Board */
    status = TRUE;
    IOTLightControlDeviceBlink(red, green, blue, on_time, off_time);
#endif

    return status;
}

