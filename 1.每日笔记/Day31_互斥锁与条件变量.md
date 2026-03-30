## Day31——互斥锁_条件变量

### 相关函数

1. **定义锁**

   ```c
   // 需要先定义一个pthread_mutex_t类型的锁(eg:)
   pthread_mutex_t mLock;
   ```

2. **初始化锁**

   ```c
   对锁的初始化,有两种方式: (man pthread_mutex_destroy)
   // 1, 一种是调用pthread_mutex_init函数
   int pthread_mutex_init(
       pthread_mutex_t *mutex, // 要初始化的锁
       const pthread_mutexattr_t *attr // 锁的属性类型
   );
   
   // 2, 另一种是直接将锁变量初始化为PTHREAD_MUTEX_INITIALIZER
   pthread_mutex_t mLock = PTHREAD_MUTEX_INITIALIZER;
   ```

3. **加锁**

   ```c
   使用pthread_mutex_lock加锁: (加锁后进入临界区)
   #include <pthread.h>
   // lock a mutex
   int pthread_mutex_lock(
       pthread_mutex_t *mutex // 锁
   );
   ```

4. **解锁**

   ```c
   使用pthread_mutex_unlock把锁置为未锁: (临界区代码执行结束,解锁)
   #include <pthread.h>
   // unlock a mutex
   int pthread_mutex_unlock(
       pthread_mutex_t *mutex // 锁
   );
   ```

5. **销毁锁**

   ```c
   在程序结束前回收锁,释放其占用的资源:
   #include <pthread.h>
   // destroy a mutex
   int pthread_mutex_destroy(
       pthread_mutex_t *mutex // 锁
   );
   ```

6. **gettimeofday——获取当前的时间和日期(POSIX标准)**

   ```c
   #include <sys/time.h>
   // get time
   int gettimeofday(
       struct timeval *tv, // 是一个指向timeval结构体的指针，用于存储获取的时间和日期
   	struct timezone *tz // 是一个指向timezone结构体的指针，用于指定时区信息，通常设置为NULL。
   );
   
   struct timeval
   {
       time_t tv_sec; // seconds 秒 = 1 × 10^6 微秒
       suseconds_t tv_usec; // microseconds 微秒
   }
   ```

7. **pthread_mutex_trylock——非阻塞锁**

   ```c
   #include <pthread.h>
   // lock a mutex
   int pthread_mutex_trylock(
       pthread_mutex_t *mutex
   );
   // int返回值, 获取到锁则返回0; 没有获取到锁则返回错误码
   
   // 注意：pthread_mutex_trylock配合循环可解锁第一种死锁问题
   ```

   ------

   

8. **定义条件变量**

   ```c
   pthread_cond_t cond;
   ```

9. **初始化条件变量**

   ```c
   #include <pthread.h>
   // initialize condition variables 
   int pthread_cond_init(
       pthread_cond_t *cond,		// 条件变量的指针
   	pthread_condattr_t *attr	// 条件变量属性对象 (默认NULL)
   )// 返回值: 初始化成功返回0；否则，返回一个错误码
   ```

10. **陷入阻塞==并释放锁==——==等待==**

    ```c
    #include <pthread.h>
    // wait on a condition 
    int pthread_cond_wait(
        pthread_cond_t *cond,		// 条件变量的指针
    	pthread_mutex_t *muten		// 要操作(释放和获取)的锁
    )// 返回值: 成功返回0；否则，返回一个错误码
    ```

11. **唤醒以指定条件变量阻塞的线程, ==并使其重新获取锁==——==唤醒==，每次从阻塞队列中取出==至少一个==唤醒,**

    ```c
    #include <pthread.h>
    // signal a condition 
    int pthread_cond_signal(
        pthread_cond_t *cond		// 条件变量的指针
    )；// 返回值: 成功返回0；否则，返回一个错误码
    ```

12. **销毁条件变量**

    ```c
    #include <pthread.h>
    // destroy a condition variables
    int pthread_cond_destroy(
        pthread_cond_t *cond		// 条件变量的指针
    )；// 返回值: 成功返回0；否则，返回一个错误码
    ```

13. **pthread_cond_timedwait——一个可设置超时的pthread_cond_wait**

    ```c
    #include <pthread.h>
    // wait on a condition 
    int pthread_cond_timedwait(
        pthread_cond_t *cond,		// 条件变量的指针
    	pthread_mutex_t *muten,		// 要操作(释放和获取)的锁
    	timespec *abstime			// 超时时间
    )// 返回值: 成功返回0；否则，返回一个错误码
        
    // timespec类型   
    struct timespec
    {
        __time_t tv_sec;//秒
        __syscall_slong_t tv_nsec; //纳秒
    };
    
    // __time_t  ->  typedef long int __time_t;
    // __syscall_slong_t  ->  typedef long int __syscall_slong_t;    
    ```

14. **pthread_cond_broadcast——以广播的方式把指定条件变量的阻塞队列线程==全部==唤醒，与pthread_cond_signal的每次从阻塞队列中取出==至少一个==唤醒不同**

    ```c
    #include <pthread.h>
    // broadcast a condition
    int pthread_cond_broadcast(
        pthread_cond_t *cond       // 条件变量的指针
    );
    // 返回值：成功返回0；否则，返回一个错误码
    ```

15. **srand**

    ```c
    #include <stdlib.h>
    void srand(unsigned int seed);
    int rand(void); // ！rand()永远不会返回负值，返回值范围：0 ~ RAND_MAX(标准库宏定义)
    
    #include <time.h>
    time_t time(time_t *timer); // time_t 即 long
    ```

### 笔记

1. **死锁的三种情况**

   <span style=color:red;;font-size:17px>死锁是指线程在执行过程中，由于竞争资源使用不当造成的一种永久阻塞的现象</span>，若无外力作用，某些逻辑将再阻塞状态下无法向下推进。使用互斥锁的时候必须小心谨慎，如果是需要持续多个锁的情况，加锁和解锁之间必须要维持一定的顺序。即使是只有一把锁，如果使用不当，也会导致死锁。

   1. **情况一**
      存在线程1:  先持有锁A, 再持有锁B;
      存在线程2:  先持有锁B, 再持有锁A;
      如果上述两个线程, 1先持有了A锁, 2持有了B锁;  在这种状态下1又试图去获取B锁, 2又试图获取A锁都会产生死锁。
   2. **情况二**
      持有锁的线程在未释放锁的情况下, 线程终止了, 造成锁未释放; 而另外的线程试图获取这个未释放的锁。
   3. **情况三**
      某个线程在对A锁进行加锁, 当持有了A锁之后, 在未释A放锁的情况下, 重复对A进行加锁。

2. **主线程中依次等待两个线程**

   ```c
   // 主线程
   pthread_join(a, NULL);  // 等待线程A
   pthread_join(b, NULL);  // 等待线程B
   ```

   **场景**：主线程 **卡在等 A** → **这时候 B 自己提前跑完了**

   - 等主线程终于等到 A 结束
   - **执行 pthread_join (b) 时，直接返回，不阻塞！**

   **核心原理**：**pthread_join 不管线程什么时候结束，只要线程已经终止了，join 就会立刻返回，不会等待。**

   - 线程 **已经死了** → `pthread_join` 直接回收资源，**不阻塞**
   - 线程 **还活着** → `pthread_join` 才会**阻塞等待**