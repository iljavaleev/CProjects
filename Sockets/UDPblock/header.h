#include <sys/un.h>
#include <sys/socket.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
               

#define ERROR(str){ printf("Error: %s\n", str); exit(1); }

#define BUF_SIZE 100 

#define SV_SOCK_PATH "/tmp/ud_ucase"