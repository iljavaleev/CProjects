#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define ERROR(str){ printf("ERROR: %s/n", str); exit(1); }

int main(int argc, char *argv[])
{
    /* CP utility implementation with mmap and memcpy */ 
	if (argc != 3 || strcmp(argv[1], "--help") == 0)
		ERROR("usage error: file_to_copy file_to_out");

    long page_size = sysconf(_SC_PAGESIZE); 

    if (page_size == -1) ERROR("get page size");
        
    int input_fd = open(argv[1], O_RDONLY);
	if (input_fd == -1)
		ERROR("open input file");
	
	struct stat sb;
	if (fstat(input_fd, &sb) == -1)
		ERROR("fstat");

	char* source = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, input_fd, 0);
	if (source == MAP_FAILED)
		ERROR("mmap");

    if (close(input_fd) == -1)
        ERROR("close");

    int output_fd = open(argv[2], O_RDWR | O_CREAT, sb.st_mode & 07777);
	if (output_fd == -1)
		ERROR("open output file");

    if (ftruncate(output_fd,  sb.st_size) == -1) 
    {
        close(output_fd);
        ERROR("Error truncating file");
    }
    
    char* dest = mmap(NULL, sb.st_size , PROT_READ | 
        PROT_WRITE, MAP_SHARED, output_fd, 0);
	if (dest == MAP_FAILED)
		ERROR("mmap");

    if (close(output_fd) == -1)
        ERROR("close");

    memcpy(dest, source, sb.st_size);
    if (msync(dest, sb.st_size, MS_SYNC) == -1)
			ERROR("msync");
    
	exit(EXIT_SUCCESS);
}

