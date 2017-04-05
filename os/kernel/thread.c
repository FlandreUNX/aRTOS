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
 * File      : thread.c
 * This file is part of ACGrtos
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

#include "./thread.h"

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
 * @addtogroup system extern
 */

/*@{*/

extern volatile osThread_Attr_t *sche_NowThread, *sche_NextThread;

extern struct osList_Head_t sche_ReadyList[MAX_PRIORITY_LEVEL];

/*@}*/

/**
 * @addtogroup thread user functions
 */

/*@{*/

/**
 * 创建任务
 *
 * @param thread 线程对象
 * @param argument 入口函数的传入参数
 *
 * @return 线程句柄
 */
osThread_ID osThread_Create(osThread_Attr_t *thread, void *argument) {
    /*调整堆栈大小,4字节对齐*/
    thread->stackSize = ALIGN(thread->stackSize, 4);

    /*申请栈空间*/
    thread->stackEnd = (void *)osMem_Malloc(thread->stackSize);
    if (thread->stackEnd == NULL) {
        return 0;
    }

    /*最后栈底最后一个字节写入一个固定数值,用于检测栈溢出*/
    *((uint32_t*) thread->stackEnd) = (uint32_t) MAGIC_WORD;

    /*设置栈顶*/
    thread->stackTop = (void *)((uint32_t)thread->stackEnd + thread->stackSize);

    /*初始化任务CPU栈寄存器*/
    thread->stackTop = cpu_SetupRegisters(thread->functions, argument, thread->stackTop);
    thread->arguments = argument;

    /*超出最大设定组别*/
    if (thread->priority > MAX_PRIORITY_LEVEL) {
        thread->priority = MAX_PRIORITY_LEVEL;
    }

    /*确定这个任务在优先bitmap的位置*/
    #if MAX_PRIORITY_LEVEL > 32
        thread->bitmap_Low_Mask = thread->priority >> 3;
        thread->bitmap_High_Mask = 1L << (thread->priority & 0x07);
        thread->bitmap_Mask = 1L << thread->bitmap_Low_Mask;
    #else
        thread->bitmap_Mask = 1L << thread->priority;
    #endif
   
    /*初始化节点*/
    osList_HeadInit(&thread->list);

    /*初始化依赖定时器*/
    osThread_Create(&thread->timer, OS_TIMER_PERIODIC, (void *)thread);

    /*设置其他参数*/
    thread->stage = osThreadSuspend;

    return (osThread_ID)thread;
}
EXPORT_SYMBOL(osThread_Create);


/**
 * 就绪任务
 *
 * @param id 线程句柄
 *
 * @return none
 */
void osThread_Ready(osThread_ID id) {
    register uint32_t level;
    osThread_Attr_t *thread = (osThread_Attr_t *)id;

    /*关中断*/
    level = hal_DisableINT();

    /*将任务调节到就绪态,并加入调度行列*/
    thread->stage = osThreadReady;

    /*关闭定时器*/
    osTimer_Stop((osTimer_ID)&thread->timer);

    /*插入调度器*/
    sche_InsertThread(thread);

    hal_EnableINT(level);
}
EXPORT_SYMBOL(osThread_Ready);




/*@}*/
