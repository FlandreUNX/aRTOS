aRTOS
===
'aRTOS' A Real-time Operating System <br>
2015 - 2017 Copyright by FlandreUNX <br>
<br>
# 性能<br>
## Memory<br>
*   采用buddy内存管理模型,可自由配置的Buddy管理范围
## Thread<br>
*   线程描述符编译占用RW-Data,堆栈属于内存管理器Heap动态分配
## Timer<br>
*   可选配的os内部时钟依赖型定时器
*   可选配的线程依赖型定时器
*   可传递参数的超时回调函数
## schedule<br>
*   最大255级的优先级调度器
*   同等优先级无限制线程数
*   抢占式+时间片调度器,bitmap算法调度算法
*   可临界屏蔽的调度器管理
*   调度器实现的IRQ托管
## lib<br>
*   支持内核符号导出(symbol_expert)
*   内置环形缓冲器(FIFO_1Byte)
*   内置基于linux的双向链表(list_t)
## pm<br>
*   基于事件实现的线程间通讯
*   支持signal事件
*   more...
## modules<br>
*   可裁剪的模块模型
*   基于后台线程 + Callback的key模块
*   基于usart的彩色console + log输出模块
*   more...
