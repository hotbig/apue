#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void prepare(void)
{
    int err;
    printf("preparing locks...\n");

    if((err = pthread_mutex_lock(&lock1)) != 0)
        printf("can't lock lock1 in prepare handler %d\n", err);
    if((err = pthread_mutex_lock(&lock2)) != 0)
        printf("can't lock lock2 in prepare handler %d\n", err);
}

void parent(void)
{
    int err;
    printf("parent unlocking...\n");

    if((err = pthread_mutex_unlock(&lock1)) != 0)
        printf("can't unlock lock1 in paremt handler %d\n", err);
    if((err = pthread_mutex_unlock(&lock2)) != 0)
        printf("can't unlock lock2 in parent handler %d\n", err);
}

void child(void)
{
    int err;
    printf("child unlocking...\n");

    if((err = pthread_mutex_unlock(&lock1)) != 0)
        printf("can't unlock lock1 in child handler %d\n", err);
    if((err = pthread_mutex_unlock(&lock2)) != 0)
        printf("can't unlock lock2 in child handler %d\n", err);
}

void *thr_fn(void *arg)
{
    printf("thread started....\n");
    pause();
    return 0;
}

int main(void)
{
    int err;
    pid_t pid;
    pthread_t tid;

    if((err = pthread_atfork(prepare, parent, child)) != 0)
    {
        printf("can't install fork handlers\n");
        exit(1);
    }

    if((err = pthread_create(&tid, NULL, thr_fn, 0)) != 0)
    {
        // ERR handling
    }

    sleep(2);
    printf("parent about to fork...\n");

    fflush(0);
    if((pid = fork()) < 0)
    {
        // ERR handling
    }
    else if(pid == 0)
    {
        printf("child returned from fork\n");
    }
    else
    {
        printf("parent returned from fork\n");
    }

    exit(0);
}

