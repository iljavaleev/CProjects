#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define ERROR(str){ printf("ERROR: %s\n", str); exit(1); }

int create(char *mqname, int maxmsg, int msgsize);

