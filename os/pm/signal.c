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
 *  @retval 操作结果
 */
osEvent_Status osSignal_Set(osThread_Id target_Id, int32_t signal, osTick_t wait) {
  osSche_Lock();
  
  osThread_Attr_t *thread = target_Id;

  return osEvent_sOK;
}


/**
 *  获取信号
 *
 *  @param wait 堵塞等待时间 
 * 
 *  @retval 返回具体事件信息
 */
osEvent_t osSignal_Wait(osTick_t wait) {
  osEvent_t retEvent;
  
  retEvent.value.v = 0;
  retEvent.state = osEvent_sOK;
  retEvent.type = osEventSignal;
  
  osSche_Lock();
  
  osThread_Attr_t *self = osThread_Self();

  /**
   *  线程存在两个事件情况->wait/set
   */
  
  /*线程被标记等待某事件*/
  /*set一级情况包括other/signal*/
  if (self->event.state == osEvent_sSet) {
    if (self->event.type == osEventSignal) {
      /*拉取信号*/
      retEvent.value.v = self->event.value.v;
    
      /*清空本次事件*/
      self->event.value.v = 0;
      self->event.type = osEventNull;
      self->event.state = osEvent_sIDLE;
    }
    /*线程被设置了其他事件*/
    else {
      osSche_Unlock();
      if (wait == 0) {
        retEvent.state = osEvent_sTimeout;
      }
      osThread_Delay(wait);
      osSche_Lock();
      
      /**
       *  延时后存在情况->IDLE/Other/Signal
       *  IDLE\Other作为timeout处理
       */
      if (self->event.type == osEventSignal) {
        retEvent.value.v = self->event.value.v;
    
        self->event.value.v = 0;
        self->event.type = osEventNull;
        self->event.state = osEvent_sIDLE;
      }
      else {
        retEvent.state = osEvent_sTimeout;
      }
    }
  }
  /*线程主动等待事件*/
  /*wait一级情况包括other/signal*/
  else {
    self->event.state = osEvent_sWait;
    
    osSche_Unlock();
    if (wait == 0) {
      retEvent.state = osEvent_sTimeout;
    }
    osThread_Delay(wait);
    osSche_Lock();
    
    if (self->event.type == osEventSignal) {
      retEvent.value.v = self->event.value.v;
  
      self->event.value.v = 0;
      self->event.type = osEventNull;
      self->event.state = osEvent_sIDLE;
    }
    else {
      retEvent.state = osEvent_sTimeout;
    }
  }
  
  osSche_Unlock();
  
  return retEvent;
}

/*@}*/
