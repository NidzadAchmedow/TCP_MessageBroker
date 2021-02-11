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
    // File descriptor for socket
    int sock_FD;

    // server address variables
    struct sockaddr_in server_addr, client_addr;
    socklen_t server_size, client_size;

    // Pointer for file stream and file name
    FILE *fileStream;
    char *fileName = FILENAME_FOR_TOPICS;

    // Storage for oneline string messages + tempoaray storage
    char *buffer;
    buffer = (char *)malloc(BUF_SIZE * sizeof(char));
    char *topicMessage;
    topicMessage = (char *)malloc(BUF_SIZE * sizeof(char));
    char *topicPrefix;
    topicPrefix = (char *)malloc(BUF_SIZE * sizeof(char));

    // 2 dim array to store whole topics from file -> usecase: # (wildcard)
    char **splitBuffer;
    splitBuffer = (char **)malloc(LENGTH_OF_ENTRIES * sizeof(char *));
    for (int k = 0; k < LENGTH_OF_ENTRIES; k++)
    {
        splitBuffer[k] = (char *)malloc(LENGTH_OF_ENTRIES * sizeof(char));
    }

    // check values to test size of messages and buffer length
    int nbytes, streamLength;

    // create datagram socket for UDP + errorhandling
    sock_FD = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_FD < 0)
    {
        perror("Failure: unable to create socket");
        return EXIT_FAILURE;
    }

    // establish server structure
    server_size = sizeof(server_addr);
    client_size = sizeof(client_addr);
    memset(&server_addr, 0, server_size);
    memset(&client_addr, 0, client_size);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // make server socket reusable after closing server
    const int enable_reuse = 1;
    if (setsockopt(sock_FD, SOL_SOCKET, SO_REUSEADDR, &enable_reuse, sizeof(enable_reuse)) < 0)
    {
        perror("Failure: unable to make server address and port reuseable");
        return EXIT_FAILURE;
    }

    // bind socket to server + errorhandling
    if ((bind(sock_FD, (struct sockaddr *)&server_addr, server_size)) < 0)
    {
        perror("Failure: unable to bind server to socket");
        return EXIT_FAILURE;
    }

    // loop to handle incoming connections and messages
    while (1)
    {
        nbytes = recvfrom(sock_FD, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_size);
        buffer[nbytes] = '\0';
        fprintf(stderr, "Request from client: %s\n", inet_ntoa(client_addr.sin_addr)); // get client addr
        fprintf(stderr, "Received Message: %s\n", buffer);

        // case: SUB
        if (checkMessageType(buffer) == 0)
        {
            // split SUB message in parts
            // structure: [sub topic] -> [sub] [topic]
            splitBuffer = splitMessageByToken(buffer, " ", splitBuffer);

            // store [message] of topic in temporary storage
            memcpy(topicMessage, splitBuffer[1], strlen(splitBuffer[1]));
            streamLength = strlen(splitBuffer[1]);
            topicMessage[streamLength] = '\0';

            // check if wildecard is requested
            if (strcmp("#", topicMessage) == 0)
            {
                int numberOfEntries = readFileContent(fileName, splitBuffer);

                buffer = concat2DimArray(splitBuffer, buffer);
                streamLength = strlen(buffer);
                nbytes = sendto(sock_FD, buffer, streamLength, 0, (struct sockaddr *)&client_addr, client_size);
            }

            // get requested topic
            else
            {
                // set topicMessage to 0 to prevent data rubbish from former long messages and copy content of buffer in topicMessage
                memset(topicMessage, 0, BUF_SIZE);
                memcpy(topicMessage, buffer, strlen(buffer));

                // to handle long topic names with spaces and save them in topicMessage as string
                incomingMessagePrefixHandler(topicMessage);

                // search for requested topic in file and send it to subscriber
                if ((getRequestedTopic(topicMessage, buffer)) == NULL)
                {
                    sprintf(buffer, "Topic: [%s] not found!", topicMessage);
                }
                streamLength = strlen(buffer);
                buffer[streamLength] = '\0';
                nbytes = sendto(sock_FD, buffer, streamLength, 0, (struct sockaddr *)&client_addr, client_size);
            }
        }

        // case: PUB
        else if (checkMessageType(buffer) == 1)
        {
            // split message in [PUB Topic] and [Message]
            // structure: [pub topic < message] -> [sub topic] [message]
            splitBuffer = splitMessageByToken(buffer, PUB_SPLIT_TOKEN, splitBuffer);

            // store [message] of topic in topicMessage storage
            memcpy(topicMessage, splitBuffer[1], strlen(splitBuffer[1]));
            streamLength = strlen(splitBuffer[1]);
            topicMessage[streamLength] = '\0';

            // store topic name in topicPrefix and build file entry
            topicPrefix = incomingMessagePrefixHandler(buffer);

            // there is no old topic to update
            if (checkUpdateTopicFile(topicPrefix) < 0)
            {
                sprintf(buffer, "%s %s", topicPrefix, topicMessage);
                int writeCheck = writeFile(fileName, buffer);
            }

            // there is a update (line with old topic is now deleted) -> update topic with new message
            else
            {
                sprintf(buffer, "%s %s", topicPrefix, topicMessage);
                int writeCheck = writeFile(fileName, buffer);
            }
        }
    }

    // set allocated storage in broker free
    // free(buffer);
    free(topicMessage);
    free(topicPrefix);
    free(splitBuffer);
    for (int k = 0; k < sizeof(splitBuffer[k]); k++)
    {
        free(splitBuffer[k]);
    }
    return EXIT_SUCCESS;
}