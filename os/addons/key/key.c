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
 * File      : key.c
 * This file is part of nyaOS
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

#include "./key.h"

/**
 * @addtogroup hal
 */
 
/*@{*/

#include "stm32f7xx.h"                  // Device header

/*@}*/

/**
 * @addtogroup os include
 */

/*@{*/

#include "../../arch/platform.h"

#include "../../lib/list.h"

#include "../../osAPI.h"

/*@}*/

/**
 * @addtogroup add ons
 */
 
/*@{*/

#include "addons/console/console.h"

/*@}*/

/**
 * @addtogroup Key var
 */
 
/*@{*/

/**
 * 前次按键扫描值
 * @note none
 */
static mKey_Bitmap_t preScanKey = 0;                     

/**
 * 前次按键读取值
 * @note none
 */
static mKey_Bitmap_t preReadKey = 0;  

/**
 * 按键掩码
 * @note none
 */
static mKey_Bitmap_t keyMask = 0;

#if USE_SHIFT_KEY == 1
/**
 * shift按键记录
 * @note none
 */
static mKey_Bitmap_t keyShift = 0;

/**
 * shift按键对象
 * @note none
 */
static mKeyDef_t key_ShiftObj = {
  .port = 0,
  .value = 0x01
};
#endif

static mKey_Bitmap_t keyPressTmr = KEY_PRESS_DLY;

/**
 * 长按键编码掩码
 * @note none
 */
static mKey_Bitmap_t key_LongMask;

/**
 * 按键对象集合
 * @note none
 */
static struct osList_t keysList;

/*@}*/

/**
 * @addtogroup FIFO Buffer var
 */
 
/*@{*/

/**
 * 环形buf
 * @note none
 */
static mKey_Bitmap_t keyBuf[KEY_BUFFER_SIZE];

/**
 * buf入指针
 * @note none
 */
static mKey_Bitmap_t keyBufInIx = 0;    

/**
 * buf出指针
 * @note none
 */
static mKey_Bitmap_t keyBufOutIx = 0; 

/**
 * buf中按键数量
 * @note none
 */
static mKey_Bitmap_t keyNRead = 0;

/*@}*/

/**
 * @addtogroup Key Functions
 */
 
/*@{*/

/**
 * 读取一次GPIO状态
 *
 * @param none
 *
 * @return uint8_t key扫描结果
 */
static OS_INLINE mKey_Bitmap_t GPIORead(void) {
  mKey_Bitmap_t keyScanCode = 0;
  mKeyDef_t *obj;
  struct osList_t *index;

#if USE_SHIFT_KEY == 1
  keyScanCode |= ((((GPIO_TypeDef *)key_ShiftObj.port)->IDR) & key_ShiftObj.gpio) ? (key_ShiftObj.level ? key_ShiftObj.value : 0) : (key_ShiftObj.level ? 0 : key_ShiftObj.value);
#endif

  osList_ForEach(index, &keysList) {
    obj = osList_Entry(index, mKeyDef_t, list);
    
    keyScanCode |= ((((GPIO_TypeDef *)obj->port)->IDR) & obj->gpio) ? (obj->level ? obj->value : 0) : (obj->level ? 0 : obj->value);
  }  

  return keyScanCode;
}


/**
 * 读取缓冲区中按键信息
 *
 * @param none
 *
 * @return mKey_Bitmap_t key
 */
static mKey_Bitmap_t KeyBufferOut(void) {
	mKey_Bitmap_t code = 0;
	
	if (keyNRead > 0) {     
		/*-1表示取走一个按键值*/
		keyNRead--;
		
		/*从buf中取走一个扫描码*/
		code = keyBuf[keyBufOutIx];
		keyBufOutIx++;
		
		/*buf满则指针循环回到起点*/
		if (keyBufOutIx >= KEY_BUFFER_SIZE) {
			keyBufOutIx = 0;
		}
		
		/*返回按键扫描值*/
		return (code); 
	}
	else {
		/*没有按键,则返回0*/
		return 0;
	}
}


/**
 * 写入一个按键信息到缓冲区
 *
 * @param none
 *
 * @return none
 */
static void KeyBufferIn(mKey_Bitmap_t code) {    
	/*buf满则放弃最早的一个按键值*/
	if (keyNRead >= KEY_BUFFER_SIZE) {               
		KeyBufferOut();                                                 
	}

	keyNRead++;
	
	/*输入按键扫描码*/
	keyBuf[keyBufInIx++] = code;
	
	/*buf满则指针循环回到起点*/
	if (keyBufInIx >= KEY_BUFFER_SIZE) {        
		keyBufInIx = 0;
	}                      
}


/**
 * 周期扫描按键
 *
 * @param none
 *
 * @return none
 */
static void mKey_Scan(void) {
	/*当前按键值扫描值*/
	mKey_Bitmap_t nowScanKey = 0;
	/*当前按键值*/
	mKey_Bitmap_t nowReadKey = 0;
	
	/*按键按下*/
	//mKey_Bitmap_t keyPressDown = 0;
  
	/*按键释放*/
	mKey_Bitmap_t keyRelease = 0;
	
	nowScanKey = GPIORead();
	
	/*电平触发 | 采样保持(即消抖)*/
	nowReadKey = (preScanKey & nowScanKey) | (preReadKey & (preScanKey ^ nowScanKey));

	/*上升沿触发*/
	//KeyPressDown  = NowReadKey & (NowReadKey ^ PreReadKey);  
  
	/*下降沿触发*/
	keyRelease = (preReadKey & (nowReadKey ^ preReadKey));

	/*用电平触发做长按键的有效判据*/
	if (nowReadKey == preReadKey && nowReadKey) {
		keyPressTmr--;
		
		/*长按判断周期到,保存相应长按键值*/
		if (!keyPressTmr) {
			/*长按键-连发模式*/
			if (nowReadKey & ~(key_LongMask)) {
#if USE_SHIFT_KEY == 1
				KeyBufferIn(nowReadKey | keyShift);
#else
				KeyBufferIn(nowReadKey);
#endif
			}
			
			/*长按键-反码模式*/
			else if (nowReadKey & (key_LongMask) & ~keyMask) {
#if USE_SHIFT_KEY == 1
				KeyBufferIn(~(nowReadKey | keyShift));  
#else
				KeyBufferIn(~nowReadKey);
#endif
			}
			
			/*重置连按周期,准备获取下1个长按键*/
			keyPressTmr = KEY_PRESS_TMR;
      
			keyMask = nowReadKey;
		}
	}
	else {
		/*按键变化,重置按键判断周期*/
		keyPressTmr = KEY_PRESS_DLY;
	}
	
	/*短按键判断*/
	if (keyRelease) {
		if (keyRelease & (~keyMask)) {
			/*shift按键码(边缘触发)*/
#if USE_SHIFT_KEY == 1
			keyShift ^= (keyRelease & (key_ShiftObj.keyValue));
      
			KeyBufferIn(keyRelease | keyShift);
#else
			KeyBufferIn(keyRelease);
#endif
		}
		else {
			keyMask = 0;
		}
	}
  
  preScanKey = nowScanKey;
  preReadKey = nowReadKey; 
}


/**
 * 获取按键
 *
 * @param none
 *
 * @return mKey_Bitmap_t 按键bitmap
 */
static OS_INLINE mKey_Bitmap_t mKey_Get(void) {                         
  return KeyBufferOut();
}


/**
 * 判断是否有按键按下
 *
 * @param none
 *
 * @return uint8_t 返回结果
 */
static OS_INLINE uint8_t mKey_IsHit(void) {
  return keyNRead > 0 ? 1 : 0;
}

/*@}*/

/**
 * @addtogroup Key Thread def
 */
 
/*@{*/

#if OS_DEBUG_MODE == 1
uint32_t mKey_Count = 0;
#endif

osThread_FuncDef(thread_mKey) {
  mKey_Bitmap_t keyGet;
  
  mKeyDef_t *obj;
  struct osList_t *index;
  
#if OS_DEBUG_MODE == 1
  mLog_ThreadPrintf(Log_I, "mKey", 0, CONSOLE_YELLOW "Startup. FreeMem=%.1f Kbyte" CONSOLE_NONE, osMem_Info.remaining / 1024.0f);
#endif
  
  for (;;) {
#if OS_DEBUG_MODE == 1
    mKey_Count++;
#endif    
    
    osThread_Delay(30);
    
    osSche_Lock();
    
    mKey_Scan();
  
    if (!mKey_IsHit()) {
      osSche_Unlock();
      
      continue;
    }
  
    keyGet = mKey_Get();
    
    osList_ForEach(index, &keysList) {
      obj = osList_Entry(index, mKeyDef_t, list);
      if (obj->callback) {
        obj->callback(obj, keyGet);
      }
    }
    osSche_Unlock();
  }
}
osThread_Def(thread_mKey, KEY_THREAD_PRIORITY, KEY_THREAD_STACK_SIZE, thread_mKey);
osThread_Id thread_mKey_ID;

/*@}*/

/**
 * @addtogroup mKey user functions
 */
 
/*@{*/

/**
 * 按键模块启动
 *
 * @param none
 *
 * @return none
 */
void mKey_ModuleStartup(void) {
#if USE_SHIFT_KEY == 1
  /*检查shift是否初始化*/
  if (!key_ShiftObj.port) {
    for (;;);
  }
#endif
  
  osList_HeadInit(&keysList);

  thread_mKey_ID = osThread_Create(osThread_Obj(thread_mKey), (void *)0);
  osThread_Ready(thread_mKey_ID);
}


#if USE_SHIFT_KEY == 1
/**
 * shift按键初始化
 *
 * @param port
 * @param gpio 
 *
 * @return none
 */
void mKey_ShiftKeySet(uint32_t port, uint32_t gpio, uint8_t level) {
  key_ShiftObj.port = port;
  key_ShiftObj.gpio = gpio;
  key_ShiftObj.level = level;
}
#endif


/**
 * 按键对象插入
 *
 * @param obj 按键对象指针
 *
 * @return none
 */
void mKey_ObjInsert(mKeyDef_t *obj) {
  osSche_Lock(); {
    osList_HeadInit(&obj->list);
    osList_AddTail(&keysList, &obj->list);
    
    if (obj->type & KEY_TYPE_LONG) {
      key_LongMask |= obj->value;
      obj->valueLong = ~obj->value;
    }
  } osSche_Unlock();
}


/**
 * 判断按键是否输出类型(短/长按/shift)
 *
 * @param obj 按键对象指针
 *
 * @return 类型
 */
uint8_t mKey_GetPressType(mKeyDef_t *obj, mKey_Bitmap_t vaule) {
#if USE_SHIFT_KEY == 1
  if (vaule == obj->value) {
    return KEY_IS_SP;
  }
  else if (vaule == (~(obj->vaule | key_ShiftObj.vaule))) {
    return KEY_IS_SLP;
  }
  else if (vaule == (obj->vaule | key_ShiftObj.vaule)) {
    return KEY_IS_SSP;
  }
  else {
    return KEY_IS_LP;
  }
#else
  if (obj->value == vaule) {
    return KEY_IS_SP;
  }
  else {
    return KEY_IS_LP;
  }
#endif
}

/*@}*/
