#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#define ERROR(str) { printf("Error: %s\n", str); exit(1); }
#define SIGNAL_ERROR(str) { printf("Error: %s\n", str); _exit(1); }

#define SERVER_MQ "/server_mq"
#define SERVER_DB "/tmp/mq_db"
#define DELAY_SECONDS 5

#define CLIENT_MQ_TEMPLATE "/client_mq_%ld"
#define MQ_NAME_LEN 100

struct request
{
    pid_t pid;
    int seqLen; 
};

struct response
{
    int seqNum; 
}; 


