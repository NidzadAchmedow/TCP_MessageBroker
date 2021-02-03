#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "LibMB.h"

int main(int argc, char **argv) {
    int sock_FD;
    char *buffer;
    buffer = calloc(BUF_SIZE, sizeof(char));

    int nbytes, streamLength;

    socklen_t server_size;

    struct sockaddr_in server_addr;

    sock_FD = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_FD < 0)
    {
        perror("Failure: unable to create socket");
        return EXIT_FAILURE;
    }

    server_size = sizeof(server_addr);
    memset(&server_addr, 0, server_size);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    sprintf(buffer, "Client-Message :D");
    streamLength = strlen(buffer);

    buffer = sendMsg(sock_FD, buffer, streamLength);

    buffer = receiveMsg(sock_FD, buffer);

    fprintf(stderr, "Received-Message: %s\n", buffer);

    close(sock_FD);
    return EXIT_SUCCESS;
}

/*
    Source:
        https://www.geeksforgeeks.org/udp-server-client-implementation-c/
*/