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

extern struct threadSwitchInfo_t sche_ThreadSwitchStatus;

extern struct osList_t sche_ReadyList[MAX_PRIORITY_LEVEL];

/*@}*/

/**
 * @addtogroup thread system functions
 */

/*@{*/

void threadTimerCallback(void *arguments) {
  //OS_ASSERT

  osThread_Attr_t *thread = (osThread_Attr_t *)arguments;

  /*标记线程就绪态*/
  thread->state = osThreadReady;

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
 * @retval 线程句柄
 */
osThread_Id osThread_Create(osThread_Attr_t *thread, void *argument) {
  //OS_ASSERT
  uint32_t *stackTop;

  /*调整堆栈大小,字节对齐*/
  thread->stackSize = ALIGN(thread->stackSize, BYTE_ALIGNMENT_MASK);
  //OS_ASSERT

  /*申请栈空间*/
  thread->stackEnd = (void *)osMem_Malloc(thread->stackSize);
  if (thread->stackEnd == NULL) {
    //OS_ASSERT

    return 0;
  }
  /*栈底,字节向上对齐*/
  thread->stackEnd = (void *)ALIGN((uint32_t)thread->stackEnd, BYTE_ALIGNMENT_MASK);

  /*最后栈底最后一个字节写入一个固定数值,用于检测栈溢出*/
  *((uint32_t*) thread->stackEnd) = (uint32_t) MAGIC_WORD;

  /*栈顶,字节向下对齐*/
  stackTop = (uint32_t *)((uint32_t)thread->stackEnd + thread->stackSize);
  stackTop = (uint32_t *)ALIGN_DOWN((uint32_t)stackTop, BYTE_ALIGNMENT_MASK);

  /*初始化线程CPU栈寄存器*/
  stackTop = cpu_SetupRegisters(thread->functions, argument, stackTop);
  
  thread->stackTop = (void *)stackTop;
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
  thread->timer.callback = threadTimerCallback;
  thread->timer.mode = osTimerHard;
  osTimer_Create(&(thread->timer), osTimerOnce, (void *)thread);

  /*设置其他参数*/
  thread->state = osThreadSuspend;
  thread->timeSlice = thread->initTimeSlice;

  return (osThread_Id)thread;
}
EXPORT_SYMBOL(osThread_Create);


/**
 * 就绪线程
 * 除了osThreadTerminated|osThreadReady,就绪任务会强制退出其他状态
 *
 * @param id 线程句柄
 *
 * @retval none
 */
void osThread_Ready(osThread_Id id) {
  //OS_ASSERT

  /*关中断*/
  hal_DisableINT();

  osThread_Attr_t *thread = (osThread_Attr_t *)id;

  /*标记就绪态,并加入调度行列*/
  if (thread->state == osThreadTerminated ||
    thread->state == osThreadReady) {
    //OS_ASSERT

    hal_EnableINT();

    return;
  }
  thread->state = osThreadReady;

  /*关闭定时器*/
  osTimer_Stop(&(thread->timer));

  /*插入调度器*/
  sche_InsertThread(thread);

  hal_EnableINT();
}
EXPORT_SYMBOL(osThread_Ready);


/**
 * 挂起线程
 * 除了osThreadTerminated|osThreadSuspend,挂起任务会强制退出其他状态
 *
 * @param id 线程句柄
 *
 * @retval none
 */
void osThread_Suspend(osThread_Id id) {
  //OS_ASSERT

  /*关中断*/
  hal_DisableINT();

  osThread_Attr_t *thread = (osThread_Attr_t *)id;

  /*标记挂起状态*/
  if (thread->state == osThreadSuspend ||
    thread->state == osThreadTerminated) {
    //OS_ASSERT

    /*开中断*/
    hal_EnableINT();

    return;
  }
  thread->state = osThreadSuspend;

  /*清除定时器状态*/
  osTimer_Stop(&(thread->timer));

  /*调度器移除线程*/
  sche_RemoveThread(thread);

  /*开中断*/
  hal_EnableINT();
}
EXPORT_SYMBOL(osThread_Suspend);


/**
 * 终结线程
 *
 * @param id 线程句柄
 *
 * @retval none
 */
void osThread_Terminate(osThread_Id id) {
  
}
EXPORT_SYMBOL(osThread_Terminate);


/**
 * 返回当前线程自己的句柄
 *
 * @param none
 *
 * @retval 线程句柄
 */
osThread_Id osThread_Self(void) {
  /*关中断*/
  hal_DisableINT();

  osThread_Id self = (osThread_Id)sche_ThreadSwitchStatus.nowThread;

  /*开中断*/
  hal_EnableINT();

  return self;
}
EXPORT_SYMBOL(osThread_Self);


/**
 * 主动放弃CPU占用
 *
 * @param none
 *
 * @retval none
 */
void osThread_Yield(void) {
  /*关中断*/
  hal_DisableINT();

  osThread_Attr_t *thread = (osThread_Attr_t *)sche_ThreadSwitchStatus.nowThread;
  
  if (thread->state == osThreadReady || 
    thread->state == osThreadRunning) {
      /*就绪任务*/
      thread->state = osThreadReady;
        
      /*线程移至调度器容器末尾*/
      osList_DeleteNode(&(thread->list));
      osList_AddTail(&(sche_ReadyList[thread->priority]), &(thread->list));
  }

  /*开中断*/
  hal_EnableINT();

  /*切换下一线程*/
  sche_ToNextThread();
}
EXPORT_SYMBOL(osThread_Yield);


/**
 * 对当前线程进行延时
 *
 * @param tick 延时数
 *
 * @retval none
 */
void osThread_Delay(osTick_t tick) {
  if (tick == 0) {
    return;
  }
  
  /*关中断*/
  hal_DisableINT();

  /*获取当前线程*/
  osThread_Attr_t *thread = (osThread_Attr_t *)sche_ThreadSwitchStatus.nowThread;
  /*标记堵塞态*/
  thread->state = osThreadBlocked;

  /*移除调度*/
  sche_RemoveThread(thread);

  /*设置堵塞tick*/
  osTimer_Start(&(thread->timer), tick);

  /*进行一次调度*/
  sche_ToNextThread();

  /*开中断*/
  hal_EnableINT();
}

/*@}*/
