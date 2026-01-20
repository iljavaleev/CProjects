#include "fifo_seqnum.h"
#include <stdlib.h>

static char clientFifo[CLIENT_FIFO_NAME_LEN];

/* Вызывается при выходе для удаления клиентской очереди FIFO */
static void removeFifo(void)
{
    unlink(clientFifo);
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "—help") == 0)
        ERROR("usage: [seq-len…]\n");
    
    umask(0);
    /* Создаем нашу очередь FIFO (до отправки запроса, чтобы избежать состояния гонки)  
       Ответ сервера должен быть в открытую очередь */
    
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, 
        (long) getpid());
    
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && 
        errno != EEXIST)
        ERROR("mkfifo client");
    
    if (atexit(removeFifo) != 0)
        ERROR("atexit");
    
    if (atexit(removeFifo) != 0) // register exit function
        ERROR("atexit");
    

    struct request req;
    struct response resp;
    /* Создаем запрос, открываем серверную очередь FIFO и отправляем запрос */
    req.pid = getpid();
    req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;
    
    int serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1)
        ERROR("open server fifo");
    
    if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
        ERROR("Can't write to server");

    /* Открываем нашу очередь FIFO, считываем и выводим ответ */
    int clientFd = open(clientFifo, O_RDONLY);
    if (clientFd == -1)
        ERROR("open client fifo");

    if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
        ERROR("Can't read response from server");

    printf("%d\n", resp.seqNum);

    exit(EXIT_SUCCESS);
}
