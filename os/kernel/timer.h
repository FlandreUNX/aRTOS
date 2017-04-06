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
 * File      : timer.h
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

#ifndef TIMER_H_
#define TIMER_H_

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "../osConfig.h"

#include "../lib/list.h"

#include "../arch/platform.h"

/*@}*/

/**
 * @addtogroup timer Configure
 */
 
/*@{*/

#if USING_SOFT_TIMER == 1
    typedef enum {
        osTimerHard = 0x01,     /**< 硬模式 */
        osTimerSoft = 0x02      /**< 软模式*/
    }osTimer_Mode_t;
#else
    typedef enum {
        osTimerHard = 0x01
    }osTimer_Mode_t;
#endif

typedef enum {
    osTimerOnce = 0x01,     /**< 单次模式 */
    osTimerPeriodic = 0x02  /**< 循环模式 */
}osTimer_Flag_t;
  
/*@}*/

/**
 * @addtogroup timer stage flag
 */
 
/*@{*/

/**
 *  Timer运行状态
 */
typedef enum {
    osTimerRunning = 0x01,     /**< 正在运行 */
    osTimerStop = 0x02         /**< 停止 */
}osTimer_Stage_t;

/*@}*/

/**
 * @addtogroup timer struct define
 */

/*@{*/

/**
 *  Timer描述符
 */
typedef struct osTimer {
    uint32_t perTick;       /**< 每周期tick */
    uint32_t timeoutTick;   /**< 超时时的tick */

    struct osList_Head_t list;      /**< 节点 */

    void (*callback)(void *arguments);     /**< 超时时的回调函数 */
    void *arguments;                        /**< 回调函数传入参数 */

    osTimer_Mode_t mode;         /**< 定时器依赖模式寄存器 */
    osTimer_Flag_t flag;         /**< 定时器运行模式寄存器 */
    osTimer_Stage_t stage;      /**< 定时器状态寄存器 */
}osTimer_Attr_t;

/**
 *  Timer全局句柄
 */
typedef void* osTimer_ID;

/*@}*/

/**
 * @addtogroup timer system functions
 */

/*@{*/

extern void timer_Init(void);

extern void timer_TickCheck(void);

/*@}*/

/**
 * @addtogroup timer user functions
 */
 
/*@{*/

extern osTimer_ID osTimer_Create(osTimer_Attr_t *obj, osTimer_Flag_t flag, void *arguments);

extern void osTimer_SetTick(osTimer_ID id, uint32_t tick);

extern void osTimer_Start(osTimer_ID id, uint32_t tick);
extern void osTimer_Stop(osTimer_ID id);

/*@}*/

#endif
