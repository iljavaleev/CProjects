#include "header.h"


int main(int argc, char *argv[])
{
    struct sockaddr_un svaddr, claddr;
    char resp[BUF_SIZE];

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        ERROR("progname msg times\n");

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        ERROR("socket");

    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path, sizeof(claddr.sun_path),
            "/tmp/ud_ucase_cl.%ld", (long) getpid());

    if (bind(sfd, (struct sockaddr *) &claddr, 
        sizeof(struct sockaddr_un)) == -1)
        ERROR("bind");

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    ssize_t numBytes;
    size_t msgLen = strlen(argv[1]);
    int num = atoi(argv[2]); 
    for (int j = 0; j < num; j++) 
    {
        if (sendto(sfd, argv[1], msgLen, 0, (struct sockaddr *) &svaddr,
                sizeof(struct sockaddr_un)) != (ssize_t) msgLen)
        {
            if (errno == ENOBUFS)
            {
                printf("Queue overflow; sleep 5 seconds\n");
                sleep(5);
                continue;
            }
            ERROR("sendto");
        }
        printf("Send %d message\n", j + 1);
    }

    remove(claddr.sun_path);    
    exit(EXIT_SUCCESS);
}