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
 * @param name  名称
 * 
 * @return NO_RETURN
 */
#define osThread_Func(name) \
    static OS_NO_RETURN name(void *argument)


/**
 * 创建线程
 *
 * @param name  名称
 * @param priority  优先级
 * @param stackSize  堆栈大小
 * @param function  运行方法
 * 
 * @return none
 */
#define osThread_Def(name, priority, stackSize, function) \
    osThread_Attr_t os_Thread_##name = { \
        .initTimeSlice = 1, \
        .functions = (void *)function,  \
        .stackSize = stackSize, \
        .priority = priority    \
    };

/*@}*/

#endif
