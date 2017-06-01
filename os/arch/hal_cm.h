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
 * File      : hal_cm.h
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
 
#ifndef HAL_CM_H_
#define HAL_CM_H_

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "./platform.h"

/*@}*/

/**
 * @addtogroup register struct 
 */
 
/*@{*/

/**
 *  arm cortext-m 寄存器结构
 */
typedef struct cm_RegisterFrame {
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  
  uint32_t exec;
  
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  
  uint32_t r12;
  
  uint32_t r14_LR;
  uint32_t r15_PC;
  uint32_t r13_SP;
} cpuRegisters_t;

/*@}*/

/**
 * @addtogroup cm IRQ configure
 */
 
/*@{*/

#define MAX_SYSCALL_INTERRUPT_PRIORITY		(5) << (8 - 3)

/*@}*/

/**
 * @addtogroup platform support system functions 
 */
 
/*@{*/

extern void hal_PendSVSet(void);

extern void hal_CallNMI(void);

/*@}*/

/**
 * @addtogroup cm_hal system functions 
 */
 
/*@{*/

extern uint32_t* cpu_SetupRegisters(void *func, void *arguments, uint32_t *stackTop);

extern void cpu_GotoFisrtTask(void);

extern uint32_t hal_DisableINT(void);
extern void hal_EnableINT(uint32_t level);

extern void hal_PendSVSet(void);
extern void hal_CallNMI(void);

/*@}*/

#endif
