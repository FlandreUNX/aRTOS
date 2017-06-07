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
 * File      : hal_Support.h
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

#ifndef HAL_SUPPORT_H_
#define HAL_SUPPORT_H_

/**
 * @addtogroup stm32F7 configure
 */

/*@{*/

/**
 *  STM32F767ZI配置说明
 *  使用FLASH-ITCM总线模式(0x00200000 -0x200000 /2MB) (0x08000000 -0x200000 /2MB) 
 *  启动ART + ART-PF
 *  主内存使用DTCM-RAM(0x20000000 -0x20000 /128KB)
 *  SRAM1(0x20020000 -0x60000 /368KB)
 *  SRAM2(0x2007C000 -0x4000 /16KB)
 */
extern int Image$$RW_IRAM2$$ZI$$Limit;
#define IRAM_BEGIN  ((void *) &Image$$RW_IRAM2$$ZI$$Limit)   /**< 内存开始地址 */
#define IRAM_END  (0x20000000 + 0x20000)                      /**< 内存结束地址 */

/*@}*/

/**
 * @addtogroup hal user functions
 */

/*@{*/

extern void osHal_CoreInit(void);

extern uint32_t osHal_GetSysclk(void);

/*@}*/

/**
 * @addtogroup platform support system functions 
 */
 
/*@{*/

extern void hal_SystickConfig(void);

extern void hal_PendSVSet(void);

extern void hal_CallNMI(void);

/*@}*/

#endif
