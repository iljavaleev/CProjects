#include "header.h"


int main(int argc, char *argv[])
{
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        ERROR("socket");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path + 1, SV_SOCK_PATH, sizeof(addr.sun_path) - 2);

    if (connect(sfd, (struct sockaddr *) &addr, 
        sizeof(sa_family_t) + strlen(SV_SOCK_PATH) + 1) == -1)
        ERROR("connect");
    
    ssize_t numRead;
    char buf[BUF_SIZE];
    while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
        if (write(sfd, buf, numRead) != numRead)
            ERROR("partial/failed write");

    if (numRead == -1)
        ERROR("read");

    exit(EXIT_SUCCESS);
}