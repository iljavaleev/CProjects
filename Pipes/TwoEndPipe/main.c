#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define ERROR(str){ printf("error %s", str); exit(1); };
#define BUF_SIZE 100

int main(int argc, char *argv[])
{
    int tuda[2]; 
    if (pipe(tuda) == -1) 
        ERROR("pipe");

    
    int obratno[2];
    if (pipe(obratno) == -1)
        ERROR("pipe");


    char buf[BUF_SIZE];
    switch (fork()) 
    {
        case -1:
            ERROR("fork");
        case 0: 
            if (close(tuda[1]) == -1)
                ERROR("close tuda[1]");

            if (close(obratno[0]) == -1)
                ERROR("close obratno[0]");

            for (;;) 
            {   
                int numRead = read(tuda[0], buf, BUF_SIZE);
                if (numRead == -1)
                    ERROR("read");
                if (numRead == 0)
                    break;
                buf[numRead] = 0;
                if (strcmp(buf, "end") == 0)
                    break;
                for (int i=0; i<numRead; ++i)
                    buf[i] = toupper(buf[i]);
                if (write(obratno[1], buf, numRead) != numRead)
                    ERROR("child — partial/failed write");
                
            }
            exit(EXIT_SUCCESS);
        default:
            break;
    }

    if (close(tuda[0]) == -1) 
        ERROR("close tuda[0]");

    if (close(obratno[1]) == -1)
        ERROR("close obratno[1]");
    char input[100]; 

    for (;;) 
    { 
        printf("Enter word to upper or 'end' to exit: \n");
        fgets(input, sizeof(input), stdin);
        int len = strlen(input);
        if (input[len - 1] == '\n') 
            input[len - 1] = '\0';
            
        if (write(tuda[1], input, len) != len)
            ERROR("child — partial/failed write");
        
        int numRead = read(obratno[0], buf, BUF_SIZE);
        if (numRead == -1)
            ERROR("read");
        if (numRead == 0)
            break;

        buf[numRead] = 0;
        printf("Upper word: %s\n", buf);
    }           


    if (close(tuda[1]) == -1)
        ERROR("close 5");
    if (close(obratno[0]) == -1)
        ERROR("close 6");

    exit(EXIT_SUCCESS);
}
