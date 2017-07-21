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
 * File      : kprintf.h
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

#ifndef LOG_H_
#define LOG_H_

/**
 * @addtogroup std C lib include
 */

/*@{*/

#include <stdint.h>

/*@}*/

/**
 * @addtogroup OS file include
 */

/*@{*/

#include "../arch/platform.h"

/*@}*/

/**
 * @addtogroup Concole Configure
 */

/*@{*/

/**
 * 输出缓冲器大小
 * @note none
 */
#define OUTPUT_BUFFER_SIZE    (128)


/**
 * 输出结尾自动换行
 * @note none
 */
#define LOG_AUTO_NEWLINE      (1)

/*@}*/

/**
 * @addtogroup Concole Escape Sequence control
 */

/*@{*/

#define CONSOLE_NONE        "\033[0m"
#define CONSOLE_BLACK       "\033[0;30m"
#define CONSOLE_L_BLACK     "\033[1;30m"
#define CONSOLE_RED         "\033[0;31m"
#define CONSOLE_L_RED       "\033[1;31m"
#define CONSOLE_GREEN       "\033[0;32m"
#define CONSOLE_L_GREEN     "\033[1;32m"
#define CONSOLE_BROWN       "\033[0;33m"
#define CONSOLE_YELLOW      "\033[1;33m"
#define CONSOLE_BLUE        "\033[0;34m"
#define CONSOLE_L_BLUE      "\033[1;34m"
#define CONSOLE_PURPLE      "\033[0;35m"
#define CONSOLE_L_PURPLE    "\033[1;35m"
#define CONSOLE_CYAN        "\033[0;36m"
#define CONSOLE_L_CYAN      "\033[1;36m"
#define CONSOLE_GRAY        "\033[0;37m"
#define CONSOLE_WHITE       "\033[1;37m"
       
#define CONSOLE_BOLD        "\033[1m"
#define CONSOLE_UNDERLINE   "\033[4m"
#define CONSOLE_BLINK       "\033[5m"
#define CONSOLE_REVERSE     "\033[7m"
#define CONSOLE_HIDE        "\033[8m"
#define CONSOLE_CLEAR       "\033[2J"
#define CONSOLE_CUR_RESET   "\033[u"
#define CONSOLE_CLRLINE     "\r\033[K"

#define CONSOLE_PRINT_NEWLINE   "\r\n"

/*@}*/

/**
 * @addtogroup Concole priority attrs
 */

/*@{*/

/**
 * log输出优先级描述符
 * @note none
 */
typedef enum {
  Log_A = 0,
  Log_E = 1,
  Log_W = 2,
  Log_I = 3,
  Log_D = 4,
  log_V = 5
} Log_Level;

/*@}*/

/**
 * @addtogroup Log Control
 */

/*@{*/

/**
 * 使能/失能LOG整体输出
 * @note none
 */
#define AD_LOG_CEN      0x80

/**
 * LOG锁状态
 * @note 启动锁即其余线程无法输出LOG
 */
#define AD_LOG_LOCKER   0x40

/*@}*/

/**
 * @addtogroup Interface functions
 */

/*@{*/

extern void log_Init(void);

/*@}*/

/**
 * @addtogroup kernel printf functionss
 */

/*@{*/

extern void osLog_RawPrintf(osTick_t timeout, const char *format, ...);

/*@}*/

/**
 * @addtogroup log fiter functions
 */

/*@{*/

extern void osLog_SetFiter(Log_Level level);
extern Log_Level osLog_GetFiter(void);

extern void osLog_TagEnable(Log_Level level);
extern void osLog_TagDisable(Log_Level level);

extern void osLog_Control(uint8_t state, uint8_t en);

/*@}*/

/**
 * @addtogroup log functionss
 */

/*@{*/

#define CURSOR_MOVEUP(x) printf("\033[%dA", (x))

#define CURSOR_MOVEDOWN(x) printf("\033[%dB", (x))

#define CURSOR_MOVETO(x, y) printf("\033[%d;%dH", (x), (y))

#define RESET_SCREEN() printf("\033[u\033[2J")

extern void osLog_Printf(Log_Level level, const char *context, osTick_t timeout, const char *format, ...);

/*@}*/

/**
 * @addtogroup os info print
 */

/*@{*/

extern void osLog_InfoShow(void);

/*@}*/

#endif
