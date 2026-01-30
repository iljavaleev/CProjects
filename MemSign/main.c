#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#define ERROR(str){ printf("ERROR: %s/n", str); exit(1); }

static void signal_handler(int signum) {
    
    if (signum == SIGBUS)
    {
        printf("Attempt to access addresses within pages that outside the \
            mapped region; SIGBUS was generated\n");
    }
    if (signum == SIGSEGV)
    {
        printf("Attempt to access addresses outside the \
            mapped region; SIGSEGV was generated\n");
    }  
    
    _exit(0);
}


int main(int argc, char *argv[])
{

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) ERROR("sigaction SIGSEGV");
    if (sigaction(SIGBUS, &sa, NULL) == -1) ERROR("sigaction SIGBUS");

    
    long page_size = sysconf(_SC_PAGESIZE); 
    if (page_size == -1) ERROR("get page size");
    

    long map_size = page_size * 2 / 3;

    printf("%d\n", map_size);

    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1)
        ERROR("open");
    
    int *addr = mmap(NULL, sizeof(int), PROT_READ | 
        PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);
    if (addr == MAP_FAILED)
        ERROR("mmap");

    if (close(fd) == -1)
        ERROR("close");
    
    // memset(addr, 1, map_size);
    
    // printf("%d\n", *(addr+map_size));
    
	exit(EXIT_SUCCESS);
}