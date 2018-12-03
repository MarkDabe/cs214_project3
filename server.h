#ifndef PROJECT_3_MAIN_H
#define PROJECT_3_MAIN_H
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <semaphore.h>
#define PORT 9001

pthread_t TIDS[256] = {0};
int COUNTER = 0;

pthread_mutex_t mutex;
sem_t semphore;

typedef struct _account{

    char* name;
    int accounts_number;
    double balance;
    short  in_session;
    struct account* next;

}account;

typedef struct _session_handler_args{

    account * head;
    int socket;
}session_handler_args;

#endif
