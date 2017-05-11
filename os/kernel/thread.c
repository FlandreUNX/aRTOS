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
 * File      : thread.c
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

#include "./thread.h"

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "../arch/platform.h"
#include "../arch/hal_cm.h"

#include "../mm/buddy.h"

#include "../lib/list.h"
#include "../lib/symbolExport.h"

#include "../osConfig.h"

#include "./schedule.h"

/*@}*/

/**
 * @addtogroup system extern
 */

/*@{*/

extern volatile osThread_Attr_t *sche_NowThread, *sche_NextThread;

extern struct osList_Head_t sche_ReadyList[MAX_PRIORITY_LEVEL];

/*@}*/

/**
 * @addtogroup thread system functions
 */

/*@{*/

void thread_TimerCallback(void *arguments) {
    //OS_ASSERT

    osThread_Attr_t *thread = (osThread_Attr_t *)arguments;

    /*标记线程就绪态*/
    thread->stage = osThreadReady;

    /*插入调度器,切换下一线程*/
    sche_InsertThread(thread);
    sche_ToNextThread();
}

/*@}*/

/**
 * @addtogroup thread user functions
 */

/*@{*/

/**
 * 创建线程
 *
 * @param thread 线程对象
 * @param argument 入口函数的传入参数
 *
 * @return 线程句柄
 */
osThread_ID osThread_Create(osThread_Attr_t *thread, void *argument) {
    //OS_ASSERT

    /*调整堆栈大小,4字节对齐*/
    thread->stackSize = ALIGN(thread->stackSize, 4);
    //OS_ASSERT

    /*申请栈空间*/
    thread->stackEnd = (void *)osMem_Malloc(thread->stackSize);
    if (thread->stackEnd == NULL) {
        //OS_ASSERT

        return 0;
    }

    /*最后栈底最后一个字节写入一个固定数值,用于检测栈溢出*/
    *((uint32_t*) thread->stackEnd) = (uint32_t) MAGIC_WORD;

    /*设置栈顶*/
    thread->stackTop = (void *)((uint32_t)thread->stackEnd + thread->stackSize);

    /*初始化线程CPU栈寄存器*/
    thread->stackTop = cpu_SetupRegisters(thread->functions, argument, thread->stackTop);
    thread->arguments = argument;

    /*超出最大设定组别*/
    if (thread->priority > MAX_PRIORITY_LEVEL) {
        //OS_ASSERT

        thread->priority = MAX_PRIORITY_LEVEL;
    }

    /*确定线程在优先bitmap的位置*/
    #if MAX_PRIORITY_LEVEL > 32
        thread->bitmap_Low_Mask = thread->priority >> 3;
        thread->bitmap_High_Mask = 1L << (thread->priority & 0x07);
        thread->bitmap_Mask = 1L << thread->bitmap_Low_Mask;
    #else
        thread->bitmap_Mask = 1L << thread->priority;
    #endif
   
    /*初始化节点*/
    osList_HeadInit(&(thread->list));

    /*初始化依赖定时器*/
    thread->timer.callback = thread_TimerCallback;
    thread->timer.mode = osTimerHard;
    osTimer_Create(&(thread->timer), osTimerOnce, (void *)thread);

    /*设置其他参数*/
    thread->stage = osThreadSuspend;
    thread->timeSlice = thread->initTimeSlice;

    return (osThread_ID)thread;
}
EXPORT_SYMBOL(osThread_Create);


/**
 * 就绪线程
 * 除了osThreadTerminated|osThreadReady,就绪任务会强制退出其他状态
 *
 * @param id 线程句柄
 *
 * @return none
 */
void osThread_Ready(osThread_ID id) {
    //OS_ASSERT

    /*关中断*/
    register uint32_t level;
    level = hal_DisableINT();

    osThread_Attr_t *thread = (osThread_Attr_t *)id;

    /*标记就绪态,并加入调度行列*/
    if (thread->stage == osThreadTerminated ||
        thread->stage == osThreadReady) {
        //OS_ASSERT

        hal_EnableINT(level);

        return;
    }
    thread->stage = osThreadReady;

    /*关闭定时器*/
    osTimer_Stop(&(thread->timer));

    /*插入调度器*/
    sche_InsertThread(thread);

    hal_EnableINT(level);
}
EXPORT_SYMBOL(osThread_Ready);


/**
 * 挂起线程
 * 除了osThreadTerminated|osThreadSuspend,挂起任务会强制退出其他状态
 *
 * @param id 线程句柄
 *
 * @return none
 */
void osThread_Suspend(osThread_ID id) {
    //OS_ASSERT

    /*关中断*/
    register uint32_t level;
    level = hal_DisableINT();

    osThread_Attr_t *thread = (osThread_Attr_t *)id;

    /*标记挂起状态*/
    if (thread->stage == osThreadSuspend ||
        thread->stage == osThreadTerminated) {
        //OS_ASSERT

        /*开中断*/
         hal_EnableINT(level);

        return;
    }
    thread->stage = osThreadSuspend;

    /*清除定时器状态*/
    osTimer_Stop(&(thread->timer));

    /*调度器移除线程*/
    sche_RemoveThread(thread);

    /*开中断*/
    hal_EnableINT(level);
}
EXPORT_SYMBOL(osThread_Suspend);


/**
 * 返回当前线程自己的句柄
 *
 * @param none
 *
 * @return 线程句柄
 */
osThread_ID osThread_Self(void) {
    /*关中断*/
    register uint32_t level;
    level = hal_DisableINT();

    osThread_ID self = (osThread_ID)sche_NowThread;

    /*开中断*/
    hal_EnableINT(level);

    return self;
}
EXPORT_SYMBOL(osThread_Self);


/**
 * 主动放弃CPU占用
 *
 * @param none
 *
 * @return none
 */
void osThread_Yield(void) {
    /*关中断*/
    register uint32_t level;
    level = hal_DisableINT();

    osThread_Attr_t *thread = (osThread_Attr_t *)sche_NowThread;

    /*标记就绪态*/
    thread->stage = osThreadReady;

    /*线程移至调度器容器末尾*/
    osList_DeleteNode(&(thread->list));
    osList_AddTail(&(sche_ReadyList[thread->priority]), &(thread->list));

    /*开中断*/
    hal_EnableINT(level);

    /*切换下一线程*/
    sche_ToNextThread();
}
EXPORT_SYMBOL(osThread_Yield);


/**
 * 对当前线程进行延时
 *
 * @param tick 延时数
 *
 * @return none
 */
void osThread_Delay(osTick_t tick) {
    /*关中断*/
    register uint32_t level;
    level = hal_DisableINT();

    /*获取当前线程*/
    osThread_Attr_t *thread = (osThread_Attr_t *)sche_NowThread;
    /*标记堵塞态*/
    thread->stage = osThreadBlocked;

    /*移除调度*/
    sche_RemoveThread(thread);

    /*设置堵塞tick*/
    osTimer_Start(&(thread->timer), tick);

    /*进行一次调度*/
    sche_ToNextThread();

    /*开中断*/
    hal_EnableINT(level);
}

/*@}*/
