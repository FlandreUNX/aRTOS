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
#include "../arch/hal_Support.h"

#include "../mm/buddy.h"

#include "../lib/list.h"
#include "../lib/symbolExport.h"

#include "../osConfig.h"

#include "../kernel/schedule.h"
#include "../kernel/timer.h"

/*@}*/

/**
 * @addtogroup add ons
 */
 
/*@{*/

#include "../service/log.h"

#include "../osInfo.h"

/*@}*/

/**
 * @addtogroup OS init user functions
 */

/*@{*/

extern struct threadSwitchInfo_t sche_ThreadSwitchStatus;

extern osThread_Id idle_ThreadID;
extern osThread_Attr_t os_Thread_Idle;
extern uint8_t os_Thread_Stack_Idle[IDLE_STACK_SIZE];


/**
 * 相关模块初始化
 *
 * @param none
 *
 * @retval none
 */
void modulesInit(void) {
  log_Init();

  osLog_InfoShow();
}


/**
 * 初始化os内核
 *
 * @param none
 *
 * @retval none
 */
void osSys_KernelInitialize(void) {
  register uint32_t level = hal_DisableINT();
  
  /*动态内存库初始化*/
  mem_Init((uint32_t)HEAP_BEGIN, (uint32_t)HEAP_END);
  
  /*调度器初始化*/
  sche_Init();

  /*定时器初始化*/
  timer_Init();
  
  modulesInit();
  
  hal_EnableINT(level);
}


/**
 * 启动os
 *
 * @param none
 *
 * @retval none
 */
void osSys_KernelStartup(void) { 
  /*初始化并启动idle*/
  idle_ThreadID = osThread_StaticCreate(&os_Thread_Idle, (void *)0, os_Thread_Stack_Idle);
  if (idle_ThreadID == 0) {
    //OS_ASSERT
  }

  osThread_Ready(idle_ThreadID);
  
  hal_DisableINT();
  
  sche_SetFirstThread();
  
  hal_SystickConfig();
  
#if CM_VFP_ENABLE == 1
  hal_EnableVFP();
#endif
  
  osLog_RawPrintf(0, "\033[1;32maRTOS Start...\033[0m\r\n\r\n");

  cpu_GotoFisrtTask();
}

/*@}*/
