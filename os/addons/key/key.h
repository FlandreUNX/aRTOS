/**　　　　　　 ,-...　　　 　 　 　　　..-.
 *　　　　　 ./:::::＼　　　　 　 　  ／:::::ヽ
 *　　　　  /::::::::::::;ゝ--──-- .._/:::::::ヽ
 *　　　　 /,.-‐''"′                  ＼:::::::|
 *　　　  ／　 　　　　　　　　　　　　  ヽ.::|
 *　　　　/　　　　●　　　 　 　 　 　 　 ヽ|
 *　　 　 l　　　...　　 　 　 　  ●　      l
 *　　　 .|　　　 　　 (_人__丿   ...　    |
 *　 　 　l　　　　　　　　　　　　 　　  l
 *　　　　` .　　　　　　　　 　 　 　　 /
 *　　　　　　`. .__　　　 　 　 　　.／
 *　　　　　　　　　/`'''.‐‐──‐‐‐┬---
 * File      : key.h
 * This file is part of aRTOS
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
 
#ifndef KEY_H_
#define KEY_H_

/**
 * @addtogroup std C lib include
 */

/*@{*/

#include <stdint.h>

/*@}*/

/**
 * @addtogroup os include
 */

/*@{*/

#include "../../arch/platform.h"

/*@}*/

/**
 * @addtogroup Key function configure
 */
 
/*@{*/

/**
 *  按键数目
 *  @note none
 */
#define KEY_NUMBER        (4)

/**
 *  是否使用长按键功能
 *  @note none
 */
#define LONG_KEY_EN       (1)

/**
 *  是否使用shift按键
 *  @note none
 */
#define USE_SHIFT_KEY     (0)

#if LONG_KEY_EN == 1
/**
 *  长按键判断周期
 *  @note 按键扫描周期 * KEY_PRESS_DLY = 长按判定
 */
#define KEY_PRESS_DLY     (20)

/*长按键有效周期*/
/**
 *  长按键判断周期
 *  @note 按键扫描周期 * KEY_PRESS_TMR = 长按发射一次
 */
#define KEY_PRESS_TMR     (3)
#endif

/**
 *  按键缓冲区大小
 *  @note none
 */
#define KEY_BUFFER_SIZE   (8)

/*@}*/

/**
 * @addtogroup 按照按键数确定bitmap
 */
 
/*@{*/
#if KEY_NUMBER < 9
typedef uint8_t mKey_Bitmap_t;
#elif KEY_NUMBER < 17
typedef uint16_t mKey_Bitmap_t;
#else
typedef uint32_t mKey_Bitmap_t;
#endif

/*@}*/

/**
 * @addtogroup Key static define
 */
 
/*@{*/

/**
 *  普通按键
 *  @note none
 */
#define KEY_OK_FLAG       (mKey_Bitmap_t)0x01
#define KEY_MENU_FLAG     (mKey_Bitmap_t)0x02
#define KEY_UP_FLAG       (mKey_Bitmap_t)0x04
#define KEY_DOWN_FLAG     (mKey_Bitmap_t)0x08

/**
 *  shift按键
 *  @note none
 */
#if USE_SHIFT_KEY == 1
#define KEY_SHIFT     (mKey_Bitmap_t)0x10
#endif

/**
 *  支持长按反码的按键,未添加的则为长按连发按键
 *  @note 例如:0x01的长按键输出码为0xFE即,短按00000001->长按11111110
 */
#if LONG_KEY_EN == 1
#define KEY_LONG_SHIFT    (KEY_OK_FLAG | KEY_MENU_FLAG)

/**
 *  长按消息定义
 *  @note 短按键的反码
 */
#define KEY_OK_LONG_FLAG    (mKey_Bitmap_t)0xFE
#define KEY_MENU_LONG_FLAG  (mKey_Bitmap_t)0xFD
#endif

/*@}*/

/**
 * @addtogroup 按键 signal event
 */
 
/*@{*/

#define KEY_OK_SIGNAL           0x20
#define KEY_MENU_SIGNAL         0x21
#define KEY_UP_SIGNAL           0x22
#define KEY_DOWN_SIGNAL         0x23

#define KEY_OK_LONG_SIGNAL      0x24
#define KEY_MENU_LONG_SIGNAL    0x25

#define KEY_UP_DOWN_SIGNAL      0x28

/*@}*/

/**
 * @addtogroup Key functions
 */
 
/*@{*/

extern void mKey_InterfaceInit(void);

extern void mKey_Scan(void);

extern mKey_Bitmap_t mKey_Get(void);

extern uint8_t mKey_IsHit(void);

/*@}*/

#endif
