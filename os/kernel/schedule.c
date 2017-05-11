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
 * File      : schedule.c
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
 
#include "./schedule.h"

/**
 * @addtogroup ANSI-C Include
 */

/*@{*/

#include <string.h>

/*@}*/

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "../arch/hal_cm.h"
#include "../arch/platform.h"

#include "../lib/symbolExport.h"
#include "../lib/list.h"

#include "./thread.h"
#include "./timer.h"
#include "./irq.h"

#include "../osConfig.h"

/*@}*/

/**
 * @addtogroup os tick
 */
 
/*@{*/

static volatile osTick_t sys_TickCount;

/*@}*/

/**
 * @addtogroup schedule Indicators
 */
 
/*@{*/

/*调度器加锁指示*/
static volatile int8_t sche_LockNest;

/*当前运行中线程的优先级 PS:等于最高优先级的线程组*/
static volatile uint8_t sche_CurrPriority;

/*调度器线程运行指示*/
volatile osThread_Attr_t *sche_NowThread, *sche_NextThread;

/*@}*/

/**
 * @addtogroup schedule bitmap scan
 */
 
/*@{*/

/**
 * 查询用表格
 * 输入一个0-255的数能找出最低的bit
*/
static const uint8_t BITMAP[] = { \
  /* 00 */ 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* 10 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* 20 */ 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* 30 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* 40 */ 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* 50 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* 60 */ 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* 70 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* 80 */ 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* 90 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* A0 */ 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* B0 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* C0 */ 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* D0 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* E0 */ 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, \
  /* F0 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 \
};

/**
 * 线程优先级排序表的L1_bitmap
 * 如果优先级组别大于32就启动L2位图 否则只使用1级位图
 * 小于32的情况->32bit对应32个优先级
 * 大于32的情况->一共32bit 对应32组 共256bit 对应256个优先级
*/
#if MAX_PRIORITY_LEVEL <= 8
    static uint8_t bitmap_L1;
#else
    static uint32_t bitmap_L1;
#endif

/**
 * 线程优先级排序L2_bitmap
 * 共32个字节 对应256bit 等于256个优先级
*/
#if MAX_PRIORITY_LEVEL > 32
    static uint8_t bitmap_L2[32];
#endif

/*@}*/

/**
 * @addtogroup schedule universal list
 */
 
/*@{*/

/*每一优先级组的线程链表*/
struct osList_Head_t sche_ReadyList[MAX_PRIORITY_LEVEL];

#if USING_DEID_THREAD_RM == 1
    struct osList_Head_t sche_NoReadyList;
#endif

/*@}*/

/**
 * @addtogroup schedule system functions
 */
 
/*@{*/

/**
 * 初始化调度器
 *
 * @param none
 * 
 * @return none
 */
void sche_Init(void) {
    sche_LockNest = 0;
    sche_NowThread = 0;
    sche_NextThread = 0;

    /*初始状态,只有IDLE*/
    sche_CurrPriority = MAX_PRIORITY_LEVEL - 1;

    #if MAX_PRIORITY_LEVEL <= 8
        bitmap_L1 = 0x80;
    #else
        bitmap_L1 = 0x80000000;
    #endif

    #if MAX_PRIORITY_LEVEL > 32
        memset(bitmap_L2, 0x00, sizeof(bitmap_L2));
    #endif

    /*对所有线程链表组初始化*/
    for (uint16_t offset = 0; offset < MAX_PRIORITY_LEVEL; offset++) {
        osList_HeadInit(&(sche_ReadyList[offset]));
    }

    #if USING_DEID_THREAD_RM == 1
        osList_HeadInit(&(sche_NoReadyList));
    #endif
}


/**
 * 插入线程
 *
 * @param thread 线程单元
 * 
 * @return none
 */
void sche_InsertThread(osThread_Attr_t *thread) {
    register uint32_t level = hal_DisableINT();

    /*插入ready_list*/
    osList_DeleteNode(&(thread->list));
    osList_AddTail(&(sche_ReadyList[thread->priority]), &(thread->list));

    /*线程bitmap插入到内核bitmap里面*/
    bitmap_L1 |= thread->bitmap_Mask;	

    #if MAX_PRIORITY_LEVEL > 32
        bitmap_L2[thread->bitmap_Low_Mask] |= thread->bitmap_High_Mask;
    #endif

    hal_EnableINT(level);
}


/**
 * 移除线程
 *
 * @param thread 线程单元
 * 
 * @return 无
 */
void sche_RemoveThread(osThread_Attr_t* thread) {
    register uint32_t level = hal_DisableINT();

    /*删除节点*/
    osList_DeleteNode(&(thread->list));

    #if USE_DEID_THREAD_RM == 1   
        /*放进未就绪list中*/
        osList_Add(&sche_NoReadyList, &thread->list);
    #endif

    /*检查剩余线程*/
    if (osList_CheckIsEmpty(&sche_ReadyList[thread->priority])) {
        /*移去内核线程队列*/
        #if MAX_PRIORITY_LEVEL <= 32
            bitmap_L1 &= ~thread->bitmap_Mask;
        #else
            bitmap_L2[thread->bitmap_Low_Mask] &= ~thread->bitmap_High_Mask;

            if (bitmap_L2[thread->bitmap_Low_Mask] == 0) {
                bitmap_L1 &= ~thread->bitmap_Mask;
            }
        #endif
    }

    hal_EnableINT(level);
}


/**
 * 交换线程
 *
 * @param none
 * 
 * @return none
 */
void sche_NextToNow(void) {
    sche_NowThread = sche_NextThread;
}


/**
 * 核心调度线程
 *
 * @param none
 * 
 * @return none
 */
void sche_ToNextThread(void) {
    register uint8_t highestPriority;
    
    /*关中断*/
    register uint32_t level;
    level = hal_DisableINT();

    /*检查调度器是否被锁定*/
    if (sche_LockNest == 0) {
        #if MAX_PRIORITY_LEVEL <= 8
            highestPriority = BITMAP[bitmap_L1];
        #else
            register uint8_t pos;

            /**  
             *  对L1 bitmap查询 分别查询4次 对应4字节 直接找出最小bit位(最高就绪) 
             *  对应256bit的哪一大组(8个就绪组为一个大组)
             */
            if (bitmap_L1 & 0xFF) {
                pos = BITMAP[bitmap_L1 & 0xFF];
            }
            else if (bitmap_L1 & 0xFF00) {
                pos = BITMAP[(bitmap_L1 >> 8) & 0xFF] + 8;
            }
            else if (bitmap_L1 & 0xFF0000) {
                pos = BITMAP[(bitmap_L1 >> 16) & 0xFF] + 16;
            }
            else {
                pos = BITMAP[(bitmap_L1 >> 24) & 0xFF] + 24;
            }
            #if MAX_PRIORITY_LEVEL > 32
                /*根据pos找出的数字在二次查询找出L2中真正的就绪小组*/
                highestPriority = (pos << 3) + BITMAP[bitmap_L2[pos]];
            #else
                highestPriority = pos;
            #endif
        #endif

        /*选定最新的线程Block*/
        sche_NextThread = osList_Entry(sche_ReadyList[highestPriority].next, osThread_Attr_t, list);

        /*判断线程是否相同*/
        if(sche_NowThread != sche_NextThread) {
            /*当前最高线程优先级*/
            sche_CurrPriority = highestPriority;

            /*标记线程状态*/
            sche_NextThread->stage = osThreadRunning;
            sche_NowThread->stage = osThreadReady;

            /*调度Call*/
            hal_CallPendSV();
        }
    }

    /*开中断*/
    hal_EnableINT(level);
}


/**
 * os调度器时间处理函数
 *
 * @param none
 * 
 * @return none
 */
void sys_TickHandler(void) {
    /*中断嵌套+1*/
    osSys_ISREnter();

    /*系统节拍+1*/
    ++sys_TickCount;

    /*线程时间片-1*/
    --sche_NowThread->timeSlice;

    /*时间片耗尽重新调度*/
    if (sche_NowThread->timeSlice == 0) {
        sche_NowThread->timeSlice = sche_NowThread->initTimeSlice;

        osThread_Yield();
    }

    /*检查定时器*/
    timer_TickCheck();

    /*中断嵌套-1*/
    osSys_ISRLeave();
}

/*@}*/

/**
 * @addtogroup schedule user functions
 */
 
/*@{*/

/**
 * 锁定调度器
 *
 * @param none
 * 
 * @return none
 */
void osSche_Lock(void) {
    register uint32_t level = hal_DisableINT();

    sche_LockNest++;

    hal_EnableINT(level);
}
EXPORT_SYMBOL(osSche_Lock);


/**
 * 解锁调度器
 *
 * @param none
 * 
 * @return none
 */
void osSche_Unlock(void) {
    register uint32_t level = hal_DisableINT();

    sche_LockNest --;

    if (sche_LockNest <= 0) {
        /*恢复调度*/
        sche_LockNest = 0;

        /*调度*/
        sche_ToNextThread();
    }

    /*打开中断*/
    hal_EnableINT(level);
}
EXPORT_SYMBOL(osSche_Unlock);


/**
 * 获取最近的tick数
 *
 * @param none
 * 
 * @return none
 */
osTick_t osSys_GetNowTick(void) {
    return sys_TickCount;
}
EXPORT_SYMBOL(osSys_GetNowTick);

/*@}*/
