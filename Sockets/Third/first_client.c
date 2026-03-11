#include "header.h"
#define MESSAGE "First client message"
#define CL_ADDR "/tmp/b"

int main(int argc, char *argv[])
{
    struct sockaddr_un svaddr, claddr;
    char resp[BUF_SIZE];

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        ERROR("socket");

    if (remove(CL_ADDR) == -1 && errno != ENOENT)
        ERROR("remove");

    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    strncpy(claddr.sun_path, CL_ADDR, sizeof(svaddr.sun_path) - 1);



    if (bind(sfd, (struct sockaddr *) &claddr, 
        sizeof(struct sockaddr_un)) == -1)
        ERROR("bind");

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    ssize_t numBytes;
    int num = 50; 
    for (int j = 0; j < num; j++) 
    {
        if (sendto(sfd, MESSAGE, strlen(MESSAGE), 0, (struct sockaddr *) &svaddr,
                sizeof(struct sockaddr_un)) != (ssize_t) strlen(MESSAGE))
        {
            
            fprintf(stderr, "Specific error message: %s\n", strerror(errno));
            if (errno == ENOBUFS)
            {
                printf("Queue overflow; sleep 5 seconds\n");
                sleep(5);
                continue;
            }
            ERROR("sendto");
        }
        sleep(1);
    }

    remove(claddr.sun_path);    
    exit(EXIT_SUCCESS);
}