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
 * @addtogroup system
 */

/*@{*/

/*@}*/ 

/**
 * @addtogroup Key var
 */
 
/*@{*/

/**
 *  前次按键扫描值
 *  @note none
 */
static mKey_Bitmap_t preScanKey = 0;                     

/**
 *  前次按键读取值
 *  @note none
 */
static mKey_Bitmap_t preReadKey = 0;  

#if USE_SHIFT_KEY == 1
/**
 *  shift按键记录
 *  @note none
 */
static mKey_Bitmap_t keyShift = 0;
#endif

/**
 *  按键掩码
 *  @note none
 */
static mKey_Bitmap_t keyMask = 0;

#if LONG_KEY_EN == 1
static mKey_Bitmap_t keyPressTmr = KEY_PRESS_DLY;
#endif

/*@}*/

/**
 * @addtogroup FIFO Buffer var
 */
 
/*@{*/

/**
 *  环形buf
 *  @note none
 */
static mKey_Bitmap_t keyBuf[KEY_BUFFER_SIZE];
/**
 *  buf入指针
 *  @note none
 */
static mKey_Bitmap_t keyBufInIx = 0;      
/**
 *  buf出指针
 *  @note none
 */
static mKey_Bitmap_t keyBufOutIx = 0; 

/**
 *  buf中按键数量
 *  @note none
 */
static mKey_Bitmap_t keyNRead = 0;

/*@}*/

/**
 * @addtogroup Key Functions
 */
 
/*@{*/

/**
 *  读取一次GPIO状态
 *
 *  @param none
 *
 *  @return uint8_t key扫描结果
 */
OS_INLINE mKey_Bitmap_t GPIORead(void) {
  mKey_Bitmap_t keyScanCode = 0;

  keyScanCode |= GPIOF->IDR & GPIO_Pin_1 ? 0 : KEY_OK_FLAG;
  keyScanCode |= GPIOA->IDR & GPIO_Pin_0 ? 0 : KEY_MENU_FLAG;
  keyScanCode |= GPIOA->IDR & GPIO_Pin_1 ? 0 : KEY_DOWN_FLAG;
  keyScanCode |= GPIOA->IDR & GPIO_Pin_2 ? 0 : KEY_UP_FLAG;

  return keyScanCode;
}


/**
 *  初始化GPIO
 *
 *  @param none
 *
 *  @return none
 */
void mKey_InterfaceInit(void) {
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOF, ENABLE);
  
  GPIO_InitTypeDef gpio;
  gpio.GPIO_Mode = GPIO_Mode_IN;
  gpio.GPIO_Pin = GPIO_Pin_1;
  gpio.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &gpio);
  
  gpio.GPIO_Mode = GPIO_Mode_IN;
  gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
  gpio.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &gpio);
}


/**
 *  读取缓冲区中按键信息
 *
 *  @param none
 *
 *  @return mKey_Bitmap_t key
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
 *  写入一个按键信息到缓冲区
 *
 *  @param none
 *
 *  @return none
 */
static void KeyBufferIn(mKey_Bitmap_t code) {    
	/*buf满则放弃最早的一个按键值*/
	if(keyNRead >= KEY_BUFFER_SIZE) {               
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
 *  周期扫描按键
 *
 *  @param none
 *
 *  @return none
 */
void mKey_Scan(void) {
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

#if LONG_KEY_EN == 1      
	/*用电平触发做长按键的有效判据*/
	if (nowReadKey == preReadKey && nowReadKey) {
		keyPressTmr--;
		
		/*长按判断周期到,保存相应长按键值*/
		if(!keyPressTmr) {
			/*长按键-连发模式*/
			if(nowReadKey & ~(KEY_LONG_SHIFT)) {
#if USE_SHIFT_KEY == 1
				KeyBufferIn(nowReadKey | keyShift);
#else
				KeyBufferIn(nowReadKey);
#endif
			}
			
			/*长按键-反码模式*/
			else if (nowReadKey & (KEY_LONG_SHIFT) & ~keyMask ) {
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
#endif
	
	/*短按键判断*/
	if (keyRelease) {
		if(keyRelease & (~keyMask)) {
			/*shift按键码(边缘触发)*/
#if USE_SHIFT_KEY == 1
			keyShift ^= (keyRelease & (KEY_SHIFT));
      
			KeyBufferIn(keyRelease | keyShift);
#else
			KeyBufferIn(keyRelease);
#endif
			
		}else{
			keyMask = 0;
		}
	}
  
  preScanKey = nowScanKey;
  preReadKey = nowReadKey; 
}


/**
 *  获取按键
 *
 *  @param none
 *
 *  @return mKey_Bitmap_t 按键bitmap
 */
__inline mKey_Bitmap_t mKey_Get(void) {                         
  return KeyBufferOut();
}


/**
 *  判断是否有按键按下
 *
 *  @param none
 *
 *  @return bool 返回结果
 */
__inline bool mKey_IsHit(void) {
  return (bool)(keyNRead > 0) ? true : false;
}

/*@}*/
