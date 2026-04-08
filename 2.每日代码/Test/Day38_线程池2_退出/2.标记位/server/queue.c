#include <my_header.h>
#include "queue.h"

// 入队
int enQueue(queue_t* Queue, int fd){
    node_t* node = (node_t*)calloc(1, sizeof(node_t));
    node->fd = fd;

    if(Queue->size == 0){ // 队列为空
        Queue->front = node;
        Queue->rear = node;
    }else{ // 队列不空
        Queue->rear->next = node;
        Queue->rear = node;
    }

    Queue->size++;

    return 0;
}

// 出队
int deQueue(queue_t* Queue){
    if(Queue->size == 0)
        return -1;

    int fd = Queue->front->fd;
    node_t* p = Queue->front;
    Queue->front = p->next;

    if(Queue->size == 1) // 若是此时出队前只有一个结点
        Queue->rear = NULL;

    Queue->size--;
    free(p);

    return fd;
}
