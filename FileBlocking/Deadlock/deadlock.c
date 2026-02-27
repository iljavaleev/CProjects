#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
               

#define ERROR(str){ printf("Error: %s\n", str); exit(1); }


int main(int argc, char *argv[])
{
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        ERROR("progname file1 file2");

    int fd1 = open(argv[1], O_RDONLY);         
    if (fd1 == -1)
        ERROR("open first file");
    
    int fd2 = open(argv[2], O_RDONLY);         
    if (fd2 == -1)
        ERROR("open second file");


    if (flock(fd1, LOCK_EX) == -1) 
        ERROR("flock");
    
    printf("PID %ld: get lock for %s\n", (long) getpid(), argv[1]);


    sleep(1);

    if (flock(fd2, LOCK_EX) == -1) 
        ERROR("flock");

    printf("PID %ld: get lock for %s\n", (long) getpid(), argv[2]);
    
    if (flock(fd1, LOCK_UN) == -1)
        ERROR("flock");

    if (flock(fd2, LOCK_UN) == -1)
        ERROR("flock");    
    
    exit(EXIT_SUCCESS);
}