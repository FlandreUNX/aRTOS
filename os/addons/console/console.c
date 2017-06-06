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
 * File      : con_Interface.c
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
 
#include "./console.h"

/**
 * @addtogroup Hal file include
 */

/*@{*/

#include "stm32f7xx.h"                  // Device header
#include "stm32f7xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

/*@}*/

/**
 * @addtogroup std C lib include
 */

/*@{*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/*@}*/

/**
 * @addtogroup OS file include
 */

/*@{*/

#include "osAPI.h"

#include "../../lib/symbolExport.h"

/*@}*/

/**
 * @addtogroup Interface
 */

/*@{*/

extern void console_PortInit(void);

extern void console_PortOutput(const char *log, uint16_t size);

/*@}*/

/**
 * @addtogroup variables
 */

/*@{*/

/**
 *  log输出优先级描述符字符串
 *  @note none
 */
const char* logLevelString[6] = {
  [Log_A] = "\033[0;33mAssert\033[0m",   /**< 棕色 */
  [Log_E] = "\033[0;31mError\033[0m",    /**< 红色 */
  [Log_W] = "\033[0;35mWarn\033[0m",      /**< 紫色 */    
  [Log_I] = "\033[0;32mInfo\033[0m",     /**< 绿色 */
  [Log_D] = "Debug",
  [log_V] = "Verbose"
};

/**
 *  输出缓冲器
 *  @note none
 */
char outputBuffer[OUTPUT_BUFFER_SIZE] = {0};

/**
 *  log输出等级过滤
 *  @note none
 */
Log_Level log_LevelFiter = log_V;

/**
 *  log输出等级屏蔽mask
 *  @note 标记则为允许输出
 */
uint8_t log_LevelMask = 0x00;

/**
 *  log状态
 *  @note bit7:使能状态|bit6:锁状态
 */
uint8_t log_State = 0x80;

/*@}*/

/**
 * @addtogroup console functions
 */

/*@{*/

/**
 * @brief 初始化console/log端口
 *
 * @param none
 * 
 * @retval none
 */
void mConsole_Init(void) {
  console_PortInit();
  
  RESET_SCREEN();
  printf("\033[1;40;32m\r\nConsole/Log port initialized.\r\n\r\n\033[0m");
}

/*@}*/

/**
 * @addtogroup log fiter functions
 */

/*@{*/

/**
 * @brief 设置log输出等级过滤
 *
 * @param level 过滤等级
 *  大于或等于level的能被输出
 * 
 * @retval none
 */
void mLog_SetFiter(Log_Level level) {
  log_LevelFiter = level;
}


/**
 * @brief 获取log输出等级过滤
 *
 * @param level 过滤等级
 * 
 * @retval none
 */
Log_Level mLog_GetFiter(void) {
  return log_LevelFiter;
}


/**
 * @brief 打开log输出等级
 *
 * @param level 等级
 * 
 * @retval none
 */
void mLog_TagEnable(Log_Level level) {
  log_LevelMask |= 0x01 << (uint8_t)level;
}


/**
 * @brief 关闭log输出等级
 *
 * @param level 等级
 * 
 * @retval none
 */
void mLog_TagDisable(Log_Level level) {
  log_LevelMask &= ~(0x01 << (uint8_t)level);
}


/**
 * @brief 控制log
 *
 * @param state 状态位
 * @param en 开关
 * 
 * @retval none
 */
void mLog_Control(uint8_t state, uint8_t en) {
  if (en >= 1) {
    log_State |= state;
  }
  else {
    log_State &= ~state;
  }
}

/*@}*/

/**
 * @addtogroup log utilities
 */

/*@{*/

/**
 * @brief 专用复制
 *
 * @param length 字符长度
 * @param dst 目标字符串
 * @param src 来源字符串
 * 
 * @retval 复制数量
 */
uint16_t logStrcpy(uint16_t length, char *dst, const char *src) {
  const char *src_old = src;

  while (*src != 0) {
    /* make sure destination has enough space */
    if (length++ <= OUTPUT_BUFFER_SIZE) {
      *dst++ = *src++;
    } 
    else {
      break;
    }
  }
  return src - src_old;
}

/*@}*/

/**
 * @addtogroup log output functions
 */

/*@{*/

/**
 * @brief 原始输出
 *
 * @param timeout 排队等待
 * @param format 字符
 * 
 * @retval none
 */
void mLog_RawPrntf(osTick_t timeout, const char *format, ...) {
  if (!(log_State & AD_LOG_CEN)) {
    return;
  }
  
  if (log_State & AD_LOG_LOCKER) {
    osThread_Delay(timeout);
    if (log_State & AD_LOG_LOCKER) {
      return;
    }
  }
  
  mLog_Control(AD_LOG_LOCKER, 0x01);
  
  va_list args;
  int32_t fmt_result;
  uint16_t stringLength = 0;
  
  va_start(args, format);
  fmt_result = vsnprintf(outputBuffer, OUTPUT_BUFFER_SIZE, format, args);
  va_end(args);

  if ((fmt_result > -1) && (fmt_result <= OUTPUT_BUFFER_SIZE)) {
    stringLength = fmt_result;
  } 
  else {
    stringLength = OUTPUT_BUFFER_SIZE;
  }
  
  console_PortOutput(outputBuffer, stringLength);
  
  mLog_Control(AD_LOG_LOCKER, 0x00);
}


/**
 * @brief Thread下的输出log
 *
 * @param level log等级
 * @param context 上下文
 * @param timeout 排队等待
 * @param format 字符
 * 
 * @retval none
 */
void mLog_ThreadPrintf(Log_Level level, const char *context, osTick_t timeout, const char *format, ...) {
  if (!(log_State & AD_LOG_CEN)) {
    return;
  }
  
  if (level > log_LevelFiter) {
    return;
  }
  else if (log_LevelMask & (0x01 << (uint8_t)level)) {
    return;
  }
  
  if (log_State & AD_LOG_LOCKER) {
    osThread_Delay(timeout);
    if (log_State & AD_LOG_LOCKER) {
      return;
    }
  }
  
  mLog_Control(AD_LOG_LOCKER, 0x01);
  
  va_list args;
  int32_t fmt_result;
  
  va_start(args, format);
  
  uint16_t stringLength = 0;
  
  /*复制osTick*/
  char osTisck_String[12];
  sprintf(osTisck_String, "[%d]", osSys_GetNowTick());
  stringLength += logStrcpy(stringLength, outputBuffer + stringLength, osTisck_String);
  
  /*复制tag*/
  stringLength += logStrcpy(stringLength, outputBuffer + stringLength, "[");
  stringLength += logStrcpy(stringLength, outputBuffer + stringLength, logLevelString[level]);
  stringLength += logStrcpy(stringLength, outputBuffer + stringLength, "]");
  
  /*复制上下文*/
  stringLength += logStrcpy(stringLength, outputBuffer + stringLength, "[");
  stringLength += logStrcpy(stringLength, outputBuffer + stringLength, context);
  stringLength += logStrcpy(stringLength, outputBuffer + stringLength, "]");
  
  /*复制空格*/
  stringLength += logStrcpy(stringLength, outputBuffer + stringLength, " ");
  
  /*复制自定义数据*/
  fmt_result = vsnprintf(outputBuffer + stringLength, OUTPUT_BUFFER_SIZE - stringLength - 1, format, args);
  
  va_end(args);
  
  /*复制新行*/
  stringLength += fmt_result;
//  if ((fmt_result > -1) && (fmt_result + stringLength + 2 <= OUTPUT_BUFFER_SIZE)) {
//    stringLength += fmt_result;
//    stringLength += logStrcpy(stringLength, outputBuffer + stringLength, CONSOLE_PRINT_NEWLINE);
//  } 
//  else {
//    stringLength = OUTPUT_BUFFER_SIZE;
//    strcpy(outputBuffer + OUTPUT_BUFFER_SIZE - 2, CONSOLE_PRINT_NEWLINE);
//  }
    
  /*输出数据*/
  console_PortOutput(outputBuffer, stringLength);
  
  mLog_Control(AD_LOG_LOCKER, 0x00);
}
/*@}*/
