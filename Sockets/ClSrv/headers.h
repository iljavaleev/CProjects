#define _DEFAULT_SOURCE
#include <sys/un.h>
#include <sys/socket.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 5

#define ERROR(str) { printf("Error: %s\n", str); exit(1); }

#define SV_SOCK_PATH "/tmp/seqnum_sv"

#define CL_SOCK_PATH_TMPL "/tmp/seqnum_cl.%ld"

#define SERVER_DB "/tmp/seqnum_db"

#define CLIENT_SOCKET_NAME_LEN (sizeof(CL_SOCK_PATH_TMPL) + 20)

struct request
{
    pid_t pid;
    int seqLen; /* Длина запрашиваемой последовательности */
};

struct response
{
    int seqNum; /* Начало последовательности */
}; 


