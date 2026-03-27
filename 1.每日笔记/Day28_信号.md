## Day28——信号

### 相关函数

1. **signal**

   用来捕获信号并且指定对应的信号处理行为

   ```c
   #include <signal.h>
   // 定义信号处理函数的类型: int类型的参数(信号编号), void返回值
   typedef void (*sighandler_t)(int);
   
   sighandler_t signal(
       int signum, // 要处理的信号编号(Eg:SIGINT、SIGTERM...)(Eg:2,15...)
       sighandler_t handler // 指向信号处理函数(回调机制): 如上面sighandler_t定义, (另外:SIG_IGN表示忽略信号; SIG_DFL表示恢复信号的默认行为)
   );
   // 返回值: 成功返回关联的指定信号的处理函数的指针; 失败返回SIG_ERR
   ```

2. **sigaction**

   在**signal**的基础上更精确控制信号处理行为              						<a id="sigaction back"></a>[跳转到结构体struct sigaction](#sigaction)

   ```c
   #include <signal.h>
   // examine and change a signal action
   int sigaction(
       int signum,                  // 要操作的信号编号(除了不能捕获的SIGKILL和SIGSTOP)
       const struct sigaction *act, // 指定信号的新处理动作(如果非空)
       struct sigaction *oldact     // 获取信号的上一个处理动作(如果非空)
   );
   // 返回值: 成功时返回0，错误时返回-1
   ```

3. **操作sigset_t类型变量(即struct sigaction中的sa_mask成员)的一系列函数**

   ```c
   sigemptyset(sigset_t *set)：// 初始化信号集，清除所有信号。
   sigfillset(sigset_t *set)：// 添加所有信号到信号集中。
   sigaddset(sigset_t *set, int signo)：// 向信号集添加一个信号。
   sigdelset(sigset_t *set, int signo)：// 从信号集中删除一个信号。
   sigismember(const sigset_t *set, int signo)：// 检查一个特定信号是否在信号集中
   ```

4. **sigpending**

   用于检查当前进程的未决信号集，即那些已经发送给进程但由于某种原因（通常是因为被阻塞）尚未被处理的信号。这个函数可以用来确定哪些信号已经被产生并等待处理，但尚未被当前进程捕获或忽略。

   ```c
   #include <signal.h>
   // examine pending signals
   int sigpending(
       sigset_t *set // 接收当前进程的未决信号集
   );
   // 返回值: 成功时返回0，失败返回-1
   ```

5. **sigprocmask**

   用于在系统中检查和更改进程的信号屏蔽字（即信号掩码mask）。信号掩码确定了哪些信号可以递送给该进程，哪些信号被阻塞。和前面通过sigaction函数设置的sa_mask阻塞机制不同的是, sigaction函数设置的**sa_mask**阻塞是**临时屏蔽**。而**sigprocmask**函数修改信号掩码mask是**全程屏蔽**. (man sigprocmask)

   ```c
   #include <signal.h>
   // examine and change blocked signals
   int sigprocmask(
       int how, // 如何修改信号掩码.SIG_BLOCK:把set内信号添加阻塞;SIG_UNBLOCK:解除set内信号阻塞; SIG_SETMASK:将信号掩码替换为set指定信号
       const sigset_t *set, // 信号集合
       sigset_t *oldset // 当前信号掩码
   );
   // 返回值: 成功返回0, 失败返回-1
   ```

6. **kill**

   用来给另一个进程**发送信号**

   ```c
   #include <sys/types.h>
   #include <signal.h>
   // send signal to a process
   int kill(
       pid_t pid, //表示进程ID (另外  0:发送信号到与发送进程相同进程组的所有进程;  -1:表示所有可以发送信号的进程发送信号; 小于-1:则根据其绝对值去关闭其作为组长的进程组)
       int sig // 信号数值
   );
   // 返回值: 成功0, 失败-1
   ```

7. **pause**

   使调用进程挂起（即暂停执行）（==**注意：挂起完全不等于阻塞**==），直到该进程捕获到一个信号。换句话说，**pause函数**让进程休眠，等待任何类型的信号到来；一旦接收到信号，如果有为该信号定义的处理函数，则执行该函数。如果没有为信号定义处理函数（或者信号的行为是默认的），进程会根据信号的默认行为来响应。

   ```c
   #include <unistd.h>
   // wait for signal
   int pause(void);
   ```

8. **sigsuspend**

   <span style=color:red;background:yellow;font-size:16px>**原子地**</span>更改进程的信号屏蔽字（block mask）并挂起进程执行，直到捕获到一个信号

   - 更改信号掩码和挂起进程之间提供原子操作
   - 在sigsuspend返回后，**进程的信号掩码会自动恢复到调用sigsuspend之前的状态（即mask会回退）**

   ```c
   #include <signal.h>
   // wait for a signal
   int sigsuspend(
       const sigset_t *mask // 指定了在挂起期间要设置的新信号掩码
   );
   ```

9. **alarm**

   用于设置一个计时器（定时器），该计时器在指定的秒数后到期。当计时器到期时，内核会向该进程发送 `SIGALRM` 信号。如果程序没有捕获或忽略该信号，则其默认行为是终止进程

   ```c
   #include <unistd.h>
   // set an alarm clock for delivery of a signal
   unsigned int alarm(unsigned int seconds);
   ```

10. **setitimer**													                              		   <a id="itimerval back"></a> [跳转到结构体struct itimerval](#itimerval)

    一个高级定时器接口，相较于**alarm函数**，它提供了更多的灵活性和精度

    ```c
    #include <sys/time.h>
    // set value of an interval timer
    int setitimer(
        int which, // 定时器的类型
        const struct itimerval *new_value, // 指定的新的定时器值
        struct itimerval *old_value // 存储定时器的前一个值
    );
    // 返回值: 成功0, 失败-1
    ```

11. **getitimer**                                                                                                                                                            [跳转到结构体struct itimerval](#itimerval)

    用来**获取当前进程的间隔定时器状态**：**查看当前定时器还剩多少时间 + 是否在循环触发**

    ```c
    #include <sys/time.h>
    // get value of an interval timer
    int getitimer(
        int which, // 定时器的类型
        struct itimerval *curr_value // 
    );
    ```

### 结构体

1. **struct sigaction**										        																		<a id="sigaction"></a>[返回sigaction](#sigaction back)

   ```c
   // 是用于定义信号处理的行为
   struct sigaction 
   {
       void     (*sa_handler)(int);// 函数指针:指向一个信号处理函数 (和sa_sigaction选一个即可)
       void     (*sa_sigaction)(int, siginfo_t *, void *);// 函数指针:指向一个接受三个参数的信号处理函数
       sigset_t   sa_mask;// 信号集: 指定当前信号处理函数执行时需要阻塞的额外信号
       int        sa_flags;// 指定信号处理的选项和标志: 
       void     (*sa_restorer)(void);// 过时,暂无用
   };
   ```

   ```c
   siginfo_t 
   {
       //...
       pid_t    si_pid;   /* Sending process ID */
       sigval_t si_value; /* Signal value */
        // ......
   }
   ```

2. **typedef struct sigset_t(即struct sigaction中的sa_mask成员)**

   ==**未决信号集（pending signal set）是内核维护的，用户进程无法直接修改。**==用户只能通过调用系统调用或者库函数来间接获取它的状态或者间接修改它

   ```c
   typedef struct {
   	unsigned long sig[_NSIG_WORDS];
   } sigset_t;
   ```

3. **struct timeval**

   ```c
   struct timeval
   {
       long tv_sec;  /* seconds */
       long tv_usec; /* microseconds */
   };
   ```

4. **struct itimerval**     															                                                      <a id="itimerval"></a>[返回setitimer](#itimerval back)

   ```c
   struct itimerval 
   {
       struct timeval it_interval;// 间隔时间: 字段被设置为非零值，定时器将变为周期性的
       struct timeval it_value; // 定时器的剩余时间
       // 当定时器的it_value达到0并触发信号后，it_value会被重新设置为 it_interval 的值，然后定时器再次开始计时
   };
   ```

5. **五个重要的信号**

   |   Signal    |   value    | Action |                    Comment                    |
   | :---------: | :--------: | :----: | :-------------------------------------------: |
   | **SIGINT**  |     2      |  Term  |   键盘中断(Ctrl+C触发) (默认行为:终止进程)    |
   | **SIGQUIT** |     3      |  Core  |   键盘退出(Ctrl+\触发) (默认行为:终止进程)    |
   | **SIGKILL** |     9      |  Term  |                   终止进程                    |
   | **SIGCONT** | 19, 18, 25 |  Cont  |                暂停后恢复运行                 |
   | **SIGSTOP** | 17, 19, 23 |  Stop  | 暂停进程(可通过Ctrl+Z触发)(SIGCONT或者fg恢复) |

6. **信号递送过程中，产生了另一个同类信号会怎么样？产生了另两个、三个、多个同类信号会怎么样？写代码验证一下你的想法。说明mask和pending如何变化。**

   ```c
   #include <my_header.h>
   int count = 0;
   void fun(int sig_val){
       if(count == 0){
           printf("before first sleep\n");
           sleep(10);
           printf("after first sleep\n");
       }
       count++;
       printf("signal of %d appeared counts = %d\n",sig_val, count );
   }
   int main(int argc, char* argv[]){                  
       signal(SIGINT, fun);
       printf("before while, all handler over\n");
       while(1);
       printf("after while\n");
       return 0;
   }
   ```

   ```
   1.初始状态：
       mask：空（没有被阻塞的信号）
       pending：空
   2.第一次发送^C（SIGINT）：
       内核将 SIGINT 加入pending，然后检查mask，发现 SIGINT 未被阻塞，于是进入 handler 函数fun。
       进入 handler 后，内核自动将SIGINT 加入当前进程的mask（阻塞同类型信号，避免递归调用）。
       mask：{SIGINT}
       pending：空（已开始处理）
   3.handler 执行期间（sleep (10) 时）发送多次^C：
       每次发送 SIGINT，内核都会尝试将其加入pending，但因为同类型信号已存在于pending中，只会保留 1 个，不会累积。
       mask：{SIGINT}（保持不变，仍阻塞 SIGINT）
       pending：{SIGINT}（最多 1 个）
   4.handler 执行完毕（sleep 结束，count 自增后）：
       内核恢复mask（移除 SIGINT 阻塞），然后检查pending集合。
       发现pending中有 SIGINT，于是再次触发 handler 函数，重复上述过程。
   5.第二次 handler 执行完毕后：
       若没有新的 SIGINT 发送，pending为空，进程回到 while (1) 循环等待。
       若再次发送^C，则重复触发流程
   ```

7. ==**被中断的信号并不处于pending(待处理)状态**==

   - **不同类型信号可以嵌套中断**
   - **相同类型信号不会中断当前处理，且最多只保留一个 pending**
   - 嵌套深度可以超过 2 层（A → B → A 这种是允许的）
   - 最终执行次数取决于信号到达的**时序**和当前正在处理的信号类型

   ```c
   假设：
   信号 A 和 信号 B 是两种不同类型的信号
   到达顺序：A, B, A, A（依次到达）
   进程正在执行信号 A 的处理函数时，后续信号陆续到达
   
   第 1 步：
   信号 A 到达
   进程开始执行信号 A 的处理函数
   在 A 的处理函数执行期间，同类型信号 A 会被自动阻塞（即不会因为新来的 A 而中断当前处理）
   信号 B 默认不会被阻塞（除非用 sigprocmask 显式阻塞）
   
   第 2 步：
   信号 B 到达（在 A 的处理过程中）
   根据规则 1：不同类型信号可以中断当前处理
   所以 A 的处理函数被暂停
   CPU 开始执行信号 B 的处理函数
   此时状态：
   A 处理函数：暂停中（未完成）
   B 处理函数：正在执行
   
   第 3 步：
   信号 A 到达（在 B 的处理过程中）
   此时进程正在执行信号 B 的处理函数
   来的信号是 A，与 B 不同类型
   根据规则 1：A 可以中断 B
   所以 B 的处理函数被暂停
   CPU 开始执行信号 A 的处理函数
   此时状态：
   第一个 A 处理函数：暂停中（更早被 B 中断）
   B 处理函数：暂停中
   第二个 A 处理函数：正在执行
   
   第 4 步：
   信号 A 到达（在第二个 A 的处理过程中）
   现在正在执行的是第二个 A 的处理函数
   来的信号是 A，与当前处理的信号同类型
   根据规则 2 和 3：
   同类型信号不会中断当前处理
   由于已经有一个 A 在待处理状态吗？这里需要小心
   实际上，在第二个 A 的处理函数执行期间：
   第一个 A 的处理函数处于暂停状态，但它并不是 pending 状态，它只是被中断了，还在内核的调用栈里
   当前正在处理的是第二个 A
   新来的第三个 A，因为同类型信号正在被处理（且该信号被自动阻塞），所以：
   如果此时没有其他 A 处于 pending 状态，则这个新 A 会被标记为 pending
   如果已经有 A 处于 pending 状态，则这个新 A 会被丢弃
   那么此时有没有 A 处于 pending？
   仔细看：第一个 A 被 B 中断了，它不是 pending，它只是被暂停在栈里。第二个 A 正在执行，它也不是 pending。
   所以目前没有 A 处于 pending 状态。
   因此，第三个 A 到达时：
   不会中断当前处理
   被标记为 pending（待处理）
   
   第 5 步：
   第二个 A 处理函数执行完毕
   从第二个 A 的处理函数返回
   内核检查 pending 信号：
   发现有一个 A 处于 pending 状态
   但注意：此时调用栈中还有第一个 A（被 B 中断）和 B（被第二个 A 中断）
   
   第 6 步：
   处理 pending 的 A
   由于有一个 pending 的 A，内核会执行这个 A 的处理函数
   这是第三个 A（实际上执行的是第三次 A 处理函数）
   此时状态：
   第一个 A：仍在栈中（被 B 中断）
   B：在栈中（被第二个 A 中断）
   第三个 A：正在执行
   
   第 7 步：
   第三个 A 执行完毕
   返回到被它中断的 B
   B 继续执行直至完毕
   
   第 8 步：
   B 执行完毕
   返回到被 B 中断的第一个 A
   第一个 A 继续执行直至完毕
   
   最终执行次数：
   信号 A：执行了 3 次
   第 1 次：最开始到达
   第 2 次：在 B 处理过程中到达并中断了 B
   第 3 次：作为第 4 步到达的那个 A 的 pending 处理
   信号 B：执行了 1 次
   只在第 2 步到达时执行了一次，之后没有被再次触发
   ```

8. 注册一个信号处理函数，并且**处理完毕一个信号之后，不需要重新注册**，就能够捕捉下一个信号。

   在 **Linux 和现代 UNIX 系统**上：在 `main` 函数开头用 `signal` **注册一次**，程序运行期间（只要不主动改变），该信号处理函数会**一直有效**，任何时候信号到达都会被捕获。

   ```c
   void handler(int signo)
   {
       printf("捕获到信号\n");
       // 不需要重新注册，下次信号来还会调用这个函数
   }
   
   int main()
   {
       signal(SIGINT, handler);  // 注册一次即可
       while(1);
   }
   ```

9. **什么是注册信号**

   - **注册信号 = 注册一个信号处理函数**
   - "注册信号"本质上是一个**面向操作系统的概念**，意思是：**告诉内核，当某个信号发生时，不要执行默认动作，而是调用我指定的函数**。

   | 说法             | 角度                                               |
   | :--------------- | :------------------------------------------------- |
   | 注册信号         | 从**操作系统**的角度：告诉内核"这个信号由我来处理" |
   | 注册信号处理函数 | 从**程序员**的角度：把函数绑定到某个信号上         |

10. **进程挂起pause**：

    - 当你调用 `sigprocmask(SIG_BLOCK, &set, NULL)` 后，`SIGINT` 信号会被**阻塞**。
    - 此时如果按下 `Ctrl+C` 发送 `SIGINT`，信号不会被立即处理，而是进入 ** 待处理（pending）** 状态。
    - 待处理信号的规则
      - 对于**非实时信号**（如 `SIGINT`），**同一时刻最多只保留 1 个待处理实例**。如果在阻塞期间多次发送相同信号，内核只会记录一次。
      - 当信号被 ** 解除阻塞（unblock）** 时，**所有待处理的信号会被递送（deliver）一次**

    ```c
    sigprocmask(SIG_BLOCK, &set, NULL); // 阻塞 SIGINT
    sleep(10);                         // 在此期间按下 Ctrl+C，信号进入 pending
    sigprocmask(SIG_UNBLOCK, &set, NULL); // 解除阻塞，递送 pending 的 SIGINT → 执行 fun()
    pause();                            // 等待下一个信号
    /*
    第一次 Ctrl+C：在 sleep(10) 期间发送，信号被阻塞，进入 pending。
    解除阻塞后：pending 的 SIGINT 被递送，执行 fun()，输出第一串 1。
    第二次 Ctrl+C：在 pause() 期间发送，此时 SIGINT 已解除阻塞，信号被立即递送，再次执行 fun()，输出第二串 1。
    */
    ```

11. **原子的改变屏蔽状态并挂起sigsuspend**

    此代码能实现，在sigsuspend之前Ctrl+C(SIGINT)则会直接开始while，而sigsuspend之前没有Ctrl+C(SIGINT)则会在sigsuspend结束后开始挂起一直到捕获到一个非3即SIGQUIT的信号（因为这里的sigsuspend做的是将信号屏蔽字更新为只屏蔽3，注意：在suspend即挂起返回之后会将信号屏蔽字回退到更改之前即只屏蔽2）

    ```c
    #include <my_header.h>
    
    void fun1(int sig_val){
        printf("2222222222222222222\n");
    }
    void fun2(int sig_val){
        printf("3333333333333333333\n");
    }
    
    int main(int argc, char* argv[]){                  
        signal(SIGINT, fun1);
        signal(SIGQUIT, fun2);
        sigset_t set, oldset;
        sigemptyset(&set);
        sigaddset(&set, SIGINT);
    
        sigprocmask(SIG_BLOCK, &set, &oldset); // 只屏蔽2即SIGINT
        printf("sleep begin\n");
        sleep(10);
        printf("sleep over\n");
        /* sigprocmask(SIG_UNBLOCK, &set, NULL); */
        /* pause(); */
    
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGQUIT);
        sigsuspend(&mask); // 只屏蔽3即SIGQUIT，此时改变了信号掩码即信号屏蔽字（新的屏蔽的持续时间为从挂起状态返回之前），即这一个语句不可被打断连续做两个动作：更改信号掩码与进程挂起等待一个信号才返回
    
        printf("before while\n");
        while(1);
        printf("after while\n");
    
        return 0;
    }
    ```
