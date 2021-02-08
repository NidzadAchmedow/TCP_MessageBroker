#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "LibMB.h"

int main(int argc, char **argv)
{
    // File descriptor for socket
    int sock_FD;

    // Storage for oneline string messages
    char *buffer;
    buffer = (char *)malloc(BUF_SIZE * sizeof(char));

    // check-values
    int nbytes, streamLength;

    // server address variables
    struct sockaddr_in server_addr;
    socklen_t server_size;
    struct hostent *hostPtr;
    char *hostName = NULL;

    // read from prompt
    if (argc < TERMINAL_ARGS_NUMBER)
    {
        fprintf(stderr, "Parameters to invoke %s: [hostname] [TOPIC]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // resolve hostname
    hostName = argv[ADDR_INDEX];
    if ((hostPtr = gethostbyname(hostName)) == NULL)
    {
        fprintf(stderr, "Failure: incorrect hostname");
        return EXIT_FAILURE;
    }

    // create socket + errorhandling
    sock_FD = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_FD < 0)
    {
        perror("Failure: unable to create socket");
        return EXIT_FAILURE;
    }

    server_size = sizeof(server_addr);
    memset(&server_addr, 0, server_size);

    server_addr.sin_family = AF_INET;
    memcpy((void *)&server_addr.sin_addr.s_addr, (void *)hostPtr->h_addr, hostPtr->h_length);
    server_addr.sin_port = htons(SERVER_PORT);

    // make it possible to re-use address and port of server after closing it
    const int enable_reuse = 1;
    if (setsockopt(sock_FD, SOL_SOCKET, SO_REUSEADDR, &enable_reuse, sizeof(enable_reuse)) < 0)
    {
        perror("Failure: unable to make server address and port reuseable");
        return EXIT_FAILURE;
    }

    // connect to server
    nbytes = connect(sock_FD, (struct sockaddr *)&server_addr, server_size);
    if (nbytes < 0)
    {
        perror("Failure: unable to connect to server");
        return EXIT_FAILURE;
    }

    //build message
    buffer = buildSubscriberMessage(argv[2], buffer);

    // send message
    buffer = sendMsg(sock_FD, buffer, streamLength);
    fprintf(stderr, "Send: %s\n", buffer);

    // receive message
    buffer = receiveMsg(sock_FD, buffer);
    fprintf(stderr, "Received-Message: %s\n", buffer);

    close(sock_FD);
    return EXIT_SUCCESS;
}

/*
    Source:
        https://www.geeksforgeeks.org/udp-server-client-implementation-c/
*/