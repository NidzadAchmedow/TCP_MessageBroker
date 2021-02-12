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

    // Storage for string messages + tempoaray storage
    char *buffer;
    buffer = (char *)malloc(BUF_SIZE * sizeof(char));
    char *topicMessage;
    topicMessage = (char *)malloc(BUF_SIZE * sizeof(char));
    char *topicPrefix;
    topicPrefix = (char *)malloc(BUF_SIZE * sizeof(char));

    // 2 dim array to store whole topics from file
    // usecase: # (wildcard) and filtering of topics
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

    // loop to handle incoming connections and messages from subscribers and publishers
    while (1)
    {
        // wait for incoming messages + print connected client and their message
        nbytes = recvfrom(sock_FD, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_size);
        buffer[nbytes] = '\0';
        fprintf(stderr, "Request from client: %s\n", inet_ntoa(client_addr.sin_addr)); // get client addr
        fprintf(stderr, "Received Message: %s\n", buffer);

        // case: [SUB] -> incoming message is from subscriber
        if (checkMessageType(buffer) == 0)
        {
            // split SUB message
            // structure: [sub topic] -> [sub] [topic]
            splitBuffer = splitMessageByToken(buffer, " ", splitBuffer);

            // store [message] of topic in "topicMessage"
            memcpy(topicMessage, splitBuffer[1], strlen(splitBuffer[1]));
            streamLength = strlen(splitBuffer[1]);
            topicMessage[streamLength] = '\0';

            // case: wildcard "#" is requested
            if (strcmp("#", topicMessage) == 0)
            {
                // get entries from file and store it in 2 dim array "splitBuffer"
                int numberOfEntries = readFileContent(fileName, splitBuffer);

                /*
                    Add all entries form 2 dim array "splitBuffer":
                        [
                            [topic message], 
                            [topic message], 
                            [...]
                        ]

                    to buffer (flatten 2 dim array in 1 dim array)
                    Buffer looks like this after function concat2DimArray:
                        [
                            topic message
                            topic message
                            ...
                        ]
                */
                buffer = concat2DimArray(splitBuffer, buffer);
                streamLength = strlen(buffer);

                // send build message to subscriber
                nbytes = sendto(sock_FD, buffer, streamLength, 0, (struct sockaddr *)&client_addr, client_size);
            }

            // case: specific topic is requested
            else
            {
                // set topicMessage to 0 to prevent data rubbish from former long messages
                memset(topicMessage, 0, BUF_SIZE);
                
                // copy content of buffer in topicMessage
                memcpy(topicMessage, buffer, strlen(buffer));

                // function to handle long topic names with spaces and save them in topicMessage as string
                incomingMessagePrefixHandler(topicMessage);

                // function to search for requested topic in file
                // if the requested topic not exists -> build error message
                if ((getRequestedTopic(topicMessage, buffer)) == NULL)
                {
                    sprintf(buffer, "Topic: [%s] not found!", topicMessage);
                }
                streamLength = strlen(buffer);
                buffer[streamLength] = '\0';

                // send result to subscriber
                nbytes = sendto(sock_FD, buffer, streamLength, 0, (struct sockaddr *)&client_addr, client_size);
            }
        }

        // case: PUB -> incoming message from publisher
        else if (checkMessageType(buffer) == 1)
        {
            // structure of incoming message: [pub topic < message]
            // split message in -> [pub topic] [message]
            splitBuffer = splitMessageByToken(buffer, PUB_SPLIT_TOKEN, splitBuffer);

            // store [message] of topic in topicMessage
            memcpy(topicMessage, splitBuffer[1], strlen(splitBuffer[1]));
            streamLength = strlen(splitBuffer[1]);
            topicMessage[streamLength] = '\0';

            // store topic name in topicPrefix
            topicPrefix = incomingMessagePrefixHandler(buffer);

            /* 
                to prevent redundancy in topic file, we have to check
                if broker has already an old entry of incoming topic from publisher
            */

            // case: no old topic to update, incoming topic is new
            if (checkUpdateTopicFile(topicPrefix) < 0)
            {
                // build topic entry
                sprintf(buffer, "%s %s", topicPrefix, topicMessage);
                
                // write topic with message in topic file
                int writeCheck = writeFile(fileName, buffer);
            }

            // case: old topic found, we have to update
            // function "checkUpdateTopicFile" deletes automaticly old topic of file
            else
            {
                // build topic entry
                sprintf(buffer, "%s %s", topicPrefix, topicMessage);
                
                // write updated version of topic in file
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