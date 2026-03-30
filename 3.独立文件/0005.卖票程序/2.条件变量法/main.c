#include <my_header.h>
/*
此程序巧妙地仅使用一个条件变量就实现了卖票sell与补票supply之间的同步
*/
typedef struct share_state{
    int ticket_num;
    int flag; // 0-还未补票，1-已经补票
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}share_state_t;

void* sell_func(void* arg){ // 卖票-减少票，所以对应消费者-wait
    share_state_t* p_share_state = (share_state_t*)arg;

    while(1){
        pthread_mutex_lock(&p_share_state->mutex);

        if(p_share_state->ticket_num <= 0 && p_share_state->flag == 1){
            pthread_mutex_unlock(&p_share_state->mutex); // 票都没了且已经补票，所以无法继续卖票即终止线程
            break;
        }

        struct timeval now_time;
        gettimeofday(&now_time, NULL);
        srand((unsigned int)now_time.tv_usec);
        double rand_num = (double)rand() / RAND_MAX; // 把随机数映射到[0.0, 1.0]浮点数区间
                                                     // RAND_MAX：标准库宏定义，rand()函数能返回的最大整数值，绝大多数系统中RAND_MAX = 32767（即2^15 - 1）

        if(p_share_state->ticket_num > 0 && rand_num < 0.1){ // 只有票数>0且随机数<0.1(即每一次买票的人只有10%概率才买)时，才卖出1张票
            p_share_state->ticket_num--;
            printf("ticket_num = %d\n", p_share_state->ticket_num);
        }

        if(p_share_state->ticket_num <= 5 && p_share_state->flag == 0){
            pthread_cond_signal(&p_share_state->cond);
            pthread_cond_wait(&p_share_state->cond, &p_share_state->mutex); // 唤醒对方的同时立刻自我阻塞，然后等着对方完成之后唤醒自己
        }

        pthread_mutex_unlock(&p_share_state->mutex);
    }

    pthread_exit(NULL);
}


void* supply_func(void* arg){ // 补票-增加票，所以对应生产者-signal
    share_state_t* p_share_state = (share_state_t*)arg;
    // 补票的总过程只有一次，所以在补票线程这里并没有无限循环while(1)
    pthread_mutex_lock(&p_share_state->mutex);

    if(p_share_state->ticket_num > 5){ // 若是补票线程抢到CPU运行时不满足补票条件，则先自我阻塞并释放锁，等待条件满足时即被唤醒时直接补票
        pthread_cond_wait(&p_share_state->cond, &p_share_state->mutex);
        p_share_state->ticket_num += 10;
        p_share_state->flag = 1;
    }else{ // 若是补票线程一上来就满足补票条件，则直接补票，从而线程直接结束
        p_share_state->ticket_num += 10;
        p_share_state->flag = 1;
    } // 注意，这里的if-else都是对应要补票，只不过情况不一样而且，也就是说if-else分支只有一个能执行，但执行效果相同
    pthread_cond_signal(&p_share_state->cond);

    pthread_mutex_unlock(&p_share_state->mutex);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]){                                  
    share_state_t share_state;
    share_state.ticket_num = 20; // 初始时票数为20
    share_state.flag = 0; // 初始时还未补票
    pthread_mutex_init(&share_state.mutex, NULL);
    pthread_cond_init(&share_state.cond, NULL);

    pthread_t sell_id, supply_id;
    pthread_create(&sell_id, NULL, sell_func, (void*)&share_state);
    pthread_create(&supply_id, NULL, supply_func, (void*)&share_state);

    pthread_join(sell_id, NULL);
    pthread_join(supply_id, NULL);
    return 0;
}
