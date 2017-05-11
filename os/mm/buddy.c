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
 * File      : buddy.c
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

#include "./buddy.h"

#if USING_BUDDY_MANAGER == 1

/**
 * @addtogroup ANSI-C Include
 */

/*@{*/

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

/*@}*/

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "../lib/symbolExport.h"
#include "../lib/list.h"

#include "../arch/hal_cm.h"

/*@}*/

/**
 * @addtogroup buddy 基本定义
 */
 
/*@{*/

/*单个内存块*/
typedef struct BuddyBlock {
  /**
   * 状态标识
   * bit0-3表示该内存块是2^n
   * bit7-> 1=正在使用 0=可用
   */
  uint8_t stage;
	
  struct osList_Head_t list;  /**< 该结构体是一个双向链表节点 */
} BuddyBlock_t;

/*@}*/

/**
 * @addtogroup buddy 变量
 */
 
/*@{*/

/*页面状态1*/
#define PAGE_BUSY         0x80
#define PAGE_AVAILABLE    0x00

/*页面状态2*/
#define PAGE_DIRTY        0x40

/*内存块结构大小*/
#define BLOCK_STRUCT_SIZE   ((uint32_t) sizeof(BuddyBlock_t))

/*最大order阶*/
#define BUDDY_MAX_ORDER     (BUDDY_ORDER - 1)

/*buddy order 2^n阶 链表*/
struct osList_Head_t buddy_list[BUDDY_ORDER];

/*内存信息*/
osMem_t osMem_Info;

/*@}*/

/**
 * @addtogroup buddy 函数
 */
 
/*@{*/

//static __inline void* __GetBlockAddress(BuddyBlock_t* block) {
//	/*将链表占用的空间也分配出去,剩余一个(stage)*/
//	return (void *)(block + sizeof(unsigned char));
//	//return (void *)((unsigned int) block + (unsigned int) (sizeof(unsigned char)));
//}


/**
 * 初始化buddy
 *
 * @param memStart 内存起址
 * @param memEnd 内存结束
 * 
 * @return none
 */
void mem_Init(uint32_t memStart, uint32_t memEnd) {
  /*内存对齐标志*/
  uint32_t memoryMask = ~(BUDDY_BLOCK_SIZE - 1);

  /*初始化buddy链表*/
  for (uint16_t i = 0; i < BUDDY_ORDER; i++) {
    osList_HeadInit(&(buddy_list[i]));
  }

  /*内存开端加上管理结构体大小为buddy管理分配区域*/
  osMem_Info.heapStart = ((memStart + (~memoryMask)) & (memoryMask));

  /*内存末端减去栈的大小*/
  osMem_Info.heapEnd = memEnd;
  osMem_Info.total = osMem_Info.remaining = (osMem_Info.heapEnd) - (osMem_Info.heapStart);

  /*计算剩余内存能分出多少最大内存块*/
  uint32_t maxOrderNumber = (osMem_Info.remaining) / ((1 << (BUDDY_MAX_ORDER)) * BUDDY_BLOCK_SIZE);

  /*连续的将剩余内存分配成最大内存块*/
  for (uint32_t i = 0; i < maxOrderNumber; i++) {
    BuddyBlock_t* block = (BuddyBlock_t *) (osMem_Info.heapStart + (i * (((1 << (BUDDY_MAX_ORDER)) * BUDDY_BLOCK_SIZE))));
    
    block->stage = BUDDY_MAX_ORDER;
    block->stage &= ~PAGE_BUSY;

    osList_HeadInit(&(block->list));
    osList_AddTail(&(buddy_list[BUDDY_MAX_ORDER]), &(block->list));
  }

  /*求剩余多少未分配空间,变成最小内存块*/
  uint32_t minOrderNumber = (osMem_Info.remaining - (maxOrderNumber * ((1 << (BUDDY_MAX_ORDER)) * BUDDY_BLOCK_SIZE))) / (BUDDY_BLOCK_SIZE);

  /*最小块初始化起址*/
  uint32_t minOrderBlockAddressBase = (osMem_Info.heapStart + (maxOrderNumber * (((1 << (BUDDY_MAX_ORDER)) * BUDDY_BLOCK_SIZE))));

  /*将剩余未分配空间全部纳入order0里面*/
  for (uint32_t i = 0; i < minOrderNumber; i++) {
    BuddyBlock_t* block = (BuddyBlock_t *)((minOrderBlockAddressBase) + (i * BUDDY_BLOCK_SIZE));
    
    block->stage = 0;
    block->stage &= ~PAGE_BUSY;
    
    osList_HeadInit(&(block->list));
    osList_AddTail(&(buddy_list[0]), &(block->list));
  }
}


/**
 * 从order序列里取出相应块
 *
 * @param order 序列
 * 
 * @return none
 */
static BuddyBlock_t* allocateBlock(uint32_t order) {
  BuddyBlock_t* block;

  /*寻找有没有空闲并且可用页面*/
  for (uint32_t i = order; i <= BUDDY_MAX_ORDER; i++) {
    /*查看当前指向的order链表有没有节点*/
    if (osList_CheckIsEmpty(&(buddy_list[i]))) {
      continue;
    }

    /*找到节点,获取结构体*/
    block = osList_Entry(buddy_list[i].next, BuddyBlock_t, list);

    /*路过这一块正在使用,推出申请*/
    if (block->stage & PAGE_BUSY) {
      break;
    }

    /*删除该节点*/
    osList_DeleteNode(&block->list);

    /*迭代分割大内存块,直到切到适合内存块*/
    while (i > order) {
      --i;

      BuddyBlock_t *newBlock = (BuddyBlock_t *)((uint32_t)block + ((1 << i) * BUDDY_BLOCK_SIZE));

      newBlock->stage = i;
      newBlock->stage &= ~PAGE_BUSY;

      osList_HeadInit(&(newBlock->list));
      osList_AddTail(&(buddy_list[i]), &(newBlock->list));
    }

    /*剩余空间消减*/
    //该内存块正在使用,曾经使用
    block->stage = 0;
    block->stage = order;
    block->stage |= PAGE_BUSY;

    /*剩余内存--*/
    osMem_Info.remaining -= ((1 << order) * BUDDY_BLOCK_SIZE);

    /*返回内存块首地址*/
    return block;
  }   

  /*错误,返回0*/
  return 0;
}


/**
 * 找伙伴
 *
 * @param block 内存块
 * @param order 等级
 * 
 * @return 返回伙伴地址
 */
OS_INLINE void* findBuddyBlock(BuddyBlock_t* block, uint32_t order) {
  /*计算长度*/
  uint32_t length = ((uint32_t)block) - osMem_Info.heapStart;

  /*偏移下一个伙伴*/
  uint32_t buddyBlockAddress = length + ((1 << order) * BUDDY_BLOCK_SIZE);

  /*返回伙伴地址*/
  return (void *)(buddyBlockAddress + osMem_Info.heapStart);
}


/**
 * 释放内存块
 *
 * @param block 内存块
 * 
 * @return none
 */
static void __FreeBlock(BuddyBlock_t * block) {
  uint8_t blockOrder;

  /*消除内存块状态位*/
  blockOrder = block->stage;
  blockOrder &= ~PAGE_BUSY;

  /*迭代合并*/
  while (blockOrder < BUDDY_MAX_ORDER) {
    /*找伙伴*/
    BuddyBlock_t* buddyBlock = (BuddyBlock_t *) findBuddyBlock(block, blockOrder);

    /*找错伙伴,不干活*/
    if ((buddyBlock->stage != blockOrder)) {
      break;
    }

    /*伙伴很忙,不干活*/
    if (buddyBlock->stage & PAGE_BUSY) {
      break;
    }

    /*删除伙伴所在的链表*/
    osList_DeleteNode(&buddyBlock->list);

    /*合并大小*/
    if (buddyBlock < block) {
      block = buddyBlock;
    }

    ++blockOrder;

    /*清除块信息*/
    block->stage = 0;

    /*载入合并后信息*/
    block->stage = blockOrder;
  }

  /*载入信息*/
  block->stage = blockOrder;
  block->stage &= ~PAGE_BUSY;

  /*最后加入到链表*/
  osList_HeadInit(&(block->list));
  osList_AddTail(&(buddy_list[blockOrder]), &(block->list));

  /*剩余内存++*/
  osMem_Info.remaining += ((1 << blockOrder) * BUDDY_BLOCK_SIZE);
}


/**
 * 申请内存
 *
 * @param size 申请大小
 * 
 * @return 内存地址
 */
void* osMem_Malloc(uint32_t size) {
  /*传入0,错误*/
  if (!size) {
    return NULL;
  }

  /*计算需要哪个order的内存块*/
  uint8_t calculateOrder = (size + BLOCK_STRUCT_SIZE) / (BUDDY_BLOCK_SIZE);
  uint8_t wantOrder = 0;
  while (calculateOrder) {
    wantOrder++;
    calculateOrder = calculateOrder >> 1;
  };

  BuddyBlock_t* block;
  
  register uint32_t level;
  level = hal_DisableINT();

  /*获取需要的页面*/
  block = allocateBlock(wantOrder);

  /*获取失败*/
  if (!block) {
    return NULL;
  }

  hal_EnableINT(level);

  /*返回地址*/
  /*将链表占用的空间也分配出去,剩余一个(stage)*/
  return (block + sizeof(uint8_t));
}
EXPORT_SYMBOL(osMem_Malloc);


/**
 * 释放内存
 *
 * @param address 地址
 * 
 * @return none
 */
void osMem_Free(void* address) {
  register uint32_t level;
  level = hal_DisableINT();
  
  /*内存块结构体基地址 = 分配出去的内存地址偏移-结构体大小*/
  BuddyBlock_t* block = (BuddyBlock_t *) ((uint32_t)address - (uint32_t) sizeof(uint8_t));

  /*释放内存块*/
  if (block) {
    __FreeBlock(block);
  }

  hal_EnableINT(level);
}
EXPORT_SYMBOL(osMem_Free);

/*@}*/

#endif
