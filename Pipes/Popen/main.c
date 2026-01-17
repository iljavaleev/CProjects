#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "log.h"

#define ERROR(str){ printf("error %s", str); exit(1); };
#define MAX_LINE_LENGTH 255

// FILE *fdopen(int fildes, const char *mode);

FILE *_popen(const char *command, const char *mode)
{
    int flow[2];
    int status;
    pid_t childPid;
    FILE *fp;

    if (pipe(flow) == -1) 
        ERROR("pipe");
    
    // else 
    //     return NULL;
    
    int desc = mode[0] == 'r' ? 0 : 1; 
    fp = fdopen(flow[desc], mode);
    if (fp == NULL) 
    {
        close(desc); 
        return NULL;
    }

     
    switch (childPid = fork()) 
    {
        case -1:
            ERROR("fork");
        case 0: 
            /* r parent read from stdout 
                w write to stdin */
            if (mode[0] == 'r')
            {
                if (flow[1] != STDOUT_FILENO) 
                { 
                    if (dup2(flow[1], STDOUT_FILENO) == -1)
                        ERROR("dup2");
                    if (close(flow[1]) == -1)
                        ERROR("close");
                    
                }
            }
            if (close(flow[desc]) == -1)
                ERROR("close unused end in child");
            

            execl("/bin/sh", "/bin/sh", "-c", command, (char *) NULL);
            perror("execl failed");
            return NULL;
           
        default:
            break;
           
    }
    


    if (mode[0] == 'w')
    {
        if (flow[0] != STDIN_FILENO) 
        { 
            if (dup2(flow[0], STDIN_FILENO) == -1)
                ERROR("dup2");
            if (close(flow[0]) == -1)
                ERROR("close");
        }
    }
    
    

    if (waitpid(childPid, &status, 0) == -1) 
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
   
    if (close(flow[!desc]) == -1)
        ERROR("close unused");
    if (WIFEXITED(status)) 
    {
        printf("Child exited with status %d\n", WEXITSTATUS(status));
    } 
    else 
    {
        printf("Child terminated abnormally\n");
    }

    return fp;
}

int _pclose(FILE* file)
{
    if (fclose(file) < 0)
        return -1;
    // код завершения дочернего процесс
    return 0;
}


int main(int argc, char **argv) 
{
    char buf[MAX_LINE_LENGTH];

    if (argc != 3 && strcmp(argv[1], "—help") == 0)
        ERROR("enter command to execute and mode [r|w]\n");   
    
    FILE *fp = _popen(argv[1], argv[2]);
    
    // if (fp == NULL)
    // {
    //     printf("Error open file\n");
    //     exit(EXIT_FAILURE);
    // }
    // ssize_t bytes_read;
   
    // while (fgets(buf, sizeof(buf), fp) != NULL) 
    // {
    //     printf("The string is: %s", buf);
    // } 
    
    // if (_pclose(fp) == -1)
    // {
    //     if (errno == EBADF)
    //         printf("1");
    //     ERROR("error close file end");
    // }

    char* payload = "data to pipe";
    
    int status;
    if (fputs(payload, fp) == EOF) 
    {
        perror("fputs error");
        status = _pclose(fp); // Attempt to close even on error
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
        
    return 0;
}
