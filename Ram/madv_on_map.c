#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 100
#define ERROR(str){ printf("ERROR: %s/n", str); exit(1); }

int main()
{
    // overwite on mmap
    char *addr = mmap(NULL, MAX_SIZE, PROT_READ | PROT_WRITE, 
        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED)
        ERROR("mmap");
    
    // write data
    char *mess = "some data";
    int len = strlen(mess);
    memset(addr, 0, len);
    strncpy(addr, mess, len);
    
    // read
    printf("%s\n", addr);

    // set advice
    if (madvise(addr, MAX_SIZE, MADV_DONTNEED) == -1)
    {
        ERROR("madvise");
    }
    // read after
    printf("%s\n", addr);

    munmap(addr, MAX_SIZE);
    exit(EXIT_SUCCESS);
}