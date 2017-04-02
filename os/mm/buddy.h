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
 * File      : buddy.h
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

#ifndef BUDDY_H_
#define BUDDY_H_

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "../arch/util.h"
#include "../osConfig.h"

/*@}*/

#if USING_BUDDY_MANAGER == 1

/**
 * @addtogroup buddy configure 
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
#define BUDDY_BLOCK_SIZE    32  /**< buddy块大小 单位(byte)*/
#define BUDDY_ORDER         6   /**< buddy块阶 */

/*@}*/

/**
 * @addtogroup buddy Memory info 
 */
 
/*@{*/

typedef struct osMem_Info {
    uint32_t heapStart;     /**< 内存起址*/
    uint32_t heapEnd;       /**< 内存结束*/

    uint32_t total;     /**< buddy块大小 单位(byte)总内存*/
    uint32_t remaining; /**< 用户可用内存剩余量*/
} osMem_t;

extern osMem_t osMem_Info;

/*@}*/

/**
 * @addtogroup Memory functions 
 */
 
/*@{*/

extern void Buddy_Init(uint32_t memStart, uint32_t memEnd);
#define osMem_Init Buddy_Init

extern void* Buddy_Malloc(uint32_t size);
#define osMem_Malloc Buddy_Malloc

extern void Buddy_Free(void* address);
#define	osMem_Free Buddy_Free

/*@}*/

#endif

#endif
