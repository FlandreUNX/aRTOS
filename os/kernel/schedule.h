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
 * File      : schedule.h
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
 
#ifndef SCHEDULE_H_
#define SCHEDULE_H_

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "./thread.h"

/*@}*/

/**
 * @addtogroup schedule  var
 */
 
/*@{*/

/**
 *  线程切换指示器
 *  @note none
 */
struct threadSwitchInfo_t {
  osThread_Attr_t* nowThread;
  osThread_Attr_t* nextThread;
};

/*@}*/

/**
 * @addtogroup schedule system functions
 */
 
/*@{*/

extern void sche_Init(void);

extern void sche_InsertThread(osThread_Attr_t* thread);
extern void sche_RemoveThread(osThread_Attr_t* thread);

extern void sche_NextToNow(void);
extern void sche_SetFirstThread(void);

extern void sche_ToNextThread(void);

extern void sys_TickHandler(void);

/*@}*/

/**
 * @addtogroup schedule user functions
 */
 
/*@{*/

extern void osSche_Lock(void);
extern void osSche_Unlock(void);

extern osTick_t osSys_GetNowTick(void);

/*@}*/

#endif
