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
 * File      : startup.c
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
 * @addtogroup Hal file include
 */

/*@{*/

#include "stm32f7xx.h"                  // Device header
#include "stm32f7xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

#include "stm32f7xx_nucleo_144.h"

/*@}*/

/**
 * @addtogroup OS file include
 */

/*@{*/

#include "os/osAPI.h"

/*@}*/

/**
 * @addtogroup add ons
 */
 
/*@{*/

#include "addons/console/console.h"

/*@}*/

/*@}*/

/**
 * @addtogroup threadDemo1
 */

/*@{*/

uint32_t threadDemo1_RunningCount = 0;

osThread_FuncDef(threadDemo1) {
  mLog_ThreadPrintf(Log_I, "Thread-1", 100, CONSOLE_YELLOW "Startup.\r\n" CONSOLE_NONE);
  for (;;) {
    osThread_Delay(100);
    
    threadDemo1_RunningCount++;
  }
}
osThread_Def(threadDemo1, 0, ALIGN(488, 8), threadDemo1);
osThread_ID threadDemo1_ID;

/*@}*/

/**
 * @addtogroup threadDemo2
 */

/*@{*/

uint32_t threadDemo2_RunningCount = 0;

osThread_FuncDef(threadDemo2) {
  mLog_ThreadPrintf(Log_I, "Thread-2", 100, CONSOLE_YELLOW "Startup.\r\n" CONSOLE_NONE);
  for (;;) {
    osThread_Delay(500);
    
    threadDemo2_RunningCount++;
  }
}
osThread_Def(threadDemo2, 1, ALIGN(488, 8), threadDemo2);
osThread_ID threadDemo2_ID;

/*@}*/

/**
 * @addtogroup threadDemo3
 */

/*@{*/

uint32_t threadDemo3_RunningCount = 0;

osThread_FuncDef(threadDemo3) {
  mLog_ThreadPrintf(Log_I, "Thread-3", 100, CONSOLE_YELLOW "Startup.\r\n" CONSOLE_NONE);
  for (;;) {
    osThread_Delay(1000);
    
    threadDemo3_RunningCount++;
  }
}
osThread_Def(threadDemo3, 2, ALIGN(488, 8), threadDemo3);
osThread_ID threadDemo3_ID;

/*@}*/

/**
 * @addtogroup timerDemo1
 */

/*@{*/

uint32_t timerDemo1_RunningCount = 0;

osTimer_Callback(timerDemo1) {
  timerDemo1_RunningCount++;
  
  BSP_LED_Toggle(LED1);
}
osTimer_Def(timerDemo1, osTimerSoft, timerDemo1);
osTimer_ID timerDemo1_ID;

/*@}*/

/**
 * @addtogroup timerDemo2
 */

/*@{*/

uint32_t timerDemo2_RunningCount = 0;

osTimer_Callback(timerDemo2) {
  timerDemo2_RunningCount++;
  
  BSP_LED_Toggle(LED2);
}
osTimer_Def(timerDemo2, osTimerSoft, timerDemo2);
osTimer_ID timerDemo2_ID;

/*@}*/

/**
 * @addtogroup timerDemo3
 */

/*@{*/

uint32_t timerDemo3_RunningCount = 0;

osTimer_Callback(timerDemo3) {
  timerDemo3_RunningCount++;
  
  BSP_LED_Toggle(LED3);
}
osTimer_Def(timerDemo3, osTimerSoft, timerDemo3);
osTimer_ID timerDemo3_ID;

/*@}*/

/**
 * @addtogroup main functions
 */

/*@{*/

int main(void) {
  /*硬件初始化*/
  osHal_CoreInit();
  
  /*板载初始化*/
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  
  /*os内核初始化*/
  osSys_KernelInitialize();
  
  /*相关模块初始化*/
  osSys_ModulesInit();
  
  /*threadDemo1线程*/
  threadDemo1_ID = osThread_Create(osThread_Obj(threadDemo1), (void *)0);
  osThread_Ready(threadDemo1_ID);
  
  /*threadDemo2线程*/
  threadDemo2_ID = osThread_Create(osThread_Obj(threadDemo2), (void *)0);
  osThread_Ready(threadDemo2_ID);
  
  /*threadDemo3线程*/
  threadDemo3_ID = osThread_Create(osThread_Obj(threadDemo3), (void *)0);
  osThread_Ready(threadDemo3_ID);

  /*定时器1*/
  timerDemo1_ID = osTimer_Create(osTimer_Obj(timerDemo1), osTimerPeriodic, (void *)0);
  osTimer_Start(timerDemo1_ID, 25);
  
  /*定时器2*/
  timerDemo2_ID = osTimer_Create(osTimer_Obj(timerDemo2), osTimerPeriodic, (void *)0);
  osTimer_Start(timerDemo2_ID, 100);
  
  /*定时器3*/
  timerDemo3_ID = osTimer_Create(osTimer_Obj(timerDemo3), osTimerPeriodic, (void *)0);
  osTimer_Start(timerDemo3_ID, 1000);

  /*启动OS*/
  osSys_KernelStartup();
  
  for (;;);
}

/*@}*/

