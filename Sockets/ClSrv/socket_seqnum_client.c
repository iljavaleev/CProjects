#define _DEFAULT_SOURCE
#include "headers.h"
#include <stdlib.h>

static char clientSocket[CLIENT_SOCKET_NAME_LEN];

static void on_exit_hndlr()
{
    if (remove(clientSocket) == -1 && errno != ENOENT)
        ERROR("remove");
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "—help") == 0)
        ERROR("usage: [seq-len…]\n");
    
    int cfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (cfd == -1)
        ERROR("socket");
    
    struct sockaddr_un sv_addr, cl_addr;
    snprintf(clientSocket, CLIENT_SOCKET_NAME_LEN, CL_SOCK_PATH_TMPL, 
        (long) getpid());
    if (strlen(clientSocket) > sizeof(cl_addr.sun_path) - 1)
        ERROR("Client socket path too long");
    
    if (remove(clientSocket) == -1 && errno != ENOENT)
        ERROR("remove");
    
    if (atexit(on_exit_hndlr) != 0)
        ERROR("Failed to register on_exit handler 1\n");    

    memset(&cl_addr, 0, sizeof(struct sockaddr_un));
    cl_addr.sun_family = AF_UNIX;
    strncpy(cl_addr.sun_path, clientSocket, sizeof(cl_addr.sun_path) - 1);

    if (bind(cfd, (struct sockaddr *) &cl_addr, 
        sizeof(struct sockaddr_un)) == -1)
    {
        if (errno == ENOENT)
            printf("er\n");
        ERROR("bind");
    }
            
    memset(&sv_addr, 0, sizeof(struct sockaddr_un));
    sv_addr.sun_family = AF_UNIX;
    strncpy(sv_addr.sun_path, SV_SOCK_PATH, sizeof(sv_addr.sun_path) - 1);

    if (connect(cfd, (struct sockaddr *) &sv_addr, 
        sizeof(struct sockaddr_un)) == -1)
    {
        printf("%s\n", strerror(errno));
        ERROR("connect");
    }
        
    struct request req;
    req.pid = getpid();
    req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;
    
    if (write(cfd, (void*) &req, sizeof(struct request)) != 
        sizeof(struct request))
            ERROR("partial/failed write");
 
    struct response resp;
    if (read(cfd, &resp, sizeof(struct response)) != 
        sizeof(struct response)) 
        ERROR("Error reading request; discarding\n");
    
    printf("%d\n", resp.seqNum);

    exit(EXIT_SUCCESS);
}
