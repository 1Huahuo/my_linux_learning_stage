#include <my_header.h>

typedef struct share_state
{
    int flag; // 0: 未加票, 1已加票 
    int ticketNum;
    pthread_mutex_t mLock;
} share_state_t;

void *sellFun(void *arg)
{
    share_state_t *pShareState = (share_state_t *)arg;

    while(1)
    {
        pthread_mutex_lock(&pShareState->mLock);

        if(pShareState->ticketNum <= 0 && pShareState->flag == 1)
        {
            pthread_mutex_unlock(&pShareState->mLock);
            break;
        }

        struct timeval nowTime;
        gettimeofday(&nowTime, NULL);
        srand((unsigned int) nowTime.tv_usec );
        double rand_num = (double)rand()/RAND_MAX;

        if(pShareState->ticketNum > 0 && rand_num < 0.1)
        {
            pShareState->ticketNum--;
            printf("ticketNum = %d \n", pShareState->ticketNum);
        }
        pthread_mutex_unlock(&pShareState->mLock);
    }
    pthread_exit(NULL);
}

void *purchaseFun(void *arg)
{
    share_state_t *pShareState = (share_state_t *)arg;

    while(1)
    {
        pthread_mutex_lock(&pShareState->mLock);

        if(pShareState->ticketNum <= 5)
        {
            pShareState->ticketNum = pShareState->ticketNum + 10;
            pShareState->flag = 1;
            pthread_mutex_unlock(&pShareState->mLock);
            break;
        } 

        pthread_mutex_unlock(&pShareState->mLock);
    }
    pthread_exit(NULL);
}

int main(int argc,char* argv[])
{
    share_state_t shareState;
    shareState.ticketNum = 20;
    shareState.flag = 0;
    pthread_mutex_init(&shareState.mLock, NULL);

    pthread_t pid1, pid2;
    pthread_create(&pid2,NULL,purchaseFun,&shareState);
    pthread_create(&pid1,NULL,sellFun,&shareState);

    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);
    return 0;
}
