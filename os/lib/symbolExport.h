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
 * File      : symbolExport.h
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

#ifndef SYMBOLEXPORT_H_
#define SYMBOLEXPORT_H_

#ifdef USING_SYMBOL_EXPORT

    /**
    * @addtogroup 导出符号类型定义
    */

    /*@{*/

    typedef struct symbolTab {
        void *address;
        const char *name;
    } symbolTab_t;

    /*@}*/

    /**
    * @addtogroup 导出符定义
    */

    /*@{*/

    /**
    * 内核导出符
    *
    * @param symbol 需要导出的函数
    * 
    * @return none
    */
    #define EXPORT_SYMBOL(symbol) \
        const char export_##symbol##_name[] OS_SECTION(".rodata.name") = #symbol; \    /**< 导出符号名称 */
        const symbolTab_t export_##symbol OS_SECTION("kernelSymbol") = { \             /**< 导出符号主体 */
            (void *)&symbol, \
            export_##symbol##_name \
        };

    /*@}*/

#elif

    #define EXPORT_SYMBOL(symbol)

#endif

#endif
