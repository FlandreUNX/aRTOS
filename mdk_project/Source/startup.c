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
 * @addtogroup threadDemo1
 */

/*@{*/

osThread_Def("Thread-1", thread1, 2, 512) {
  osEvent_t event;
  for (;;) {
    event = osSignal_Wait(5000);
    
    switch (event.value.v) {
      case (0x03) :
      case (0x02) :
      case (0x01) : {
        osLog_Printf(Log_I, osThread_GetName(osThread_Self()), 10, "Signal get -> %d", event.value.v);
      } break;
    }
  }
}
osThread_Id thread1_ID;

/*@}*/

/**
 * @addtogroup threadDemo2
 */

/*@{*/

osThread_StackDef(thread2, 512);
osThread_Def("Thread-2", thread2, 1, sizeof(osThread_StackObj(thread2))) {
  thread1_ID = osThread_Create(osThread_Obj(thread1), (void *)0);
  osThread_Ready(thread1_ID);

  osEvent_t event;
  for (;;) {
    event = osSignal_Wait(5000);
    
    switch (event.value.v) {
      case (0x03) :
      case (0x02) :
      case (0x01) : {
        osLog_Printf(Log_I, osThread_GetName(osThread_Self()), 10, "Signal get -> %d", event.value.v);
      } break;
    }
  }
}
osThread_Id thread2_ID;

/*@}*/

/**
 * @addtogroup threadDemo2
 */

/*@{*/

osThread_StackDef(thread3, 512);
osThread_Def("Thread-3", thread3, 0, sizeof(osThread_StackObj(thread3))){
  osThread_Ready(thread2_ID);

  for (;;) {
   osThread_Delay(1000);
   osLog_Printf(Log_I, osThread_GetName(osThread_Self()), 5, "Live!");
    BSP_LED_Toggle(LED3);
  }
}
osThread_Id thread3_ID;

/*@}*/

/**
 * @addtogroup timerDemo1
 */

/*@{*/

osTimer_Callback(timerDemo1) {
  BSP_LED_Toggle(LED1);
  osSignal_Set(thread1_ID, 0x01, 0);
}
osTimer_Def(timerDemo1, osTimerSoft, timerDemo1);
osTimer_ID timerDemo1_ID;

/*@}*/

/**
 * @addtogroup timerDemo2
 */

/*@{*/

osTimer_Callback(timerDemo2) {
  BSP_LED_Toggle(LED2);
  osSignal_Set(thread2_ID, 0x02, 0);
}
osTimer_Def(timerDemo2, osTimerSoft, timerDemo2);
osTimer_ID timerDemo2_ID;

/*@}*/

/**
 * @addtogroup main functions
 */

/*@{*/

int main(void) {
  /*硬件初始化*/
  osHal_CoreInit();
  
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /*os初始化*/
  osSys_KernelInitialize();

  thread3_ID = osThread_StaticCreate(osThread_Obj(thread3), (void *)0, osThread_StackObj(thread3));
  osThread_Ready(thread3_ID);
  
  thread2_ID = osThread_StaticCreate(osThread_Obj(thread2), (void *)0, osThread_StackObj(thread2));

  timerDemo1_ID = osTimer_Create(osTimer_Obj(timerDemo1), osTimerPeriodic, (void *)0);
  osTimer_Start(timerDemo1_ID, 200);
  
  timerDemo2_ID = osTimer_Create(osTimer_Obj(timerDemo2), osTimerPeriodic, (void *)0);
  osTimer_Start(timerDemo2_ID, 100);

  /*启动OS*/
  osSys_KernelStartup();
  
  for (;;);
}

/*@}*/

