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

extern struct threadSwitchInfo_t sche_ThreadSwitchStatus;

/*@}*/

/**
 * @addtogroup cortex-m hal functions
 */
 
/*@{*/

/**
 * 线程退出处理
 *
 * @param none
 * 
 * @retval none
 */
static void taskExitError(void) {
	hal_DisableINT();
  
	for(;;);
}


/**
 * cpu寄存器初始化
 *
 * @param func thread函数
 * @param arguments thread传递参数
 * @param stackTop thread堆栈起点
 * 
 * @retval 修改后的堆栈指针
 */
uint32_t* cpu_SetupRegisters(void *func, void *arguments, uint32_t *stackTop) {
  /* Simulate the stack frame as it would be created by a context switch
	interrupt. */

	/* Offset added to account for the way the MCU uses the stack on entry/exit
	of interrupts, and to ensure alignment. */
	stackTop -= 1;

	*stackTop = 0x01000000;	/* xPSR */
	stackTop--;
	*stackTop = ((uint32_t)func) & ((uint32_t)0xfffffffeUL);	/* PC */
	stackTop--;
	*stackTop = (uint32_t)taskExitError;	/* LR */

	/* Save code space by skipping register initialisation. */
	stackTop -= 5;	/* R12, R3, R2 and R1. */
	*stackTop = (uint32_t)arguments;	/* R0 */

	/* A save method is being used that requires each task to maintain its
	own exec return value. */
	stackTop--;
	*stackTop = 0xFFFFFFFD;

	stackTop -= 8;	/* R11, R10, R9, R8, R7, R6, R5 and R4. */
  
//  /*xPSR*/
//	*(--stackTop) = (uint32_t)0x01000000;
//	/*PC*/
//	*(--stackTop) = (uint32_t)func;
//	/*LR(R14)*/
//	*(--stackTop) = (uint32_t)0;
//	/*R12*/
//	*(--stackTop) = (uint32_t)0x00000012;
//	/*R3*/
//	*(--stackTop) = (uint32_t)0x00000003;
//	/*R2*/
//	*(--stackTop) = (uint32_t)0x00000002;
//	/*R1*/
//	*(--stackTop) = (uint32_t)0x00000001;
//	/*R0*/
//	*(--stackTop) = (uint32_t)arguments;
//	/*exec 返回 线程,PSP模式*/
//	*(--stackTop) = (uint32_t)0xFFFFFFFD;
//	/*R11*/
//	*(--stackTop) = (uint32_t)0x00000011;
//	/*R10*/
//	*(--stackTop) = (uint32_t)0x00000010;
//	/*R9*/
//	*(--stackTop) = (uint32_t)0x00000009;
//	/*R8*/
//	*(--stackTop) = (uint32_t)0x00000008;
//	/*R7*/
//	*(--stackTop) = (uint32_t)0x00000007;
//	/*R6*/
//	*(--stackTop) = (uint32_t)0x00000006;
//	/*R5*/
//	*(--stackTop) = (uint32_t)0x00000005;
//	/*R4*/
//	*(--stackTop) = (uint32_t)0x00000004;

	return stackTop;
}


/**
 *  KEIL CMSIS平台
 *  @note 使用keil目标平台的starup_文件和编译器特性
 */
#ifdef __CC_ARM

/**
 *  检查是否特权模式
 *
 *  @note Determine whether we are in thread mode or handler mode.
 *
 *  @param  none
 * 
 *  @retval 状态
 */
int32_t hal_IsHandlerMode(void) {
  return __get_IPSR() != 0;
}

/**
 *  启动FPU
 *
 *  @param none
 * 
 *  @retval none
 */
__asm void hal_EnableVFP( void ) {
	PRESERVE8

	/* The FPU enable bits are in the CPACR. */
	ldr.w r0, =0xE000ED88
	ldr	r1, [r0]

	/* Enable CP10 and CP11 coprocessors, then save back. */
	orr	r1, r1, #(0xf << 20)
	str r1, [r0]
	bx	r14
	nop
}


/**
 *  关闭全局中断
 *
 *  @param none
 * 
 *  @retval none
 */
void hal_RaiseBASEPRI(void) {
  uint32_t ulNewBASEPRI = MAX_SYSCALL_INTERRUPT_PRIORITY;
  
  __asm {
    cpsid i
		msr basepri, ulNewBASEPRI
		dsb
		isb
		cpsie i
  }
}


/**
 *  开启全局中断
 *
 *  @param  none
 * 
 *  @retval none
 */
void hal_SetBASEPRI(uint32_t value) {
  __asm {
    msr basepri, value
  }
}


/**
 *  进入第一个任务
 *
 *  @param  none
 * 
 *  @retval none
 */
__asm void cpu_GotoFisrtTask(void) {
  /*SCB->VTOR 0xE000ED08*/
  /*保存指针*/
  ldr r0, =0xE000ED08

  /*保存指针所指的值*/
  ldr r0, [r0]
  ldr r0, [r0]

  /*将SP指针指回__initial_sp*/
  msr msp, r0
  cpsie i
	cpsie f
	dsb
	isb
  
  svc 0
  
  align 4
}


/**
 *  SVC
 *
 *  @param none
 * 
 *  @retval none
 */
__asm void SVC_Handler(void) {
  extern sche_ThreadSwitchStatus;
  
  PRESERVE8
  
  /*关闭中断*/
  mrs r0, basepri
  mov r1, #MAX_SYSCALL_INTERRUPT_PRIORITY
  msr basepri, r1

  dsb
  isb
  
  /*最近线程地址写进R3*/
  ldr r3, =__cpp(&sche_ThreadSwitchStatus.nextThread)
  
  /*将线程的栈中的SP写进R1*/
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
 *  PendSV
 *
 *  @param none
 * 
 *  @retval none
 */
__asm void PendSV_Handler(void) {
  extern sche_ThreadSwitchStatus;
  extern sche_ThreadSwitchStatus;
  
  extern sche_NextToNow;
  
  PRESERVE8
  
  mrs r0, psp
  isb
  
  ldr r3, =__cpp(&sche_ThreadSwitchStatus.nowThread)
  ldr r2, [r3]
  
  /*检查是否开启FPU,开启就存档*/
  tst r14, #0x10
	it eq
	vstmdbeq r0!, {s16 - s31}
      
  stmdb r0!, {r4 - r11, r14}

  str r0, [r2]
  stmdb sp!, {r3}

  mov r0, #MAX_SYSCALL_INTERRUPT_PRIORITY
	cpsid i
	msr basepri, r0
	dsb
	isb
	cpsie i 

  /*仅消耗4条ASM指令,不修改*/
  bl __cpp(sche_NextToNow)

  mov r0, #0
  msr basepri, r0

  ldmia sp!, {r3}
  
  ldr r1, [r3]
  ldr r0, [r1]
  
  ldmia r0!, {r4 - r11, r14}

  tst r14, #0x10
	it eq
	vldmiaeq r0!, {s16-s31}

  msr psp, r0
  isb

  bx r14
      
  align 4
}


/**
 *  Systick
 *
 *  @param none
 * 
 *  @retval none
 */	
void SysTick_Handler(void) {
  sys_TickHandler();
}

/*@}*/

#endif
