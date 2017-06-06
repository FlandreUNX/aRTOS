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
 * File      : osInfo.h
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

#ifndef OSINFO_H_
#define OSINFO_H_

#include "./osConfig.h"


/**
 * @addtogroup os信息记录
 */
  
/*@{*/

/**
 * CPU运行频率,单位Hz
 */
#define SYS_CLOCK       (216000000)

/**
 * MCU名称
 */
#define MCU_NAME        "STM32F767ZI"

/**
 * 使用LOGO
 */
#define USING_LOGO    (1)

/*@}*/

/**
 * @addtogroup logo
 */
  
/*@{*/

#if USING_LOGO == 1

static const char *osLogo[] = {
  "     ..-..                  ..-...\\ \r\n", \
  "    ./::::\\                 /::::::\\ \r\n", \
  "   /::::::::::::/\\__--_-==./::::::::\\ \r\n", \
  "  /..--:::^                 \\:::::::| \r\n", \
  "  /                          \\.::::\\ \r\n", \
  " /       .                       \\| \r\n", \
  " l      ...               .       l \r\n", \
  " |            (_/\\__)   ...      | \r\n", \
  " l                               l \r\n", \
  " \\                             / \r\n", \
  "   \\. ..__                   ./ \r\n", \
  "         /'''.--------T----\\ \r\n" \
};

#define OS_INFO  "'\033[1;35maRTOS\033[0m' A Real-time Operating System"

#define OS_AUTHOR  "2015 - 2017 Copyright by \033[0;33mFlandreUNX\033[0m"

#endif

/*@}*/

#endif
