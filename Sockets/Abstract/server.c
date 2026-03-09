#include "header.h"
#define BACKLOG 5


int main(int argc, char *argv[])
{
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        ERROR("socket");

    struct sockaddr_un addr;
    if (strlen(SV_SOCK_PATH) > sizeof(addr.sun_path) - 1)
        ERROR("Server socket path too long");

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        ERROR("remove");

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path + 1, SV_SOCK_PATH, sizeof(addr.sun_path) - 2);

    if (bind(sfd, (struct sockaddr *) &addr, 
        sizeof(sa_family_t) + strlen(SV_SOCK_PATH) + 1) == -1)
        ERROR("bind");

    if (listen(sfd, BACKLOG) == -1)
        ERROR("listen");
    
    int cfd;
    char buf[BUF_SIZE];
    ssize_t numRead;
    for (;;) 
    {          
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1)
            ERROR("accept");
       
        while ((numRead = read(cfd, buf, BUF_SIZE)) > 0)
            if (write(STDOUT_FILENO, buf, numRead) != numRead)
                ERROR("partial/failed write");

        if (numRead == -1)
            ERROR("read");

        if (close(cfd) == -1)
            ERROR("close");
    }

    exit(EXIT_SUCCESS);
}