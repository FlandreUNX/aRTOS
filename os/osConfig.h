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
 * File      : osConfig.h
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

#ifndef OSCONFIG_H_
#define OSCONFIG_H_

/**
 * @addtogroup 平台配置
 */
  
/*@{*/

/**
 * CPU运行频率,单位Hz
 */
#define OS_CLOCK       (48000000)

/**
 * CPU_Tick中断溢出计量
 */
#define OS_TICK        (1000)

/*@}*/

/**
 * @addtogroup 系统配置
 */
  
/*@{*/

/**
 * 系统线程优先级分级数量,此值-1为最小优先级,0为最大优先级
 */
#define MAX_PRIORITY_LEVEL      (128)

/**
 * 空闲线程堆栈大小
 */
#define IDLE_STACK_SIZE     (256)

/**
 * 空闲线程优先级
 */
#define IDLE_PRIORITY     (MAX_PRIORITY_LEVEL - 1)

/**
 * 是否启用销毁线程移除功能
 */
#define USING_DEID_THREAD_RM      (1)

/**
 * 软件(附加线程)定时器启用开关
 */
#define USING_SOFT_TIMER        (1)
#if USING_SOFT_TIMER == 1
  /**
   *  软件定时器线程优先级
   */
  #define SOFT_TIMER_PRIORITY     (MAX_PRIORITY_LEVEL - 1)

  /**
   *  软件定时器线程堆栈大小
   */
  #define SOFT_TIMER_STACK_SIZE   (256)
#endif

/*@}*/

/**
 * @addtogroup 内存库配置
 */
  
/*@{*/

/**
 * buddy内存库启用开关
 */
#define USING_BUDDY_MANAGER     (1)

#if USING_BUDDY_MANAGER == 1
  /**
  * @addtogroup buddy 内存库配置 
  */

  /*@{*/

  /**
  * buddy 阶 order 链表数量
  * 例如:一块内存块为32B
  * 2^(0, 1, 2, 3, 4, 5)
  *      0 ->  32 * 1    = 32      (2^0)
  *      1 ->  32 * 2    = 64      (2^1)
  *      2 ->  32 * 4    = 128     (2^2)
  *      3 ->  32 * 8    = 256     (2^3)
  *      4 ->  32 * 16   = 512     (2^4)
  *      5 ->  32 * 32   = 1024    (2^5)
  */

  /**
   *  buddy单个内存块大小,单位为byte
   */
  #define BUDDY_BLOCK_SIZE    (32)

  /**
   *  buddy内存块阶级数量,参考上表格对应关系
   */
  #define BUDDY_ORDER         (6)

  /*@}*/
#endif

/*@}*/

/**
 * @addtogroup 附加模块选项
 */
  
/*@{*/

/**
 * 是否启动函数内核导出符功能
 */
#define USING_SYMBOL_EXPORT     (1)

/*@}*/

#endif
