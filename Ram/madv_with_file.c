#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 100
#define ERROR(str){ printf("ERROR: %s/n", str); exit(1); }

int main()
{
  
    int fd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, 0755);
	if (fd == -1)
		ERROR("open file");
    
    if (ftruncate(fd,  MAX_BUF_SIZE) == -1) 
    {
        close(fd);
        ERROR("Error truncating file");
    }

    int len;
    char *before_message = "some data";
    len = strlen(before_message);
    size_t total_written = 0, bytes_written = 0;
    
    while (total_written < len) 
    {
        bytes_written = write(fd, before_message + total_written, 
            len - total_written);
        if (bytes_written < 0) 
        {
            close(fd);
            ERROR("error write");
        }
        total_written += bytes_written;
    }  
    struct stat sb;
	if (fstat(fd, &sb) == -1)
		ERROR("fstat");
    char buf[MAX_BUF_SIZE];
    
    // read before map
    lseek(fd, 0, SEEK_SET);
    if ((len = read(fd, buf, sb.st_size)) == -1)
    {
        ERROR("read from file before map");
    }
    buf[len] = 0;
    printf("%s\n", buf);

    // overwite on mmap
    char *addr = mmap(NULL, MAX_BUF_SIZE, PROT_READ | PROT_WRITE, 
        MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        ERROR("mmap");
    
    char *over = "another data";
    len = strlen(over);
    memset(addr, 0, len);
    strncpy(addr, over, len);

    // read after overwriting
    lseek(fd, 0, SEEK_SET);
    if ((len = read(fd, buf, len)) == -1)
    {
        ERROR("read from file before map");
    }
    
    buf[len] = 0;
    printf("%s\n", buf);

    // set advice
    if (madvise(addr, MAX_BUF_SIZE, MADV_DONTNEED) == -1)
    {
        ERROR("madvise");
    }
    lseek(fd, 0, SEEK_SET);
    if (msync(addr, MAX_BUF_SIZE, MS_SYNC) == -1)
		ERROR("msync");

    // read after advice
    if ((len = read(fd, buf, len)) == -1)
    {
        ERROR("read from file before map");
    }
    
    buf[len] = 0;
    printf("%s\n", buf);

    munmap(addr, MAX_BUF_SIZE);
    exit(EXIT_SUCCESS);
}
