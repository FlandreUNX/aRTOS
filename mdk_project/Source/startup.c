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
#include "addons/key/key.h"

/*@}*/

/**
 * @addtogroupkey key obj
 */

/*@{*/

mKey_Callback(userKey) {
  if (SELF_PRESS_TYPE() == KEY_IS_SP) {
    BSP_LED_Toggle(LED2);
    
    extern osThread_Id threadDemo1_ID;
    osSignal_Set(threadDemo1_ID, 0x01, 0);
  }
  else {
    BSP_LED_Toggle(LED3);
    
    extern osThread_Id threadDemo1_ID;
    osSignal_Set(threadDemo1_ID, 0x02, 0);
  }
  
}

mKeyDef_t key_User = {
  .type = KEY_TYPE_LONG,
  .level = KEY_LEVEL_HIGHT,
  
  .value = 0x02,
  
  .port = (uint32_t)USER_BUTTON_GPIO_PORT,
  .gpio = USER_BUTTON_PIN,

  .callback = userKey
};

/*@}*/

/**
 * @addtogroup threadDemo1
 */

/*@{*/

osThread_FuncDef(threadDemo1) {
  osEvent_t event;
  mLog_ThreadPrintf(Log_I, "Thread1", 1000, CONSOLE_YELLOW "Startup. FreeMem=%.1f Kbyte" CONSOLE_NONE, osMem_Info.remaining / 1024.0f);
  
  mKey_ObjInsert(&key_User);
  
  for (;;) {
    event = osSignal_Wait(5000);
    switch (event.value.v) {
      case (0x02) :
      case (0x01) : {
        mLog_ThreadPrintf(Log_I, "Thread1", 1000, "Signal get -> %d", event.value.v);
      } break;
      
      default : {
        mLog_ThreadPrintf(Log_W, "Thread1", 1000, "Waitting timeout");
      } break;
    }
  }
}
osThread_Def(threadDemo1, 0, 488, threadDemo1);
osThread_Id threadDemo1_ID;

/*@}*/

/**
 * @addtogroup timerDemo1
 */

/*@{*/

osTimer_Callback(timerDemo1) {
  BSP_LED_Toggle(LED1);
}
osTimer_Def(timerDemo1, osTimerSoft, timerDemo1);
osTimer_ID timerDemo1_ID;

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
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /*os内核初始化*/
  osSys_KernelInitialize();
  
  /*相关模块初始化*/
  osSys_ModulesInit();
  
  /*threadDemo1线程*/
  threadDemo1_ID = osThread_Create(osThread_Obj(threadDemo1), (void *)0);
  osThread_Ready(threadDemo1_ID);

  /*定时器1*/
  timerDemo1_ID = osTimer_Create(osTimer_Obj(timerDemo1), osTimerPeriodic, (void *)0);
  osTimer_Start(timerDemo1_ID, 500);

  /*启动OS*/
  osSys_KernelStartup();
  
  for (;;);
}

/*@}*/

