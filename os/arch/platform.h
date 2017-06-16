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
 * File      : platform.h
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

#ifndef PLATFORM_H_
#define PLATFORM_H_

/**
 * @addtogroup ANSI-C Include
 */

/*@{*/

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

/*@}*/

/**
 * @addtogroup 32位CPU相关
 */
 
/*@{*/

typedef uint32_t  osTick_t;  /**< osTick最大计量 */

#define CPU_UINT32_MAX      (0xFFFFFFFF)                /**< Maxium number of UINT32 */
#define CPU_TICK_MAX        (uint32_t)(CPU_UINT32_MAX)  /**< Maxium number of tick */

#define OS_WAIT_FOREVER     (CPU_TICK_MAX)    /**< 系统无限等待数 */

#define MAGIC_WORD          (uint32_t)(0xE25A2EA5)      /**< 魔幻数,用于检验thread的堆栈溢出 */

#define BYTE_ALIGNMENT_MASK     (8)      /**< 对齐字节 */

/*@}*/

/**
 * @addtogroup Platform configure
 */
 
/*@{*/

/**
 *  是否启动VFP浮点模块
 *  @note 启动锁即其余线程无法输出LOG
 */
#define CM_VFP_ENABLE     (1)

/*@}*/

/**
 * @addtogroup 编译平台相关
 */
 
/*@{*/

/**
 *  KEIL CMSIS平台
 *  @note #include "cmsis_armcc.h"
 *  @note 对于keil平台,结构对齐建议使用"#pragma pack()"
 */
#ifdef __CC_ARM 

#include "cmsis_armcc.h"

#include "./hal_Support.h"

/**
 *  none
 *  @note none
 */
#define OS_SECTION(x)      __attribute__((section(x)))
#define OS_WEEK            __weak
#define OS_INLINE          __inline
#define OS_NO_RETURN       int __declspec(noreturn)

/**
 *  内存堆起止
 *  @note none
 */
#define HEAP_BEGIN  IRAM_BEGIN
#define HEAP_END    IRAM_END
#endif

/*@}*/

/**
 * @addtogroup 辅助数学函数
 */
 
/*@{*/

/**
 * @ingroup BasicDef
 *
 * @def ALIGN(number, align)
 *
 * 向上计算以number为底数,align的x2迭代的对齐值 
 * 例如:ALIGN(13, 4) = 16
 */
#define ALIGN(number, align) \
  (((number) + (align) - 1) & ~((align) - 1))


/**
 * @ingroup BasicDef
 *
 * @def ALIGN_DOWN(number, align)
 *
 * 向下计算以number为底数,align的x2迭代的对齐值
 * 例如:ALIGN_DOWN(13, 4) = 12
 */
#define ALIGN_DOWN(number, align) \
  ((number) & ~((align) - 1))

/*@}*/

#endif
