
/*
    Concurrent write to the file with mmap and FIXED flag
*/

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>


#define ERROR(str){ printf("ERROR: %s/n", str); exit(1); }
#define JOIN(tid) { if(pthread_join(tid, NULL) != 0) ERROR("error join"); }
#define CREATE(tid, func, arg){ \
    if(pthread_create(&tid, NULL, func, (void*) arg) != 0) \
    ERROR("error pthread_create");}


static long page_size;
static int fd;
static char *addr;


struct params
{
    int offset;
    char *message;
};


void* write_to_map(void* arg)
{
    struct params* par = (struct params*) arg;
   
    char *mapped_part = mmap(addr + page_size * par->offset, page_size, 
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_FIXED, fd, page_size * par->offset);
    if (mapped_part == MAP_FAILED)
    {
        printf("Error map thread");
        pthread_exit(NULL);
    }

    int len = strlen(par->message);
    if (len > page_size)
     {
        printf("Error len message");
        pthread_exit(NULL);
    }

    memset(mapped_part, '-', page_size);
    memcpy(mapped_part, par->message, len);

    return NULL;
}


int main()
{
    page_size = sysconf(_SC_PAGESIZE);
    if (!page_size)
        ERROR("get page size");
    
    long length = 3 * page_size; 

    fd = open("test.txt", O_RDWR | O_CREAT, 0755);
	if (fd == -1)
		ERROR("open file");

    if (ftruncate(fd,  length) == -1) 
    {
        close(fd);
        ERROR("Error truncating file");
    }

    addr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED 
        | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED)
        ERROR("mmap");

    struct params *p1 = (struct params *)malloc(sizeof(struct params));
    p1->offset = 2;
    p1->message = "end of the message";

    struct params *p2 = (struct params *)malloc(sizeof(struct params));
    p2->offset = 1;
    p2->message = "middle";

    struct params *p3 = (struct params *)malloc(sizeof(struct params));
    p3->offset = 0;
    p3->message = "start";
    

    pthread_t one, two, three; 
     
    CREATE(one, write_to_map, p1);
    CREATE(two, write_to_map, p2);
    CREATE(three, write_to_map, p3);
    
    JOIN(three);
    JOIN(two);
    JOIN(one);
 
    if (close(fd) == -1)
        ERROR("Error close file");
    
    free(p1);
    free(p2);
    free(p3);

    if (msync(addr, length, MS_SYNC) == -1)
		ERROR("msync");
    
   
    munmap(addr, length);
    exit(EXIT_SUCCESS);
}