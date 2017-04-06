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
 * File      : thread.h
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

#ifndef THREAD_H_
#define THREAD_H_

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "./timer.h"

#include "../arch/platform.h"

#include "../lib/list.h"

#include "../osConfig.h"

/*@}*/

/**
 * @addtogroup Thread标志符
 */
 
/*@{*/

/**
 *  Thread状态
 *  @note 用于标记线程运行状态
 */
typedef enum {
    osThreadReady,      /**< 就绪 */
    osThreadSuspend,    /**< 挂起 */
    osThreadRunning,    /**< 运行中 */
    osThreadBlocked,    /**< 堵塞 */
    osThreadTerminated  /**< 结束 */
}osThread_Stage_t;

/*@}*/

/**
 * @addtogroup Thread描述符
 */
 
/*@{*/

typedef struct Thread_Attr {
    /**
     *  Thread堆栈储存指针
     *  @note 禁止变更位置
     */
    void *stackTop;      /**< Thread栈顶指针 */
    void *stackEnd;      /**< Thread栈底指针 */
    uint16_t stackSize;  /**< Thread栈大小 */

    void *functions;    /**< Thread运行主体 */
    void *arguments;    /**< Thread主体变量 */

    /**
     *  Thread优先级
     */
    uint8_t priority;   /**< 优先级 */

    #if MAX_PRIORITY_LEVEL > 32
        uint8_t bitmap_Low_Mask;    /**< low bitmap 标志 */
        uint8_t bitmap_High_Mask;   /**< high bitmap 标志 */
    #endif

    #if MAX_PRIORITY_LEVEL <= 8
        uint8_t bitmap_Mask;    /**< 8优先级以内bitmap标志 */
    #else
        uint32_t bitmap_Mask;   /**< 大于8级的bitmap标志 */
    #endif

    /**
     *  其他属性
     */
    struct osList_Head_t list;    /**< 链表节点 */

    osTimer_Attr_t timer;    /**< 运行时核心计数器 */

    uint16_t initTimeSlice;     /**< 初始时间片大小 */
    uint16_t timeSlice;         /**< 时间片大小 */

    osThread_Stage_t stage;    /**< 状态 */
}osThread_Attr_t;

/**
 *  Thread全局句柄
 */
typedef void* osThread_ID;

/*@}*/

/**
 * @addtogroup thread user functions
 */
 
/*@{*/

extern osThread_ID osThread_Create(osThread_Attr_t *thread, void *argument);

extern void osThread_Ready(osThread_ID id);
extern void osThread_Suspend(osThread_ID id);

extern void osThread_Delay(uint32_t tick);

extern osThread_ID osThread_Self(void);

extern void osThread_Yield(void);

/*@}*/

#endif
