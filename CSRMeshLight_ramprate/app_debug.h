/******************************************************************************
 *  Copyright Cambridge Silicon Radio Limited 2015
 *  CSR Bluetooth Low Energy CSRmesh 1.3 Release
 *  Application version 1.3
 *
 *  FILE
 *      app_debug.h
 *
 *  DESCRIPTION
 *      Header definitions for debug defines
 *
 ******************************************************************************/

#ifndef __APP_DEBUG_H__
#define __APP_DEBUG_H__

#ifdef DEBUG_ENABLE

#include <debug.h>

#define DEBUG_STR(s)  DebugWriteString(s)
#define DEBUG_U32(u)  DebugWriteUint32(u)
#define DEBUG_U16(u)  DebugWriteUint16(u)
#define DEBUG_U8(u)   DebugWriteUint8(u)

#else

#define DEBUG_STR(s)
#define DEBUG_U32(u)
#define DEBUG_U16(u)
#define DEBUG_U8(u)

#endif /* DEBUG_ENABLE */

#endif /* __APP_DEBUG_H__ */