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

#include "../arch/util.h"

/*@}*/

/**
 * @addtogroup Thread标志符
 */
 
/*@{*/

/**
 *  Thread状态
 *  @note 用于标记任务运行状态
 */
enum osThread_Stage_t {
    Ready,      /**< 就绪 */
    Suspend,    /**< 挂起 */
    Running,    /**< 运行中 */
    Blocked,    /**< 堵塞 */
    Terminated  /**< 结束 */
};

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

    /**
     *  Thread优先级
     */
    uint8_t priority;

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
    osList_Head_t list;    /**< 链表节点 */

    //osTimer_t timer;    /**< 运行时核心计数器 */

    uint16_t timeSlice;     /**< 时间片大小 */
    uint16_t initTimeSlice;  /**< 初始时间片大小 */

    enum osThread_Stage_t stage;    /**< 状态 */
}osThread_Attr_t;

/**
 *  Thread全局句柄
 */
typedef void* osThread_ID_t;

/**
 *  Thread创建器
 */
#define osThread_Def(name, priority, stackSize, function) \
    osThread_Attr_t os_Thread_##name = { \
        .initTimeSlice = 1, \
        .functions = (void *)function,  \
        .stackSize = stackSize, \
        .priority = priority    \
    }; \
    static OS_NO_RETURN name(void *argument)

/*@}*/

#endif