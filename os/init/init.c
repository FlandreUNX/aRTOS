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
 * File      : init.c
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

#include "../kernel/schedule.h"
#include "../kernel/timer.h"

/*@}*/

/**
 * @addtogroup OS init user functions
 */

/*@{*/

/**
 * 初始化os内核
 *
 * @param none
 *
 * @return none
 */
void osSys_KernelInitialize(void) {
  /*调度器初始化*/
  sche_Init();

  /*定时器初始化*/
  timer_Init();
  
  /*动态内存库初始化*/
  mem_Init((uint32_t)HEAP_BEGIN, HEAP_END);
}


/**
 * 启动os
 *
 * @param none
 *
 * @return none
 */
void osSys_KernelStartup(void) {
  /*初始化并启动idle*/
  extern osThread_ID idle_ThreadID;
  extern osThread_Attr_t os_Thread_Idle;
  idle_ThreadID = osThread_Create(osThread_Obj(Idle), (void*)0);
  if (idle_ThreadID == 0) {
    //OS_ASSERT
  }
  osThread_Ready(idle_ThreadID);

  cpu_GotoFisrtTask();
}

/*@}*/
