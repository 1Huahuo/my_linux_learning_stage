#include <my_header.h>

/*
主线程的消亡,引发的一些问题
    在上面的例子当中，如果主线程不sleep, 或者将 sleep(1) 改成 usleep(20)或者别的 (设个时间也取决于
系统的cpu核心和执行效率)，线程并发执行的特点会导致一些看上去非常奇怪的结果：在某次执行的时候，标准
输出上有一定的几率显示两条相同的语句。产生这种问题的原因是这样的，stdout缓冲区在多个线程之间是共享
，当执行 printf 时，会首先将stdout的内容拷贝到内核态的文件对象中，再清空缓冲区，当主线程终止导致所
有线程终止时，可能子线程已经将数据拷贝到了内核态（此时第一份语句已经打印了），但是stdout的内容并未
清空，此时进程终止，会把所有缓冲区清空，清空缓冲区的行为会将留存在缓冲区的内容直接清空并输出到标准
输出中，此时就会出现内容的重复打印了。
*/

void* thread_func(void* arg){
    printf("I am child thread\n");
    return NULL;
}

int main(int argc, char* argv[]){                                  
    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);
    printf("I am main thread\n");

    /* sleep(1); */

    return 0;
}

