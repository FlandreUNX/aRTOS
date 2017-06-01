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
 * @addtogroup ------------------os函数定义方式------------------
 */

/*@{*/

/**
 *  1. 带os + osXx_Xxx的为用户函数
 *  2. 不带os + xx_Xxx的为系统内部函数
 *  3. 不带os + aaBbCc的为模块内联函数
 */

/*@}*/

/**
 * @addtogroup ------------------Platform Arch------------------
 */

/*@{*/

/**
 *  @brief 硬件初始化
 *
 *  @param none
 * 
 *  @retval none
 */
extern void osHal_CoreInit(void);

/*@}*/

/**
 * @addtogroup ------------------System manager------------------
 */

/*@{*/

/**
 * 初始化os内核
 *
 * @param none
 *
 * @retval none
 */
extern void osSys_KernelInitialize(void);


/**
 * 启动os
 *
 * @param none
 *
 * @retval none
 */
extern void osSys_KernelStartup(void);

/*@}*/

/**
 * @addtogroup ------------------Memory manager------------------
 */
 
/*@{*/

/**
 *  core include file 
 */
#include "./mm/buddy.h"


/**
 * 申请内存
 *
 * @param size 申请大小
 * 
 * @retval 内存地址
 */
extern void* osMem_Malloc(uint32_t size);


/**
 * 释放内存
 *
 * @param address 地址
 * 
 * @retval none
 */
extern void osMem_Free(void* address);

/*@}*/

/**
 * @addtogroup ------------------Schedule manager------------------
 */

/*@{*/

/**
 *  core include file 
 */
#include "./kernel/schedule.h"


/**
 * 锁定调度器
 *
 * @param none
 * 
 * @retval none
 */
extern void osSche_Lock(void);


/**
 * 解锁调度器
 *
 * @param none
 * 
 * @retval none
 */
extern void osSche_Unlock(void);

/*@}*/

/**
 * @addtogroup ------------------Thread manager------------------
 */

/*@{*/

/**
 *  core include file 
 */
#include "./kernel/thread.h"


/**
 * 线程运行时
 *
 * @param name 名称
 * 
 * @retval NO_RETURN
 */
#define osThread_FuncDef(name) \
  static OS_NO_RETURN os_ThreadFunc_##name(void *argument)


/**
 * 定义线程
 *
 * @param name 名称
 * @param pri 优先级
 * @param ss 堆栈大小
 * @param func 运行方法
 * 
 * @retval none
 */
#define osThread_Def(name, pri, ss, func) \
  osThread_Attr_t os_Thread_##name = { \
    .initTimeSlice = 1, \
    .functions = (void *)os_ThreadFunc_##func, \
    .stackSize = ss, \
    .priority = pri \
  };


/**
 * 线程对象
 *
 * @param name 名称
 * 
 * @retval none
 */
#define osThread_Obj(name) \
  &os_Thread_##name


/**
 * 创建线程
 *
 * @param thread 线程对象
 * @param argument 入口函数的传入参数
 *
 * @retval 线程句柄
 */
extern osThread_ID osThread_Create(osThread_Attr_t *thread, void *argument);


/**
 * 就绪线程
 * 除了osThreadTerminated|osThreadReady,就绪任务会强制退出其他状态
 *
 * @param id 线程句柄
 *
 * @retval none
 */
extern void osThread_Ready(osThread_ID id);


/**
 * 挂起线程
 * 除了osThreadTerminated|osThreadSuspend,挂起任务会强制退出其他状态
 *
 * @param id 线程句柄
 *
 * @retval none
 */
extern void osThread_Suspend(osThread_ID id);


/**
 * 对当前线程进行延时
 *
 * @param tick 延时数
 *
 * @retval none
 */
extern void osThread_Delay(osTick_t tick);


/**
 * 返回当前线程自己的句柄
 *
 * @param none
 *
 * @retval 线程句柄
 */
extern osThread_ID osThread_Self(void);


/**
 * 主动放弃CPU占用
 *
 * @param none
 *
 * @retval none
 */
extern void osThread_Yield(void);

/*@}*/

/**
 * @addtogroup ------------------Timer manager------------------
 */

/*@{*/

/**
 *  core include file 
 */
#include "./kernel/timer.h"


/**
 * 定时器超时处理函数
 *
 * @param name  名称
 * 
 * @retval none
 */
#define osTimer_Callback(name) \
  static void os_TimerCB_##name(void *arguments)


/**
 * 定义定时器
 *
 * @param name 名称
 * @param Mode 运行模式(osTimerHard|osTimerSoft)
 * @param callback 超时回调函数
 * 
 * @retval none
 */
#define osTimer_Def(name, Mode, cb) \
  osTimer_Attr_t os_Timer_##name = { \
    .mode = Mode, \
    .callback = os_TimerCB_##cb \
  };


/**
 * 定时器对象
 *
 * @param name 名称
 * 
 * @retval none
 */
#define osTimer_Obj(name) \
  &os_Timer_##name


/**
 * 创建定时器
 *
 * @param obj 定时器对象
 * @param flag 模式(osTimerOnce|osTimerPeriodic)
 * @param arguments 回调函数传入参数
 * 
 * @retval 定时器句柄
 */
extern osTimer_ID osTimer_Create(osTimer_Attr_t *obj, osTimer_Flag flag, void *arguments);


/**
 * 设置的定时器周期
 *
 * @param id 定时器句柄
 * @param tick 周期长度
 * 
 * @retval none
 */
extern void osTimer_SetTick(osTimer_ID id, osTick_t tick);


/**
 * 获取定时器的剩余时间
 *
 * @param id 定时器句柄
 * 
 * @retval tick剩余值
 */
extern osTick_t osTimer_GetResidueTick(osTimer_ID id);


/**
 * 设置定时器回调函数的传入参数
 *
 * @param id 定时器句柄
 * @param arguments 参数指针
 * 
 * @retval none
 */
extern void osTimer_SetArgument(osTimer_ID id, void *arguments);


/**
 * 定时器启动
 *
 * @param id 定时器句柄
 * @param tick 定时器定时值
 * 
 * @retval none
 */
extern void osTimer_Start(osTimer_ID id, osTick_t tick);


/**
 * 定时器停止
 *
 * @param id 定时器句柄
 * 
 * @retval none
 */
extern void osTimer_Stop(osTimer_ID id);

/*@}*/

#endif
