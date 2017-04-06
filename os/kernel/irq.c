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
 * File      : irq.c
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

#include "./irq.h"

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "../arch/hal_cm.h"

#include "../lib/symbolExport.h"

/*@}*/

/**
 * irq.c 函数用于OS跟其他额外组件进行交互
 * 例如BSP中断交互,或者是TCP/IP模组的交互
 */
 
/**
 * @addtogroup irq vars
 */
 
/*@{*/

static volatile int8_t interruptNest = 0;

/*@}*/

/**
 * @addtogroup irq user functions
 */
 
/*@{*/

/**
 * 退出中断后调用
 *
 * @param none
 *
 * @return none
 */
void osSys_ISRLeave(void) {
    register uint32_t level;
    level = hal_DisableINT();

    interruptNest --;

    hal_EnableINT(level);
}
EXPORT_SYMBOL(osSys_ISRLeave);


/**
 * 进入中断后调用
 *
 * @param none
 *
 * @return none
 */
void osSys_ISREnter(void) {
    register uint32_t level;
    level = hal_DisableINT();

    interruptNest ++;

    hal_EnableINT(level);
}
EXPORT_SYMBOL(osSys_ISREnter);


/**
 * 获取中断嵌套数
 *
 * @param none
 *
 * @return 中断嵌套数
 */
int8_t osSys_GetNest(void) {
    return interruptNest;
}
EXPORT_SYMBOL(osSys_GetNest);

/*@}*/
