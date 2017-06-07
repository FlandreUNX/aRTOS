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
 * @addtogroup add ons
 */
 
/*@{*/

#include "addons/console/console.h"
#include "../osInfo.h"

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

/*@}*/

/**
 * @addtogroup idle thread 
 */
 
/*@{*/

#if OS_DEBUG_MODE == 1
/**
 *  线程运行计数
 *  @note none
 */
uint32_t idle_RunningCount = 0;
#endif

OS_NO_RETURN os_Idle_Thread(void *argument) {
#if OS_DEBUG_MODE == 1
  mLog_ThreadPrintf(Log_I, "Idle", 0, CONSOLE_YELLOW "Startup. FreeMem=%.1f Kbyte" CONSOLE_NONE, osMem_Info.remaining / 1024.0f);
#endif
  
  for (;;) {
#if OS_DEBUG_MODE == 1
    idle_RunningCount++;
#endif
  }
}

/*@}*/
