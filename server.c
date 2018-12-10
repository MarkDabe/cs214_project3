#include "server.h"


account ** global_head = NULL;

account * find_account(account* head ,char* account_name){

    account* ptr =  head ->next;

    while(ptr != NULL){

        if(strcmp(ptr->name, account_name) == 0){
            break;
        }
        ptr = ptr->next;

    }

    return ptr;
}

account* add_account(account* head ,char* account_name){

    account* ptr = head;

    while(ptr->next != NULL){
        ptr =  ptr->next;

    }

    ptr->next = (account*) malloc(sizeof(account));

    ptr->next->name = (char*) malloc(strlen(account_name)+1);
    strncpy(ptr->next->name, account_name, strlen(account_name) + 1);
    ptr->next->balance = 0;
    ptr->next->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(ptr->next->mutex , NULL);

    ptr->next->next = NULL;

    return head;
}

void delete_list(account* head){
    account* ptr = head;
    account* temp = NULL;
    while (ptr != NULL){
        temp = ptr;
        pthread_mutex_destroy(temp->mutex);
        free(temp->mutex);
        free(temp->name);
        free(temp);
        ptr = ptr->next;

    }
}


void * session_handler(void * args){


    session_handler_args* session_args = (session_handler_args*) args;


    char * commands[5] = {"create", "serve", "deposit", "withdraw", "query"};

    char buffer[1024] = {0};

    int i = -1;

    while (1){

        read(session_args->socket, buffer, 1024);

        char* token = strtok(buffer, " ");

        if (i == 0){
            if(strcmp(token, "end") == 0){
                send(session_args->socket, "session terminated",
                     strlen("session terminated") + 1, 0);
                pthread_exit(NULL);
            }else if(strcmp(token, "quit") == 0){
                send(session_args->socket, "client disconnected",
                     strlen("client disconnected") + 1, 0);
                pthread_exit(NULL);
            }else{
                send(session_args->socket, "end session after adding an account",
                     strlen("end session after adding an account") + 1, 0);
                continue;

            }

        }

        for(i = 0; i < 5; i++){

            if(strcmp(token, commands[i]) == 0){
                break;
            }

        }

        if(i >= 5){
            send(session_args->socket, "session terminated", strlen("session terminated") +1, 0);
            pthread_exit(NULL);
        }else if (i == 0){
            token = strtok(NULL, " ");
            if( find_account(session_args->head, token) == NULL){
                if (add_account(session_args->head, token) != NULL ) {
                    send(session_args->socket, "account added", strlen("account added") + 1, 0);
                    continue;
                }else{
                    send(session_args->socket, "error adding account", strlen("error adding account") + 1, 0);
                    continue;
                }

            }

        }else if(i == 1){
            token = strtok(NULL, " ");

            account* session_account = find_account(session_args->head, token);

            if(session_account != NULL){
                if(session_account->in_session == 1){
                    send(session_args->socket, "account already in session", strlen("account already in session") + 1, 0);
                    pthread_exit(NULL);
                }

                pthread_mutex_lock(session_account->mutex);
                session_account->in_session = 1;
                send(session_args->socket, "account found", strlen("account found") + 1, 0);
                while (1){

                    read(session_args->socket, buffer, 1024);

                    token = strtok(buffer, " ");

                    if(token == NULL){
                        printf("NULL TOKEN\n");
                        continue;
                    }

                    for(i = 0; i < 5; i++){

                        if(strcmp(token, commands[i]) == 0){
                            break;
                        }

                    }


                    if(i == 2){



                        token = strtok(NULL, " ");

                        double amount = 0;

                        sscanf(token, "%lf", &amount);


                        session_account->balance = session_account->balance + amount;

                        send(session_args->socket, "deposit success", strlen("deposit success") + 1, 0);


                        continue;


                    }else if(i == 3){

                        token = strtok(NULL, " ");

                        double amount = 0;

                        sscanf(token, "%lf", &amount);

                        session_account->balance = session_account->balance - amount;

                        send(session_args->socket, "withdraw success", strlen("withdraw success") + 1, 0);

                        continue;

                    }else if(i == 4){



                        char balance[256] = {0};

                        sprintf(balance, "balance: %lf", session_account->balance);

                        send(session_args->socket, balance, strlen(balance) + 1, 0);

                        continue;


                    } else if(i >=5){
                        send(session_args->socket, "session terminated", strlen("session terminated") +1, 0);
                        session_account->in_session = 0;
                        pthread_mutex_unlock(session_account->mutex);
                        pthread_exit(NULL);
                    }




                }

            }else{
                send(session_args->socket, "account not found", strlen("account not fount") + 1, 0);
                continue;

            }
        }



    }

}


void print_accounts(int signum)
{
    sem_post(&semphore);
    sem_wait(&semphore);
    account* ptr = *global_head;

    while(ptr!= NULL){
        printf("%s\n", ptr->name);
        ptr = ptr->next;
    }

}


int main(int argc, char const *argv[])
{

    struct sigaction sa;
    struct itimerval timer ;

    memset ( &sa, 0, sizeof ( sa ) ) ;

    sa.sa_handler = &print_accounts ;
    sigaction (SIGALRM, &sa, NULL );

    timer.it_value.tv_sec = 1 ;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
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



    session_handler_args *args = (session_handler_args *) malloc(sizeof(session_handler_args));
    args->head = (account*) malloc(sizeof(account));
    args->head -> name = (char*) malloc(sizeof("linked_list_head") + 1);
    strncpy(args -> head -> name, "linked_list_head", strlen("linked_list_head") + 1);
    args -> head -> balance = -1;
    args -> head -> mutex = NULL;
    args -> head -> accounts_number = 0;
    args -> head -> in_session = -1 ;
    args -> head -> next = NULL;


    global_head = (account**) malloc(sizeof(account**));
    memcpy(global_head, &args->head, sizeof(account*));


    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                                 (socklen_t *) &addrlen)) < 0) {
        }else {

            args->socket = new_socket;

            pthread_create(&TIDS[COUNTER], NULL, session_handler, args);
            COUNTER++;


        }

    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&semphore);
    return 0;
}

