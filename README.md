ACGrtos
===
新一轮的RTOS,集中设计数据结构以及模块加载<br>
<br>
# 性能<br>
## Memory<br>
*   采用buddy内存管理模型,可配置管理范围
## Thread<br>
*   线程描述符编译占用RW-Data,堆栈属于内存管理器Heap动态分配
## Timer<br>
*   可选配硬件依赖定时器(依赖os内部时钟)
*   可选配软件依赖定时器(依赖线程模型)
*   可传递参数的超时回调函数
## schedule<br>
*   抢占式+时间片调度器,bitmap算法查询线程
## lib<br>
*   支持内核符号导出
*   内置环形缓冲器
*   内置基于linux的双向链表
