#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "LibMB.h"

int main(int argc, char **argv)
{
    int sock_FD;
    struct sockaddr_in server_addr, client_addr;
    socklen_t server_size, client_size;

    FILE *fileStream;
    char *fileName = "Topic.txt";

    char *buffer;
    buffer = calloc(BUF_SIZE, sizeof(char));

    char **topicMessage;
    topicMessage = (char **)malloc(LENGTH_OF_ENTRIES * sizeof(char *));
    for (int k = 0; k < LENGTH_OF_ENTRIES; k++)
    {
        topicMessage[k] = (char *)malloc(LENGTH_OF_ENTRIES * sizeof(char));
    }

    int nbytes, streamLength;

    sock_FD = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_FD < 0)
    {
        perror("Failure: unable to create socket");
        return EXIT_FAILURE;
    }

    server_size = sizeof(server_addr);
    client_size = sizeof(client_addr);
    memset(&server_addr, 0, server_size);
    memset(&client_addr, 0, client_size);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // reuse server after closing
    const int enable_reuse = 1;
    if (setsockopt(sock_FD, SOL_SOCKET, SO_REUSEADDR, &enable_reuse, sizeof(enable_reuse)) < 0)
    {
        perror("Failure: unable to make server address and port reuseable");
        return EXIT_FAILURE;
    }

    if ((bind(sock_FD, (struct sockaddr *)&server_addr, server_size)) < 0)
    {
        perror("Failure: unable to bind server to socket");
        return EXIT_FAILURE;
    }

    while (1)
    {
        // buffer = receiveMsg(sock_FD, buffer);
        nbytes = recvfrom(sock_FD, buffer, BUF_SIZE, 0, (struct sockaddr *) &client_addr, &client_size);
        buffer[nbytes] = '\0';
        fprintf(stderr, "Received Message: %s\n", buffer);

        int writeCheck = writeFile(fileName, buffer);
        printf("Write-Check: %d\n", writeCheck);

        sprintf(buffer, "ACK");
        streamLength = strlen(buffer);

        nbytes = sendto(sock_FD, buffer, streamLength, 0, (struct sockaddr *) &client_addr, client_size);
        
        int numberOfEntries = readFileContent(fileName, topicMessage);
        
        fprintf(stderr, "\nContent of: %s\n", fileName);
        for (int indexOfTopic = 0; indexOfTopic < numberOfEntries; indexOfTopic++)
        {
            fprintf(stderr, "%s\n", topicMessage[indexOfTopic]);
        }


        break;
    }

    free(buffer);
    // for (int k = 0; k < sizeof(topicMessage[k]); k++)
    // {
    //     free(topicMessage[k]);
    // }
    free(topicMessage);
    return EXIT_SUCCESS;
}