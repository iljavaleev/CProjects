#include "cpopen.h"


int main(int argc, char **argv) 
{
    char buf[MAX_LINE_LENGTH];

    if (argc != 3 && strcmp(argv[1], "—help") == 0)
        ERROR("enter command to execute and mode [r|w]\n");   
    
    char* mode = argv[2];
    if (!(mode[0] == 'r' || mode[0] == 'w'))
        ERROR("enter mode 'r' or 'w' only\n"); 

    FILE *fp = _popen(argv[1], mode);

    if (mode[0] == 'r')
    {
        if (fp == NULL)
        {
            printf("Error open file\n");
            exit(EXIT_FAILURE);
        }
        ssize_t bytes_read;
    
        while (fgets(buf, sizeof(buf), fp) != NULL) 
        {
            printf("The string is: %s", buf);
        } 
        
        if (_pclose(fp) == -1)
        {
            if (errno == EBADF)
                printf("1");
            ERROR("error close file end");
        }
    }
    else if (mode[0] == 'w')
    {
        char* payload = "data to pipe";
        
        int status;
        if (fputs(payload, fp) == EOF) 
        {
            perror("fputs error");
            status = _pclose(fp); 
            if (status == -1) 
            {
                perror("pclose error");
            }
            exit(EXIT_FAILURE);
        }

        status = _pclose(fp);
        if (status == -1) 
        {
            perror("pclose error");
            exit(EXIT_FAILURE);
        }   
    }    
    return 0;
}
