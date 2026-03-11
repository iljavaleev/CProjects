#include "header.h"


int main(int argc, char *argv[])
{
    struct sockaddr_un svaddr;
    char buf[BUF_SIZE];

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);       
    if (sfd == -1)
        ERROR("socket");

    if (strlen(SV_SOCK_PATH) > sizeof(svaddr.sun_path) - 1)
        ERROR("Server socket path too long");

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        ERROR("remove");

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &svaddr, 
        sizeof(struct sockaddr_un)) == -1)
        ERROR("bind");
    
    ssize_t numBytes;
    int num = 1;
    for (;;) 
    {
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0, 0, 0);
        if (numBytes == -1)
            ERROR("recvfrom");

        buf[numBytes] = '\0'; 
        printf("Server received message: %s  %d\n", buf, num++);
        sleep(1);
    }
}