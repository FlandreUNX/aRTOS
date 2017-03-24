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
 * File      : msp432.h
 * This file is part of ACGrtos
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

#ifndef MSP432_H_
#define MSP432_H_

/**
 * @addtogroup 硬件核心相关
 */
 
/*@{*/

/**
 * CPU
 */
#define MCU_MCLK    (48000000)

/**
 * 内存堆起止
 */
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN  ((void *) &Image$$RW_IRAM1$$ZI$$Limit)
#define HEAP_END    (0x20000000 + 64 * 1024)

/**
 * IRQ
 */
#define MAX_SYSCALL_INTERRUPT_PRIORITY		(5) << (8 - 3)

/**
 * Systick 
 */
#define SYSTICK_TICK_PER_SEC    (1000)     /**<  1000 = 1ms systick计数1000次中断一次 */

/*@}*/

/**
 * @addtogroup irq
 */
 
/*@{*/

extern uint32_t hal_DisableINT(void);
extern void hal_EnableINT(uint32_t level);

extern void hal_CallNMI(void);
extern void hal_CallPendSV(void);

/*@}*/

#endif
