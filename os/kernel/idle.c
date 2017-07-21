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
 * File      : idle.c
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
 * @addtogroup schedule extern 
 */
 
/*@{*/

extern struct osList_t schedule_NoReadyList;

/*@}*/

/**
 * @addtogroup idle thread define 
 */
 
/*@{*/

osThread_Id idle_ThreadID;
extern OS_NO_RETURN os_Idle_Thread(void *argument);
osThread_Attr_t os_Thread_Idle = { \
  .initTimeSlice = 1, \
  .functions = os_Idle_Thread, \
  .stackSize = IDLE_STACK_SIZE, \
  .priority = IDLE_PRIORITY \
};
uint8_t os_Thread_Stack_Idle[IDLE_STACK_SIZE];

/*@}*/

/**
 * @addtogroup idle thread 
 */
 
/*@{*/

OS_NO_RETURN os_Idle_Thread(void *argument) {
  for (;;) {
  }
}

/*@}*/
