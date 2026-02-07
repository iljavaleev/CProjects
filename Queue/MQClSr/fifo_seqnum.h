#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define ERROR(str) { printf("Error: %s\n", str); exit(1); }

#define SERVER_MQ "server_mq"

#define CLIENT_MQ_TEMPLATE "client_mq.%ld"


struct request
{
    pid_t pid;
    int seqLen; /* Длина запрашиваемой последовательности */
};

struct response
{
    int seqNum; /* Начало последовательности */
}; 


