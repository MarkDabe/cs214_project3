#include "server.h"


void * session_handler(void * args){


    session_handler_args* session_args = (session_handler_args*) args;

    printf("created thread\n");


    char buffer[1024] = {0};

    while (1){
        read(session_args->socket, buffer, 1024);

        printf(buffer);

        if(strcmp(buffer, "exit") == 0){
            printf("%s\n", buffer);
            send(session_args->socket, "session terminated", strlen("session terminated"), 0);
            break;
        }

        send(session_args->socket, "hello", strlen("hello"), 0);

    }



    pthread_exit(NULL);
}


void print_accounts(int signum)
{

    sem_post(&semphore);
    sem_wait(&semphore);
    printf("accounts\n");

}


int main(int argc, char const *argv[])
{

    struct sigaction sa;
    struct itimerval timer ;

    memset ( &sa, 0, sizeof ( sa ) ) ;

    sa.sa_handler = &print_accounts ;
    sigaction (SIGALRM, &sa, NULL );

    timer.it_value.tv_sec = 15 ;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 15;
    timer.it_interval.tv_usec = 0 ;

    setitimer(ITIMER_REAL, &timer, NULL) ;

    sem_init(&semphore, 0, 1);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_mutex_init(&mutex, NULL);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

//    l_opt.l_onoff = 0;
//    l_opt.l_linger = 0;
//
//
//    if ( setsockopt(server_fd, SOL_SOCKET, SO_LINGER, &l_opt, sizeof l_opt)){
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//
//
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
    }

    account* head = (account*) malloc(sizeof(account));
    head -> name = "linked_list_head";
    head -> accounts_number = 0;

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                                 (socklen_t *) &addrlen)) < 0) {
            perror("accept");
        }else {

            session_handler_args *args = (session_handler_args *) malloc(sizeof(session_handler_args));

            args->socket = new_socket;
            args->head = head;

            pthread_create(&TIDS[COUNTER], NULL, session_handler, args);
            COUNTER++;

        }

//        read(new_socket, buffer, 1024);
//
//        if(strcmp(buffer, "exit") == 0){
//            printf("%s\n", buffer);
//            send(new_socket, "bye", strlen("bye"), 0);
//            break;
//        }
//        send(new_socket, "hello", strlen("hello"), 0);
    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&semphore);
    return 0;
}


//#include <fcntl.h>
//#include <stdio.h>
//#include <sys/stat.h>
//#include <unistd.h>
//#include <string.h>
//#include <sys/socket.h>
//#include <stdlib.h>
//#include <string.h>
//#include <arpa/inet.h>
//#include <sys/ioctl.h>
//#include <errno.h>
//#include <pthread.h>
//#include <signal.h>
//#include <sys/time.h>
//#include <semaphore.h>
//
//sem_t mutex;
//
//void* thread_1(void* arg)
//{
//
//    sem_wait(&mutex);
//    int* t_1 = (int*) arg;
//    printf("\nEntered..%d\n", *t_1);
//    sleep(5);
//    printf("\nJust Exiting...%d\n", *t_1);
//    sem_post(&mutex);
//    return NULL;
//}
//
//void* thread_2(void* arg)
//{
//
//    sem_wait(&mutex);
//    int* t_1 = (int*) arg;
//    printf("\nEntered..%d\n", *t_1);
//    sleep(15);
//    printf("\nJust Exiting...%d\n", *t_1);
//    sem_post(&mutex);
//    return NULL;
//}
//
//
//void* thread_3(void* arg)
//{
//
//    sem_wait(&mutex);
//    int* t_3 = (int*) arg;
//    printf("\nEntered..%d\n", *t_3);
//    sleep(30);
//    printf("\nJust Exiting...%d\n", *t_3);
//    sem_post(&mutex);
//    return NULL;
//}
//
//
//void interrupt(int signum)
//{
//    sem_post(&mutex);
//    sem_wait(&mutex);
//    printf("\ninterrupt\n");
//
//
//
//}
//
//
//
//int main()
//{
//    struct sigaction sa;
//    struct itimerval timer ;
//
//    memset ( &sa, 0, sizeof ( sa ) ) ;
//
//    sa.sa_handler = &interrupt ;
//    sigaction (SIGALRM, &sa, NULL );
//
//    timer.it_value.tv_sec = 1 ;
//    timer.it_value.tv_usec = 0;
//    timer.it_interval.tv_sec = 1;
//    timer.it_interval.tv_usec = 0 ;
//
//    setitimer(ITIMER_REAL, &timer, NULL) ;
//    int t_1 = 1;
//    int t_2 = 2;
//    int t_3 = 3;
//    sem_init(&mutex, 0, 1);
//    pthread_t t1, t2, t3;
//    pthread_create(&t1,NULL,thread_1,&t_1);
//    pthread_create(&t2,NULL,thread_2,&t_2);
//    pthread_create(&t3,NULL,thread_3,&t_3);
//    pthread_join(t1,NULL);
//    pthread_join(t2, NULL);
//    pthread_join(t3, NULL);
//    sem_destroy(&mutex);
//    return 0;
//}