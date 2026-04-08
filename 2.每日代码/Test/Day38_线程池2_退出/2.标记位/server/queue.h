#ifndef __QUEUE_H__
#define __QUEUE_H__

// 定义队列结点结构体
typedef struct node_s{
    int fd;
    struct node_s* next;
}node_t;

// 定义队列结构体
typedef struct queue_s{
    node_t* front;
    node_t* rear;
    int size;
}queue_t;

// 入队
int enQueue(queue_t* Queue, int fd);

// 出队
int deQueue(queue_t* Queue);

#endif
