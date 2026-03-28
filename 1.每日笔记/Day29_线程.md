## Day29——线程

### 相关函数

1. **pthread_create**——创建一个线程

   ```c
   #include <pthread.h>
   int pthread_create(
       pthread_t *thread,               // 不同线程独立唯一的线程id 
       const pthread_attr_t *attr,      // 线程的属性 (attr属性),  暂时使用NULL填充
       void *(*start_routine) (void *), // 线程的启动函数
       void *arg                        // 线程的传参 
   );
   // 返回值： 0成功， 其它失败可暂时忽略or参见1.3.2错误处理
   ```

2. **pthread_join**——**捕获**线程的**退出**和**退出状态**，且等待线程结束并回收资源

   ```c
   #include <pthread.h>
   // join with a terminated thread
   int pthread_join( 
   	pthread_t thread, // 要捕获线程目标的线程id
   	void **retval // 目标线程的退出状态(指向线程的退出时携带参数)
   );
   // 返回值int标识捕获状态:  0代表成功 (别的仅作了解: 死锁,不可捕获,被别的线程捕获, 不识别的线程id)
   ```

3. **pthread_exit——主动退出线程**

   ```c
   #include <pthread.h>
   // terminate calling thread
   void pthread_exit( 
   	void *retval //该参数描述了线程的退出状态，即它指向的数据将作为线程退出时的返回值。如果线程不需要返回任何数据，将retval参数置为NULL即可。)
   );
   ```

4. **pthread_cancel——取消一个(拥有取消点的)执行中的线程**

   ```c
   #include <pthread.h>
   // send a cancellation request to a thread
   int pthread_cancel(
   	pthread_t thread, // 不同线程的线程id
   );
   // 返回值int标识捕获状态: 0代表成功， ESRCH代表参数错误(没找到这个线程id)
   ```

5. **pthread_cleanup_push& pthread_cleanup_pop——通过向这个清理栈中添加清理函数，以及pop出清理函数执行，从而确保线程的正确终止和资源回收**

   当线程开始执行时，会创建一个清理栈（thread's stack of thread-cancellation），用于存储注册的清理函数和参

   数。

   ```c
   #include <pthread.h>
   // push thread cancellation clean-up handlers
   void pthread_cleanup_push(
   	void (*routine)(void *),// 指向清理函数的指针，该函数将在线程取消时被调用
   	void *arg // 传递给清理函数的参数
   );
   
   #include <pthread.h>
   // pop thread cancellation clean-up handlers
   void pthread_cleanup_pop(
   	int execute // 设置参数0代表弹出栈顶函数并且'不执行'这个函数， 非0代表代表弹出栈顶函数并且'执行'这个函数
   );
   ```

### 笔记

1. **子线程在启动函数返回后就消亡了吗？**

   子线程的**入口函数（启动函数）执行完成并返回后，线程就会自动终止、消亡**，操作系统会回收线程的内核资源。

2. **两个线程访问进程中的全局变量时会出现覆盖污染数据的问题**

   - 因为两个线程同时读写同一个全局变量，操作不是原子的，会互相覆盖，导致结果错乱。

   - 多个线程同时读写共享变量，操作不是原子的，会发生竞争，导致数据不一致。

   - **一行代码 ≠ 一条 CPU 指令**，比如 `cnt++`，CPU 实际要做三步：

     - 读：从内存读到寄存器

     - 改：寄存器里 +1

     - 写：写回内存

3. 一行 C/C++ 代码，几乎**永远都不是原子操作**

   - 原子操作 = **CPU 一条指令就能做完，中途不会被线程切换打断**。

   - 只要需要 **多条 CPU 指令**，就**不是原子**，多线程下就会乱。

   - 哪些常见行代码都不是原子？

     ```c
     a = b;
     a++;
     a += 1;
     a = a + 1;
     flag = !flag;
     ```

   - 那什么才可能是原子？
     只有**单条 CPU 指令**的操作才可能原子，比如：

     - 简单赋值：`a = 1;`（某些平台是原子）
     - 简单读取：`x = a;`（某些平台是原子）

   - **一行高级语言代码 ≠ 一条指令**
     多线程下，只要不是明确的原子操作，都不安全。