#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ERROR(str){ printf("ERROR %s\n", str); exit(1); }


int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        ERROR("prog mq-name\n");
    if (mq_unlink(argv[1]) == -1)
        ERROR("mq_unlink");
    exit(EXIT_SUCCESS);
}
