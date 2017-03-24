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
 * File      : msp432.c
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

#include "./msp432.h"

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "../util.h"

/*@}*/

/**
 * @addtogroup ARCH Include
 */

/*@{*/

#include "./driverlib/driverlib.h"

/*@}*/

/**
 * @addtogroup os运行必要接口
 */

/*@{*/

/**
 * 关闭全局中断
 *
 * @param 无
 * 
 * @return 无
 */
__asm uint32_t hal_DisableINT(void) {
    PRESERVE8

    mrs r0, basepri
    mov r1, #MAX_SYSCALL_INTERRUPT_PRIORITY
    msr basepri, r1

    dsb
    isb

    bx r14
}


/**
 * 开启全局中断
 *
 * @param 无
 * 
 * @return 无
 */
__asm void hal_EnableINT(uint32_t level) {
	PRESERVE8
	
	msr basepri, r0
	bx r14
}


/**
 * 标志pensv异常
 *
 * @param 无
 * 
 * @return 无
 */
void hal_CallPendSV(void) {
    Interrupt_pendInterrupt(FAULT_PENDSV);
}


/**
 * 进入NMI异常
 *
 * @param 无
 * 
 * @return 无
 */
void hal_CallNMI(void) {
    Interrupt_pendInterrupt(FAULT_NMI);
}

/*@}*/
