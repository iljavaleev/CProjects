#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
               

#define ERROR(str){ printf("Error: %s\n", str); exit(1); }

int isUnlocked(int fd)
{
    struct flock fl;
    fl.l_type = F_RDLCK;
    if (fcntl(fd, F_GETLK, &fl) == -1)
        ERROR("fcntl - F_GETLK");
    
    return fl.l_type == F_UNLCK;
}

void childInheritance(int fd)
{
    if (flock(fd, LOCK_EX) == -1) 
        ERROR("flock");
    
    pid_t pid;
    switch (pid = fork()) {
        case -1:
            ERROR("fork");
        case 0:
            if (flock(fd, LOCK_UN) == -1)
                ERROR("flock");
            break;

        default:
            if (wait(NULL) == -1) 
                ERROR("wait");
            break;
    }
    /* child process unlocked file */
    assert(isUnlocked(fd) == 1);    
}


void parentToChild(int fd)
{
    if (flock(fd, LOCK_EX) == -1) 
        ERROR("flock");
    
    pid_t pid;
    switch (pid = fork()) 
    {
        case -1:
            ERROR("fork");
        case 0:
            /* still locked after parent close file */
            assert(isUnlocked(fd) == 0);
            break;
        default:
            if (wait(NULL) == -1) 
                ERROR("wait");
            break;
    }
    if (flock(fd, LOCK_UN) == -1) 
        ERROR("flock");
    
}


int main(int argc, char** argv)
{
    pid_t pid;
    int status;

    int fd = open(argv[1], O_RDONLY);         
    if (fd == -1)
        ERROR("open file");
    
    childInheritance(fd);
    
    parentToChild(fd);

    if (close(fd) == -1)
        ERROR("close");
    
    exit(EXIT_SUCCESS);
}