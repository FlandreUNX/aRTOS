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
 * File      : ringBuffer.c
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
 
#include "./ringBuffer.h"
#include "./symbolExport.h"

/**
 * @addtogroup 具体方法
 */
 
/*@{*/

/**
 * 创建RingBuffer
 *
 * @param size RingBuffer大小(不包含buufer必要信息)
 * 
 * @return RingBuffer有效句柄
 * @return 0: 创建失败
 */
osRBuffer_t* osRBuffer_Create(uint16_t size) {
    osRBuffer_t* buffer;

    /*malloc struct*/
    //buffer = (osRBuffer_t *)nyaMemory_Malloc(sizeof(osRBuffer_t) + size);

    if (!buffer) {
        return 0;
    }

    /*initialization*/
    buffer->size = size;
    buffer->in = 0;
    buffer->out = 0;
    buffer->pBuffer = (uint8_t *)((uint32_t)buffer + sizeof(osRBuffer_t));

    return buffer;
}
SYMBOL_EXPORT(osRBuffer_Create);


/**
 * 创建静态RingBuffer
 *
 * @param *pool RingBuffer容器
 * @param size RingBuffer容器大小(包含buffer结构体信息)
 * 
 * @return RingBuffer有效句柄
 */
osRBuffer_t* osRBuffer_StaticCreate(void *pool, uint16_t size) {
    osRBuffer_t* buffer;

    /*malloc struct*/
    buffer = (osRBuffer_t *)pool;

    /*initialization*/
    buffer->size = size - sizeof(osRBuffer_t);
    buffer->in = 0;
    buffer->out = 0;
    buffer->pBuffer = (uint8_t *)((uint32_t)buffer + sizeof(osRBuffer_t));

    return buffer;
}
SYMBOL_EXPORT(osRBuffer_StaticCreate);


/**
 * 检查buffer储存状态
 *
 * @param *obj RingBuffer对象
 * 
 * @return 存放有效数据量
 */
enum BUFFER_STAGE osRBuffer_GetStatus(osRBuffer_t *obj) {
    if (obj->in == obj->out) {
        if (obj->out_Mirror == obj->in_Mirror) {
            return RBUFFER_EMPTY;
        }
        else {
            return RBUFFER_FULL;
        }
    }

    return RBUFFER_HALFFULL;
}
SYMBOL_EXPORT(osRBuffer_GetStatus);


/**
 * 检查多少数据在buffer里面
 *
 * @param *obj RingBuffer对象
 * 
 * @return 存放有效数据量
 */
uint16_t osRBuffer_GetDataLen(osRBuffer_t *obj) {
    uint16_t len;

    switch (osRBuffer_GetStatus(obj)) {
        case RBUFFER_EMPTY : {
            len = 0;
        }break;

        case RBUFFER_FULL : {
            len = obj->size;
        }break;

        case RBUFFER_HALFFULL : 
        default: {
            if (obj->in > obj->out) {
                len = obj->in - obj->out;
            }
            else {
                len = obj->size - (obj->out - obj->in);
            }
        }break;
    };

    return len;
}
SYMBOL_EXPORT(osRBuffer_GetDataLen);


/**
 * 返回剩余空间
 *
 * @param *obj RingBuffer对象
 * 
 * @return 有效剩余
 */
uint16_t osRBuffer_GetSpaceLen(osRBuffer_t *obj) {
    return (obj)->size - osRBuffer_GetDataLen(obj);
}
SYMBOL_EXPORT(osRBuffer_GetSpaceLen);


/**
 * 插入数据到RingBuffer
 *
 * @param *obj RingBuffer对象
 * @param *src 数据源
 * @param len 数据长度
 * 
 * @return 存放有效数据量
 */
uint16_t osRBuffer_Put(osRBuffer_t *obj, const uint8_t *src, uint16_t len) {
    uint16_t size;

    /*检查数据是否超过size*/
    size = osRBuffer_GetSpaceLen(obj);

    /*空间不足*/
    if (size == 0) {
        return 0;
    }
    /*有空间但不足放下全部,丢弃一部分*/
    if (size < len) {
        len = size;
    }

    /*in在out的情况,len不大于前方的空间时*/
    if (obj->size - obj->in > len) {
        /*复制数据, 剩余空间 = out - in*/
        memcpy(&obj->pBuffer[obj->in], src, len);
        obj->in += len;

        return len;
    }

    /*情况2,空间足够,但是in需要回绕到0的时候*/
    memcpy(&obj->pBuffer[obj->in],
        &src[0],
        obj->size - obj->in);

    memcpy(&obj->pBuffer[0],
        &src[obj->size - obj->in],
        len - (obj->size - obj->in));

    obj->in_Mirror = ~obj->in_Mirror;
    obj->in = len - (obj->size - obj->in);

    return len;
}
SYMBOL_EXPORT(osRBuffer_Put);


/**
 * 获取数据
 *
 * @param *obj RingBuffer对象
 * @param *dest 数据目标
 * @param len 数据长度
 * 
 * @return 有效数据量
 */
uint16_t osRBuffer_Get(osRBuffer_t *obj, uint8_t *dest, uint16_t len) {
    uint16_t size;

    /*检查数据存储量*/
    size = osRBuffer_GetDataLen(obj);

    /*无数据,退出*/
    if (size == 0) {
        return 0;
    }

    /*接受的长度少于储存量,就拿出len的量*/
    if (size < len) {
        len = size;
    }

    if (obj->size - obj->out > len) {
        /*读取所有数据*/
        memcpy(dest, &obj->pBuffer[obj->out], len);
        obj->out += len;

        return len;
    }
  
    /*回绕到0的情况*/
    memcpy(&dest[0],
        &obj->pBuffer[obj->out],
        obj->size - obj->out);

    memcpy(&dest[obj->size - obj->out],
        &obj->pBuffer[0],
        len - (obj->size - obj->out));

    obj->out_Mirror = ~obj->out_Mirror;
    obj->out = len - (obj->size - obj->out);

    return len;
}
SYMBOL_EXPORT(osRBuffer_Get);


/**
 * 插入一个字节到RB中
 *
 * @param *obj RingBuffer对象
 * @param src 数据源
 * 
 * @return 有效数据量
 */
uint8_t osRBuffer_PutByte(osRBuffer_t *obj, uint8_t src) {
    /*检查剩余空间*/
    if (!osRBuffer_GetSpaceLen(obj)) {
        return 0;
    }

    obj->pBuffer[obj->in] = src;

    /*到达size边界,镜像翻转,回绕到0*/
    if (obj->in == obj->size - 1) {
        obj->in_Mirror = ~obj->in_Mirror;
        obj->in = 0;
    }
    else {
        obj->in++;
    }

    return 1;
}
SYMBOL_EXPORT(osRBuffer_PutByte);


/**
 * 获取一个字节
 *
 * @param *obj RingBuffer对象
 * @param *dest 目标源
 * 
 * @return 有效数据量
 */
uint8_t osRBuffer_GetByte(osRBuffer_t *obj, uint8_t *dest) {
    /*检查是不是空RB*/
    if (!osRBuffer_GetDataLen(obj)) {
        return 0;
    }

    *dest = obj->pBuffer[obj->out];

    /*到达size边界,镜像翻转,回绕到0*/
    if (obj->out == obj->size - 1) {
        obj->out_Mirror = ~obj->out_Mirror;
        obj->out = 0;
    }
    else {
        obj->out++;
    }

    return 1;
}
SYMBOL_EXPORT(osRBuffer_GetByte);


/**
 * 清空缓冲
 *
 * @param *obj RingBuffer对象
 * 
 * @return none
 */
void osRBuffer_Clear(osRBuffer_t *obj) {
    obj->in = obj->out = 0;
    obj->in_Mirror = obj->out_Mirror = 1;
}
SYMBOL_EXPORT(osRBuffer_Clear);

/*@}*/
