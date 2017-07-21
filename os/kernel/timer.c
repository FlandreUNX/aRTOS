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
 * File      : timer.c
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

#include "./timer.h"

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "./schedule.h"
#include "./thread.h"

#include "../arch/hal_cm.h"

#include "../lib/symbolExport.h"

/*@}*/

/**
 * @addtogroup timer system variable
 */

/*@{*/

/**
 * 硬定时器计时循环队列
 * @note none
 */
struct osList_t timer_HardList;

#if USING_SOFT_TIMER == 1
/**
 * 软定时器计时循环队列
 * @note none
 */
struct osList_t timer_softList;

/**
 * 软定时器计时线程
 * @note none
 */
osThread_Id timer_SoftThreadID;
extern OS_NO_RETURN os_SoftTimer_Thread(void *argument);
osThread_Attr_t os_Thread_SoftTimer = { \
  .initTimeSlice = 1, \
  .functions = os_SoftTimer_Thread, \
  .stackSize = SOFT_TIMER_STACK_SIZE, \
  .priority = SOFT_TIMER_PRIORITY \
};
uint8_t os_Thread_Stack_SoftTimer[SOFT_TIMER_STACK_SIZE];
#endif

/*@}*/

/**
 * @addtogroup timer system functions
 */

/*@{*/

/**
 * 定时器初始化
 *
 * @param none
 * 
 * @retval none
 */
void timer_Init(void) {
  /*初始化定时器容器*/
  osList_HeadInit(&timer_HardList);

#if USING_SOFT_TIMER == 1
  /*初始化定时器容器*/
  osList_HeadInit(&timer_softList);

  /*初始化线程*/
  timer_SoftThreadID = osThread_StaticCreate(&os_Thread_SoftTimer, NULL, os_Thread_Stack_SoftTimer);
  osThread_Ready(timer_SoftThreadID);
#endif
}


/**
 * 定时器超时检查
 *
 * @param none
 * 
 * @retval none
 */
void timer_TickCheck(void) {
  /*关中断*/
  register uint32_t level = hal_DisableINT();
  
  /*获取最近的TICK*/
  osTick_t currentTick = osSys_GetNowTick();

  /*遍历list*/
  osTimer_Attr_t *timer;
  while(!osList_CheckIsEmpty(&timer_HardList)) {
    /*取出timer*/
    timer = osList_Entry(timer_HardList.next, osTimer_Attr_t, list);

    /** 
     * 判断是否超时
     * 原理是currentTick - timer->timeoutTick 如果是负数,数值反转大于CPU_TICK_MAX / 2,证明没到时间条件不符合
     * 减法结果为大于等于0时,条件符合证明到时间或者已经超过过规定时间了,避免可能某原因跳过判断
     */
    if ((currentTick - timer->timeoutTick) < OS_TIMOUT_CHECKPOINT) {
      /*删除节点*/
      osList_DeleteNode(&timer->list);

      /*改变状态*/
      timer->state = osTimerStop;

      /*回调该定时器的函数*/
      timer->callback(timer->arguments);

      /*检查定时器运行类型*/
      if (timer->flag == osTimerPeriodic) {
        /*重启定时器*/
        osTimer_Start(timer, timer->perTick);
      }
    }
    /*因为链表是按顺序的,如果检测到一个没到时间,证明后面的都没到时间可以退出*/
    else {
      break;
    }
  }

  /*开中断*/
  hal_EnableINT(level);
}


#if USING_SOFT_TIMER == 1

/**
  * 软定时器核心线程
  *
  * @param none
  * 
  * @retval none
  */
OS_NO_RETURN os_SoftTimer_Thread(void *argument) {
  volatile osTick_t currentTick;
  volatile osTick_t nextTimeout;
  osTimer_Attr_t *timer;

  for (;;) {
    if (osList_CheckIsEmpty(&timer_softList)) {
      nextTimeout = OS_WAIT_FOREVER;
    }
    else {
      timer = osList_Entry(timer_softList.next, osTimer_Attr_t, list);
      nextTimeout = timer->timeoutTick;
    }

    /*无定时器挂载,先挂起*/
    if (nextTimeout == OS_WAIT_FOREVER) {
      osThread_Suspend(osThread_Self());
      
      osThread_Yield();
    }
    else {
      /*获取最近的Tick*/
      currentTick = osSys_GetNowTick();
      
      /*判断还有多少tick后超时*/
      if ((nextTimeout - currentTick) < OS_TIMOUT_CHECKPOINT) {
        /*延时相对时间*/  
        osThread_Delay(nextTimeout - currentTick);
      }
    }
    
    /*锁定调度器*/
    osSche_Lock();

    /*操作定时器*/
    while(!osList_CheckIsEmpty(&timer_softList)) {
      /*取出定时器*/
      timer = osList_Entry(timer_softList.next, osTimer_Attr_t, list);

      /*获取最近的TICK*/
      currentTick = osSys_GetNowTick();

      if ((currentTick - timer->timeoutTick) < OS_TIMOUT_CHECKPOINT) {
        /*从timer_list中删除节点*/
        osList_DeleteNode(&(timer->list));

        /*解锁调度器*/
//        osSche_Unlock();

        /*回调该定时器的函数*/
        timer->callback(timer->arguments);
         
        /*重新获取最近的TICK*/
        currentTick = osSys_GetNowTick();

        /*Relock*/
//        osSche_Lock();
        
        /*标记定时器该周期结束*/
        timer->state = osTimerStop;

        if (timer->flag == osTimerPeriodic) {
          /*重启定时器*/
          osTimer_Start(timer, timer->perTick);
        }
      }
      else {
        break;
      }
    }

    /*解锁调度器*/
    osSche_Unlock();
  }
}
#endif

/*@}*/

/**
 * @addtogroup timer user functions
 */

/*@{*/

/**
 * 创建定时器
 *
 * @param obj 定时器对象
 * @param flag 模式(once|periodic)
 * @param arguments 回调函数传入参数
 * 
 * @retval 定时器句柄
 */
osTimer_ID osTimer_Create(osTimer_Attr_t *obj, osTimer_Flag flag, void *arguments) {
  obj->state = osTimerStop;
  obj->flag = flag;
  obj->arguments = arguments;

  osList_HeadInit(&(obj->list));

  return (osTimer_ID)obj;
}
EXPORT_SYMBOL(osTimer_Create);


/**
 * 设置的定时器周期
 *
 * @param id 定时器句柄
 * @param tick 周期长度
 * 
 * @retval none
 */
void osTimer_SetTick(osTimer_ID id, osTick_t tick) {
  osTimer_Attr_t *obj = (osTimer_Attr_t *)id;

  /*检查定时运行状态*/
  if (obj->flag == osTimerRunning) {
    //OS_ASSERT

    return;
  }

  obj->perTick = tick;
}
EXPORT_SYMBOL(osTimer_SetTick);


/**
 * 定时器启动
 *
 * @param id 定时器句柄
 * @param tick 定时器定时值
 * 
 * @retval none
 */
void osTimer_Start(osTimer_ID id, osTick_t tick) {
  struct osList_t *timerList;

  /*关中断*/
  register uint32_t level = hal_DisableINT();

  osTimer_Attr_t *timer = (osTimer_Attr_t *)id;
  osTimer_Attr_t *timerNext;

  /*清空关联*/
  osList_DeleteNode(&(timer->list));

  /*标记运行状态*/
  timer->state = osTimerRunning;

  /*设置超时的Tick*/
  timer->perTick = tick;
  
  if (tick == OS_WAIT_FOREVER) {
    timer->timeoutTick = OS_WAIT_FOREVER;
  }
  else {
    timer->timeoutTick = osSys_GetNowTick() + timer->perTick;
  }

  /*选择定时器运行列表*/
#if USING_SOFT_TIMER == 1
  if (timer->mode == osTimerSoft) {
    timerList = &timer_softList;
  }
  else {
    timerList = &timer_HardList;
  }
#else
  timerList = &timer_HardList;
#endif

  if (osList_CheckIsEmpty(timerList)) {
    /*第一个list,直接加入*/
    osList_AddTail(timerList, &(timer->list));
  }
  else {
    /*循环取出最小timer值并插入到其中*/
    struct osList_t *pos, *list;
    for (pos = (timerList)->next, list = pos->next; \
      pos != (timerList); \
      pos = list, list = pos->next) {

      timerNext = osList_Entry(pos, osTimer_Attr_t, list);

      /*如果Timer超时值小于原有list最小值,插到他前面,否则continue*/
      if (timer->timeoutTick <= timerNext->timeoutTick) {
        osList_AddTail(pos, &(timer->list));
        
        break;
      }
    }

    /*检查timer->list是否还没插到list中*/
    if (osList_CheckIsEmpty(&(timer->list))) {
      /* 这个定时器为最大值 */
      osList_AddTail(timerList, &timer->list);
    }
  }

#if USING_SOFT_TIMER == 1
  /*唤醒os_SoftTimer_Thread*/
  if (timer->mode == osTimerSoft) {
    if (os_Thread_SoftTimer.state == osThreadSuspend) {
      osThread_Ready(timer_SoftThreadID);
    }
  }
#endif

  /*开中断*/
  hal_EnableINT(level);
}
EXPORT_SYMBOL(osTimer_Start);


/**
 * 定时器停止
 *
 * @param id 定时器句柄
 * 
 * @retval none
 */
void osTimer_Stop(osTimer_ID id) {
  /*关中断*/
  register uint32_t level = hal_DisableINT();

  osTimer_Attr_t *timer = (osTimer_Attr_t *)id;

  /*从timer_list中删除节点*/
  osList_DeleteNode(&(timer->list));

  /*清除定时器运行状态*/
  timer->state = osTimerStop;
  timer->timeoutTick = 0;

  /*开中断*/
  hal_EnableINT(level);
}
EXPORT_SYMBOL(osTimer_Stop);


/**
 * 设置定时器回调函数的传入参数
 *
 * @param id 定时器句柄
 * @param arguments 参数指针
 * 
 * @retval none
 */
void osTimer_SetArgument(osTimer_ID id, void *arguments) {
  /*关中断*/
  register uint32_t level = hal_DisableINT();

  osTimer_Attr_t *timer = (osTimer_Attr_t *)id;

  if (timer->state != osTimerStop) {
    //OS_ASSERT
    
    /*开中断*/
    hal_EnableINT(level);

    return;
  }

  timer->arguments = arguments;

  /*开中断*/
  hal_EnableINT(level);
}
EXPORT_SYMBOL(osTimer_SetArgument);


/**
 * 获取定时器的剩余时间
 *
 * @param id 定时器句柄
 * 
 * @retval tick剩余值
 */
osTick_t osTimer_GetResidueTick(osTimer_ID id) {
  /*关中断*/
  register uint32_t level = hal_DisableINT();

  osTimer_Attr_t *timer = (osTimer_Attr_t *)id;
  
  osTick_t tick = timer->timeoutTick - osSys_GetNowTick();

  /*开中断*/
  hal_EnableINT(level);  

  return tick; 
}
EXPORT_SYMBOL(osTimer_GetResidueTick);

/*@}*/
