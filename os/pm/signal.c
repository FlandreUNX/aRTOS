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

#include "../lib/symbolExport.h"

/*@}*/

/**
 * @addtogroup signal base functions
 */

/*@{*/

/**
 * 设置线程信号
 *
 * @param target_Id 目标线程Id
 * @param signal 信号值
 * @param wait 堵塞等待时间 
 * 
 * @retval 操作结果
 */
osEvent_Status osSignal_Set(osThread_Id target_Id, int32_t signal, osTick_t wait) {
  if (target_Id == NULL) {
    return osEvent_sUnTarget;
  }
  
  osThread_Attr_t *thread = target_Id;
  
  osSche_Lock();

  /*目标无事件等待*/
  if (thread->event.type == osEventNull) {
    thread->event.type = osEventSignal;
    thread->event.value.v = signal;
    
    osSche_Unlock();
  
    return osEvent_sOK;
  }
  /*目标正在等待signal*/
  else if (thread->event.type == osEventSignal) {
    thread->event.value.v = signal;
    
    /*唤醒目标*/
    osSche_Unlock();
    osThread_Ready(thread);
    
    return osEvent_sOK;
  }
  /*线程存在其他事件*/
  else {
    /*延时*/
    osSche_Unlock();
    osThread_Delay(wait);
    osSche_Lock();
    
    /*目标等待signal或者空闲*/
    if (thread->event.type == osEventSignal || thread->event.type == osEventNull) {
      thread->event.value.v = signal;
      
      /*唤醒目标*/
      osSche_Unlock();
      osThread_Ready(thread);

      return osEvent_sOK;
    }
    else {
      osSche_Unlock();
      
      return osEvent_sTimeout;
    }
  }
}
EXPORT_SYMBOL(osSignal_Set);


/**
 * 获取信号
 *
 * @param wait 堵塞等待时间 
 * 
 * @retval 返回具体事件信息
 */
osEvent_t osSignal_Wait(osTick_t wait) {
  osEvent_t retEvent;
  
  retEvent.value.v = 0;
  retEvent.state = osEvent_sOK;
  retEvent.type = osEventSignal;
  
  osSche_Lock();
  
  osThread_Attr_t *self = osThread_Self();
    
  /*线程event空闲,主动等待信号*/
  if (self->event.type == osEventNull) {
    self->event.type = osEventSignal;
    self->event.value.v = 0;
  
    /*延时*/
    osSche_Unlock();
    osThread_Delay(wait);
    osSche_Lock();
    
    /*接受到信号*/
    if (self->event.value.v) {
      retEvent.value.v = self->event.value.v;
      
      self->event.type = osEventNull;
      self->event.value.v = 0;
    }
    /*还是没信号*/
    else {
      self->event.type = osEventNull;
      self->event.value.v = 0;
      
      retEvent.state = osEvent_sTimeout;
    }
  }
  /*线程已经存在signal事件*/
  else if (self->event.type == osEventSignal) {
    retEvent.value.v = self->event.value.v;

    self->event.type = osEventNull;
    self->event.value.v = 0;
  }
  /*线程存在其他事件*/
  else {
    retEvent.state = osEvent_sUnTarget;
  }
  
  osSche_Unlock();
  
  return retEvent;
}
EXPORT_SYMBOL(osSignal_Wait);

/*@}*/
