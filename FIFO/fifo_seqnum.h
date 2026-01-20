#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define ERROR(str) { printf("Error: %s\n", str); exit(1); }

#define SERVER_FIFO "/tmp/seqnum_sv"

#define CLIENT_FIFO_TEMPLATE "/tmp/seqnum_cl.%ld"

#define SERVER_DB "/tmp/seqnum_db"

#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)

struct request
{
    pid_t pid;
    int seqLen; /* Длина запрашиваемой последовательности */
};

struct response
{
    int seqNum; /* Начало последовательности */
}; 


