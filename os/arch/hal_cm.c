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
 * File      : hal_cm.c
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

#include "./hal_cm.h"

/**
 * @addtogroup OS Include
 */

/*@{*/

#include "../kernel/schedule.h"

/*@}*/

/**
 * @addtogroup os schedule extern
 */
 
/*@{*/

extern volatile struct threadSwitchInfo_t sche_ThreadSwitchStatus;

/*@}*/

/**
 * @addtogroup cortex-m hal functions
 */
 
/*@{*/

/**
 * cpu寄存器初始化
 *
 * @param func thread函数
 * @param arguments thread传递参数
 * @param stackTop thread堆栈起点
 * 
 * @return 修改后的堆栈指针
 */
uint32_t* cpu_SetupRegisters(void *func, void *arguments, uint32_t *stackTop) {
  cpuRegisters_t *regs = (cpuRegisters_t *)((uint32_t)stackTop - sizeof(cpuRegisters_t));

  regs->r13_SP = (uint32_t)0x01000000;
  regs->r15_PC = (uint32_t)func;
  regs->r14_LR = (uint32_t)0;

  regs->r12 = (uint32_t)0x00000012;
  regs->r3 = (uint32_t)0x00000003;
  regs->r2 = (uint32_t)0x00000002;
  regs->r1 = (uint32_t)0x00000001;
  regs->r0 = (uint32_t)arguments;

  regs->exec = (uint32_t)0xFFFFFFFD;

  regs->r11 = (uint32_t)0x00000011;
  regs->r10 = (uint32_t)0x00000010;
  regs->r9 = (uint32_t)0x00000009;
  regs->r8 = (uint32_t)0x00000008;
  regs->r7 = (uint32_t)0x00000007;
  regs->r6 = (uint32_t)0x00000006;
  regs->r5 = (uint32_t)0x00000005;
  regs->r4 = (uint32_t)0x00000004;
  return (uint32_t *)regs;
}


/**
 *  KEIL CMSIS平台
 *  @note 使用keil目标平台的starup_文件和编译器特性
 */
#ifdef __CC_ARM 

/**
 *  Cortext-M NVIC register
 */
#define NVIC_INT_CTRL 0xE000ED04    /**< Interrupt control state register */
#define NVIC_SYSPRI14 0xE000ED22    /**< System priority register (priority 14) */
#define NVIC_PENDSVSET 0x10000000   /**< Value to trigger PendSV exception */
#define NVIC_PENDSV_PRI 0xFF    /**< PendSV priority value (lowest) */


/**
  * 标记pensv中断
  *
  * @param none
  * 
  * @return none
  */
__asm void hal_IRQConfigure(void) {
  PRESERVE8
  
  /*设置pendSV为最低优先级*/
  ldr r0, =NVIC_SYSPRI14
  ldr r1, =NVIC_PENDSV_PRI
  strb r1, [r0]
}

/**
  * 标记pensv中断
  *
  * @param none
  * 
  * @return none
  */
__asm void hal_CallPendSV(void) {
  PRESERVE8
  
  ldr r0, =NVIC_INT_CTRL
  ldr r1, =NVIC_PENDSVSET
  strb r1, [r0]
}


/**
  * 标记NMI中断
  *
  * @param none
  * 
  * @return none
  */
__asm void hal_CallNMI(void) {
  
}


/**
  * 关闭全局中断
  *
  * @param none
  * 
  * @return none
  */
__asm uint32_t hal_DisableINT(void) {
  PRESERVE8

  mrs r0, basepri
  mov r1, #MAX_SYSCALL_INTERRUPT_PRIORITY
  msr basepri, r1

  dsb
  isb

  bx r14
}


/**
  * 开启全局中断
  *
  * @param none
  * 
  * @return none
  */
__asm void hal_EnableINT(uint32_t level) {
  PRESERVE8
  
  msr basepri, r0
  bx r14
}


/**
  * 进入第一个任务
  *
  * @param none
  * 
  * @return none
  */
__asm void cpu_GotoFisrtTask(void) {
  /*SCB->VTOR 0xE000ED08*/
  /*保存指针*/
  ldr r0, =0xE000ED08

  /*保存指针所指的值*/
  ldr r0, [r0]						
  ldr r0, [r0]						

  isb

  /*将SP指针指回__initial_sp*/
  msr msp, r0						
  
  dsb							   
  isb								
  
  /*开放中断*/
  mrs r0, PRIMASK					
  cpsie 	i

  svc 0
  
  align 4
}


/**
  * SVC
  *
  * @param none
  * 
  * @return none
  */
__asm void SVC_Handler(void) {
  extern sche_ThreadSwitchStatus;
  
  PRESERVE8
  
  /*最近任务TUB地址写进R3*/
  ldr r3, =__cpp(&sche_ThreadSwitchStatus.nowThread)
  
  /*将TUB的栈中的SP写进R1*/
  ldr r1, [r3]			
  
  /*恢复SP*/
  ldr r0, [r1]		
  
  /*恢复R4-R11, R14*/
  ldmia r0!, {r4 - r11, r14} 
  
  isb
  
  /*恢复PSP*/
  msr psp, r0
      
  isb
      
  /*恢复中断*/
  mov r0, #0
  msr basepri, r0	
      
  /*返回任务*/
  bx 	r14
      
  align 4					
}


/**
  * PendSV
  *
  * @param none
  * 
  * @return none
  */
__asm void PendSV_Handler(void) {
  extern sche_ThreadSwitchStatus;
  extern sche_ThreadSwitchStatus;
  
  extern sche_NextToNow;
  
  PRESERVE8	
  
  /*关中断*/
  mrs r1, PRIMASK								
  cpsid i		
  isb
  
  mrs r0, psp
  
  ldr r3, =__cpp(&sche_ThreadSwitchStatus.nowThread)						
  ldr r2, [r3]	
      
  stmdb r0!, {r4 - r11, r14}					
  str    r0, [r2]									
  stmdb sp!, {r3}										

  /*仅消耗4条ASM指令,不修改*/
  bl __cpp(sche_NextToNow)										

  ldmia sp!, {r3}								
  ldr r1, [r3]									
  ldr r0, [r1]								
  ldmia r0!, {r4 - r11, r14}				

  msr psp, r0					
  
  /*恢复中断*/
  msr    PRIMASK, r1;								
  isb											

  bx r14									
      
  align 4										
}


/**
  * Systick
  *
  * @param none
  * 
  * @return none
  */	
void SysTick_Handler(void) {
  sys_TickHandler();
}

/*@}*/

#endif
