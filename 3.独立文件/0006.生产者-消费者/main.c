#include <my_header.h>
/*
存在两个生产者, 三个消费者;
生产者每隔3秒生产一个商品, 容器满则等待;
消费者,在前5秒不消费, 5秒之后, 每隔一秒消费一个商品, 无商品则等待;
容器初始10个商品, 最大上限20个商品
*/
typedef struct node_s{ // 商品结点
    int value;
    struct node_s* next;
}node_t;

typedef struct queue_s{ // 商品队列
    node_t *head; // 指向第一个商品
    node_t *tail; // 指向最后一个商品
    int size;
}queue_t;

void offer(queue_t *queue, int value){ // 提供一个价值为value的商品，并将其加入到容器中的商品队列
    node_t* new_node = (node_t*)calloc(1, sizeof(node_t));
    new_node->value = value;

    if(queue->size == 0)
        queue->head = new_node;
    else
        queue->tail->next = new_node;
    queue->tail = new_node; // if-else的公共操作
    queue->size++;
}

int fetch(queue_t* queue){ // 拿走一个商品，返回其价值fetch_value，并将其移出容器中的商品队列
    if(queue == NULL || queue->size == 0) // 队列为空或者队列中商品暂时被拿完
        return 0;

    node_t* old_head = queue->head;
    queue->head = old_head->next;
    queue->size--;
    if(queue->size == 0) // 特判：若是正好把商品拿空，则把尾指针置空，头指针在上面已经自我置空了
        queue->tail = NULL;
    int fetch_value = old_head->value;
    free(old_head);
    return fetch_value;
}

void show(queue_t* queue){ // 把队列里的所有商品按顺序都打印出来，方便看队列里当前都有什么
    node_t* cur = queue->head;
    if(cur == NULL)
        return;
    printf("==  ");
    while(cur){
        printf("%3d  ", cur->value);
        cur = cur->next;
    }
    printf("==\n");
}

// 共享资源上下文结构体
typedef struct share_value_s{ // 互斥锁、条件变量、标志变量的复合结构体
    queue_t queue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}share_value_t;

void* produce(void* arg){ // 生产者线程入口函数（同时运行2个此函数）
    share_value_t* p_share_value = (share_value_t*)arg; // 将传参强转为共享上下文结构体
    while(1){
        pthread_mutex_lock(&p_share_value->mutex);
        
        while(p_share_value->queue.size >= 20)
            pthread_cond_wait(&p_share_value->cond, &p_share_value->mutex);
        int value = rand() % 1000;
        offer(&p_share_value->queue, value);
        printf("add:%d\n", value);
        show(&p_share_value->queue);
        pthread_cond_broadcast(&p_share_value->cond); // 生产者生产完一个，必须叫醒所有人，让它们去抢锁、消费
                                                      // 如果用 signal 只叫醒一个，另外两个可能一直睡死，程序卡住

        pthread_mutex_unlock(&p_share_value->mutex);
        sleep(3);
    }
}

void* consume(void* arg){ // 消费者线程入口函数（同时运行3个此函数）
    share_value_t* p_share_value = (share_value_t*)arg;
    sleep(5);
    while(1){
        pthread_mutex_lock(&p_share_value->mutex);

        while(p_share_value->queue.size <= 0)
            pthread_cond_wait(&p_share_value->cond, &p_share_value->mutex);
        int value = fetch(&p_share_value->queue);
        printf("take out:%d\n", value);
        show(&p_share_value->queue);
        pthread_cond_broadcast(&p_share_value->cond); // 同理

        pthread_mutex_unlock(&p_share_value->mutex);
        sleep(1);
    }
}

int main(int argc, char* argv[]){                                  
    share_value_t share_value; // （最初始的）共享资源上下文结构体
    memset(&share_value.queue, 0, sizeof(queue_t));

    for(int i = 0; i < 10; i++){
        int data = rand() % 1000;
        offer(&share_value.queue, data);
    }
    show(&share_value.queue);
    // 
    pthread_mutex_init(&share_value.mutex, NULL);
    pthread_cond_init(&share_value.cond, NULL);

    pthread_t tid1, tid2, tid3, tid4, tid5;
    // 
    pthread_create(&tid1, NULL, produce, (void*)&share_value);
    pthread_create(&tid2, NULL, produce, (void*)&share_value);
    pthread_create(&tid3, NULL, consume, (void*)&share_value);
    pthread_create(&tid4, NULL, consume, (void*)&share_value);
    pthread_create(&tid5, NULL, consume, (void*)&share_value);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);
    pthread_join(tid5, NULL);

    pthread_mutex_destroy(&share_value.mutex);
    pthread_cond_destroy(&share_value.cond);

    return 0;
}
