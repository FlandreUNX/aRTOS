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
 * File      : event.h
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

#ifndef EVENT_H_
#define EVENT_H_

/**
 * @addtogroup OS file include
 */

/*@{*/

#include "../arch/platform.h"


/*@}*/

/**
 * @addtogroup event ennum define
 */

/*@{*/

/**
 * event 类型描述符
 * @note 描述存在的event类型
 */
typedef enum {
  osEventSignal,   /**< 信号类 */
  osEventNull      /**< 无事件 */
} osEvent_Type;


/**
 * event 状态描述符
 * @note 描述event的状态
 */
typedef enum {
  /**
   * Thread->event状态
   */
  osEvent_sSet,         /**< 线程事件被置位 */
  osEvent_sWait,        /**< 线程正在等待事件 */
  osEvent_sIDLE,        /**< 线程事件空闲中 */
  
  /**
   * 输出Event_t通用状态
   */
  osEvent_sOK,          /**< 无错误 */
  osEvent_sTimeout,     /**< 等待超时 */
  osEvent_sUnkwn        /**< 未知错误 */
} osEvent_Status;

/*@}*/

/**
 * @addtogroup event define
 */

/*@{*/

/**
 *  event 根描述符
 *  @note 作为所有线程通讯类型的根
 */
typedef struct {
  union {
    int32_t v;   /**< 事件值 */
    void *p;      /**< 事件指向子类指针 */
  } value;
  
  osEvent_Type type;  /**< 事件类型 */

  osEvent_Status state;   /**< 事件状态 */
} osEvent_t;

/*@}*/

#endif
