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
 * File      : signal.c
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

#include "./event.h"

/**
 * @addtogroup OS file include
 */

/*@{*/

#include "../kernel/schedule.h"
#include "../kernel/thread.h"

#include "../arch/hal_cm.h"

/*@}*/

/**
 * @addtogroup signal base functions
 */

/*@{*/

/**
 *  设置线程信号
 *
 *  @param target_Id 目标线程Id
 *  @param signal 信号值
 *  @param wait 堵塞等待时间 
 * 
 *  @retval none
 */
osEvent_Status osSignal_Set(osThread_Id target_Id, int32_t signal, osTick_t wait) {
  /*屏蔽调度器,线程安全*/
  osSche_Lock();
  
  osThread_Attr_t *thread = (osThread_Attr_t *)target_Id;
  
  /*保存该线程的事件状态*/
  osEvent_Status preEventStage = thread->event.state;

  /*该线程等待某事件*/
  if (preEventStage == osEventWaitting) {
    /*该线程等待的是同一个东西*/
    if (thread->event.type == osEventSignal && 
      thread->event.vaule.v == signal) {
      /*标记事件*/
      thread->event.state = osEventGet;
        
      /*线程堵塞&等待改信号则唤醒*/
      if (thread->state == osThreadBlocked) {
        osThread_Ready(thread);
        
        return osEventNoError;
      }
    }
    /*不是同一个东西,延时*/
    else {
      if (wait == 0) {
        /*解锁*/
        osSche_Unlock();
        
        return osEventTargetBusy;
      }
      
      /*解锁*/
      osSche_Unlock();
      
      /*等待*/
      osThread_Delay(wait);
      
      /*重新上锁*/
      osSche_Lock();
      
      if (thread->event.type == osEventSignal && 
        thread->event.vaule.v == signal) {
        /*标记事件*/
        thread->event.state = osEventGet;
        
        /*线程堵塞&等待改信号则唤醒*/
        if (thread->state == osThreadBlocked) {
          /*解锁*/
          osSche_Unlock();
          
          osThread_Ready(thread);
          
          return osEventNoError;
        }
        else {
          /*解锁*/
          osSche_Unlock();
          
          return osEventTargetBusy;
        }
      }
    }
  }
  /*线程原事件过期或者无事件等待*/
  else if (preEventStage == osEventNoError ||
    preEventStage == osEventInvalid) {
    /*标记事件*/
    thread->event.state = osEventGet;
    thread->event.vaule.v = signal;
      
    /*解锁*/
    osSche_Unlock();
      
    return osEventNoError;
  }
  
  /*未知状态*/
  return osEventUnknown;
}

/*@}*/
