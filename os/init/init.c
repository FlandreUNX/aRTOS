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

#include "addons/console/console.h"
#include "../osInfo.h"

/*@}*/

/**
 * @addtogroup OS init user functions
 */

/*@{*/

extern struct threadSwitchInfo_t sche_ThreadSwitchStatus;

extern osThread_Id idle_ThreadID;
extern osThread_Attr_t os_Thread_Idle;

/**
 * 初始化os内核
 *
 * @param none
 *
 * @retval none
 */
void osSys_KernelInitialize(void) {
  hal_DisableINT();
  
  /*动态内存库初始化*/
  mem_Init((uint32_t)HEAP_BEGIN, (uint32_t)HEAP_END);
  
  /*调度器初始化*/
  sche_Init();

  /*定时器初始化*/
  timer_Init();
  
  hal_EnableINT();
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
  idle_ThreadID = osThread_Create(&os_Thread_Idle, (void *)0);
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
  
  mLog_RawPrntf(0, "\033[1;32maRTOS Start...\033[0m\r\n\r\n");

  hal_EnableINT();
  
  cpu_GotoFisrtTask();
}


/**
 * 相关模块初始化
 *
 * @param none
 *
 * @retval none
 */
void osSys_ModulesInit(void) {
  mConsole_Init();
  
  /**
   *  欢迎界面
   */
  for (uint8_t i = 0; i < 12; i++) {
    mLog_RawPrntf(0, "%s\r\n", osLogo[i]);
  }
  
  mLog_RawPrntf(0, "%s\r\n", OS_INFO);
  mLog_RawPrntf(0, "%s\r\n", OS_AUTHOR);
  mLog_RawPrntf(0, "[CPU]\033[1;36m%s\033[0m ", MCU_NAME);
  mLog_RawPrntf(0, "[Sysclk]\033[1;36m%dHz\033[0m ", osHal_GetSysclk());
  mLog_RawPrntf(0, "[Mem(%s)]\033[1;36m%.1fKbyte\033[0m ", MCU_RAM_TYPE, osMem_Info.total / 1024.0f);
  mLog_RawPrntf(0, "\r\nModules initialize successed.\r\n");
}

/*@}*/
