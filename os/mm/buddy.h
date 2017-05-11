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

#ifndef BUDDY_H_
#define BUDDY_H_

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "../arch/platform.h"

#include "../osConfig.h"

/*@}*/

#if USING_BUDDY_MANAGER == 1
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
    * @addtogroup buddy system functions
    */
        
    /*@{*/

    extern void mem_Init(uint32_t memStart, uint32_t memEnd);

    /*@}*/

    /**
    * @addtogroup buddy user functions
    */
        
    /*@{*/

    extern void* osMem_Malloc(uint32_t size);
    extern void osMem_Free(void* address);

    /*@}*/
    
#else
    #include <stdlib.h>
    
    #define osMem_Malloc malloc
    #define osMem_Free free
#endif
#endif
