#include "cpopen.h"


FILE *_popen(const char *command, const char *mode)
{
    int flow[2];
    int status;
    pid_t childPid;
    FILE *fp;

    if (pipe(flow) == -1) 
        ERROR("pipe");
    
    
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
    else
    {
        if (close(flow[!desc]) == -1)
            ERROR("close unused");
    }
    
    
    if (waitpid(childPid, &status, 0) == -1) 
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
   
    childStatus = WEXITSTATUS(status);
    
    return fp;
}

int _pclose(FILE* file)
{
    if (fclose(file) < 0)
        return -1;
    return childStatus;
}