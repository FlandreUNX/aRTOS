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
 * File      : osAPI.h
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

#ifndef OSAPI_H_
#define OSAPI_H_

/**
 * @addtogroup ------------------Platform Arch------------------
 */

/*@{*/

/*@}*/

/**
 * @addtogroup ------------------Memory manager------------------
 */

/*@{*/

#include "./mm/buddy.h"


/**
 * 申请内存
 *
 * @param size 申请大小
 * 
 * @return 内存地址
 */
extern void* osMem_Malloc(uint32_t size);


/**
 * 释放内存
 *
 * @param address 地址
 * 
 * @return none
 */
extern void osMem_Free(void* address);

/*@}*/

/**
 * @addtogroup ------------------Schedule manager------------------
 */

/*@{*/

#include "./kernel/schedule.h"


/**
 * 锁定调度器
 *
 * @param none
 * 
 * @return none
 */
extern void osSche_Lock(void);


/**
 * 解锁调度器
 *
 * @param none
 * 
 * @return none
 */
extern void osSche_Unlock(void);

/*@}*/

/**
 * @addtogroup ------------------Thread manager------------------
 */

/*@{*/

#include "./kernel/thread.h"


/**
 * 线程运行时
 *
 * @param name 名称
 * 
 * @return NO_RETURN
 */
#define osThread_Func(name) \
    static OS_NO_RETURN name(void *argument)


/**
 * 定义线程
 *
 * @param name 名称
 * @param priority 优先级
 * @param stackSize 堆栈大小
 * @param function 运行方法
 * 
 * @return none
 */
#define osThread_Def(name, priority, stackSize, function) \
    osThread_Attr_t os_Thread_##name = { \
        .initTimeSlice = 1, \
        .functions = (void *)function, \
        .stackSize = stackSize, \
        .priority = priority \
    };


/**
 * 线程对象
 *
 * @param name 名称
 * 
 * @return none
 */
#define osThread_Obj(name) \
    &os_Thread_##name

/*@}*/

/**
 * @addtogroup ------------------Timer manager------------------
 */

/*@{*/

#include "./kernel/timer.h"


/**
 * 定时器超时处理函数
 *
 * @param name  名称
 * 
 * @return none
 */
#define osTimer_Callback(name) \
    void name(void *arguments)


/**
 * 定义定时器
 *
 * @param name 名称
 * @param mode 运行模式(hard|soft)
 * @param callback 超时回调函数
 * @param arguments 回调函数传入参数
 * 
 * @return none
 */
#define osTimer_Def(name, mode, callback) \
    osTimer_Attr_t os_Timer_##name = { \
        .mode = mode, \
        .callback = callback \
    };


/**
 * 定时器对象
 *
 * @param name 名称
 * 
 * @return none
 */
#define osTimer_Obj(name) \
    &os_Timer_##name

/*@}*/

#endif
