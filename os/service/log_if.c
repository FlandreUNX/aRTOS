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
 * File      : kprintf_if.c
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

/**
 * @addtogroup Hal file include
 */

/*@{*/

#include "stm32f7xx.h"                  // Device header
#include "stm32f7xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

/*@}*/

/**
 * @addtogroup OS file include
 */

/*@{*/

/*@}*/

/**
 * @addtogroup std C lib include
 */

/*@{*/

#include <stdio.h>

/*@}*/

/**
 * @addtogroup hard Interface
 */

/*@{*/

/**
 *  Console uart handler
 */
UART_HandleTypeDef uartHandler;


/**
 * @brief 终端接口初始化
 *
 * @param none
 * 
 * @retval none
 */
void printPortInit(void) {
  RCC_PeriphCLKInitTypeDef periphClock;
  periphClock.PeriphClockSelection = RCC_PERIPHCLK_USART3;
  periphClock.Usart3ClockSelection = RCC_USART3CLKSOURCE_HSI;
  HAL_RCCEx_PeriphCLKConfig(&periphClock);
  __HAL_RCC_USART3_CLK_ENABLE();

  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitTypeDef gpio;
  gpio.Mode = GPIO_MODE_AF_PP;
//  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  gpio.Pin = GPIO_PIN_8;
  gpio.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOD, &gpio);

  gpio.Pin = GPIO_PIN_9;
  gpio.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOD, &gpio);

  uartHandler.Instance = USART3;
  uartHandler.Init.BaudRate = 115200;
  uartHandler.Init.WordLength = UART_WORDLENGTH_8B;
  uartHandler.Init.StopBits = UART_STOPBITS_1;
  uartHandler.Init.Parity = UART_PARITY_NONE;
  uartHandler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartHandler.Init.Mode = UART_MODE_TX_RX;
  uartHandler.Init.OverSampling = UART_OVERSAMPLING_8;

  if (HAL_UART_Init(&uartHandler) != HAL_OK){
    for (;;);
  }
}


/**
 * @brief 标准C接口重定义
 *
 * @param none
 * 
 * @retval none
 */
int fputc(int ch, FILE *f) {
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&uartHandler, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/*@}*/

/**
 * @addtogroup implementation Interface
 */

/*@{*/

void printPortOutput(uint8_t *log, uint16_t size) {
  HAL_UART_Transmit(&uartHandler, log, size, 0xFFFFFFFF);
}

/*@}*/
