// 本程序演示线程的创建

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thmain(void *arg);    // 线程主函数

int main(int argc, char *argv[]){
    pthread_t thid = 0;     // 线程id typedef unsigned long pthread_t

    // 创建线程。
    if(pthread_create(&thid, NULL, thmain, NULL) != 0){
        // 一般失败只有操作系统有问题或者创建的线程太多了
        printf("pthread_create failed.\n");
        exit(-1);
    }

    // 等待子线程的退出。
    // join类似于进程里的wait
    printf("join...\n");
    pthread_join(thid, NULL);
    printf("join ok.\n");
}

void *thmain(void *arg){     // 线程主函数
    for(int i = 0; i < 5; i++){
        sleep(1);
        printf("pthmain sleep(%d) ok.\n", i+1);
    }
}