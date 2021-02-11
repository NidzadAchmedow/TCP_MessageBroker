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
    int indexOfDelimiter = getDelimiterIndex(argv, argc, PUB_SPLIT_TOKEN);

    // variable to control loop on program
    int runs = 1;
    
    // server address variables
    struct sockaddr_in server_addr;
    socklen_t server_size;
    struct hostent *hostPtr;
    char *hostName = NULL;

    // check if command is valid
    if (argc < 2)
    {
        fprintf(stderr, "Parameters to invoke %s: hostname\n", argv[0]);
        return EXIT_FAILURE;
    }

    // resolve hostname
    hostName = argv[ADDR_INDEX];
    if ((hostPtr = gethostbyname(hostName)) == NULL)
    {
        fprintf(stderr, "Failure: incorrect hostname\n");
        return EXIT_FAILURE;
    }

    // create socket + errorhandling
    sock_FD = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_FD < 0)
    {
        perror("Failure: unable to create socket\n");
        return EXIT_FAILURE;
    }

    //! missing documentation
    server_size = sizeof(server_addr);
    memset(&server_addr, 0, server_size);

    //! missing documentation
    server_addr.sin_family = AF_INET;
    memcpy((void *)&server_addr.sin_addr.s_addr, (void *)hostPtr->h_addr, hostPtr->h_length);
    server_addr.sin_port = htons(SERVER_PORT);

    // make it possible to re-use address and port of server after closing it
    const int enable_reuse = 1;
    if (setsockopt(sock_FD, SOL_SOCKET, SO_REUSEADDR, &enable_reuse, sizeof(enable_reuse)) < 0)
    {
        perror("Failure: unable to make server address and port reuseable\n");
        return EXIT_FAILURE;
    }

    // connect to server
    nbytes = connect(sock_FD, (struct sockaddr *)&server_addr, server_size);
    if (nbytes < 0)
    {
        perror("Failure: unable to connect to server\n");
        return EXIT_FAILURE;
    }

    // strings for user interaction
    char topic[MAX_STRING_SIZE];
    char message[MAX_STRING_SIZE];
    char optionInput[MAX_STRING_SIZE];
    char *options[] = {"pub", "sub", "exit"};

    // int to control user request loops
    int invalidInput = 1;

    // loop to keep running
    while (runs)
    {
        invalidInput = 1;
        while (invalidInput)
        {
            fprintf(stderr, "Welcome to smbClient!\nType one of the following options to continue :\n%s\n%s\n%s\n", options[0], options[1] , options[2]);
            if(getUserInput(optionInput))
            {
                fprintf(stderr, "Something went wrong..\n\n");
                return EXIT_FAILURE;
            }
            else if (strcmp(optionInput, options[0]) == 0) // case if publish was invoked
            {
                while (invalidInput) // ask for topic until correct input
                {
                    fprintf(stderr, "\nInvoked publish..\nEnter topic to continue :\n");
                    if(getUserInput(topic))
                    {
                        fprintf(stderr, "Something went wrong..\n\n");
                        return EXIT_FAILURE;
                    }
                    else if (strlen(topic) > 0) // valid topic was entered
                    {
                        while (invalidInput) // ask for message until correct input
                        {
                            fprintf(stderr, "\nEnter message for topic : \"%s\" to continue :\n", topic);
                            if(getUserInput(message))
                            {
                                fprintf(stderr, "Something went wrong..\n\n");
                                return EXIT_FAILURE;
                            }
                            else if (strlen(message) > 0) // valid message was entered
                            {
                                invalidInput = 0; // end loops for user input
                            }
                            else
                            {
                                fprintf(stderr, "Input must be at least one character!\n\n");
                            }
                        }

                        // build pub message when everything is set
                        buffer = buildPublisherMessage(topic, message, buffer);

                        // send message
                        nbytes = sendto(sock_FD, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, server_size);
                        fprintf(stderr, "\nSend: %s\n\n", buffer);
                    }
                }
            }
            else if (strcmp(optionInput, options[1]) == 0) // case if subscribe was invoked
            {
                while (invalidInput) // ask for requested topic until correct input
                {
                    fprintf(stderr, "\nInvoked subscribe..\nEnter requested topic to continue :\n");
                    if(getUserInput(topic))
                    {
                        fprintf(stderr, "Something went wrong..\n\n");
                        return EXIT_FAILURE;
                    }
                    else if (strlen(topic) > 0) // valid topic was entered
                    {
                        invalidInput = 0; // end loops for user input
                    }
                }

                // build sub request when everything is set
                buffer = buildSubscriberMessage(topic, buffer);

                // send subscribe message
                nbytes = sendto(sock_FD, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, server_size);
                fprintf(stderr, "\nSend: %s\n\n", buffer);

                // receive message
                nbytes = recvfrom(sock_FD, buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr, &server_size);
                fprintf(stderr, "Received-Message:\n%s\n\n", buffer);
            }
            else if (strcmp(optionInput, options[2]) == 0) // case if exit was invoked
            {
                fprintf(stderr, "\nLeaving..\n");
                return EXIT_SUCCESS;
            }
            else // case if no valid option was entered
            {
                fprintf(stderr, "Invalid input!\n\n");
            }
        }            
    }

    close(sock_FD);
    return EXIT_SUCCESS;
}

/*
    Source:
        https://www.geeksforgeeks.org/udp-server-client-implementation-c/
*/