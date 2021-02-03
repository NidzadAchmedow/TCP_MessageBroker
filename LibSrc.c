#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>

#include "LibMB.h"

char **splitMessageByWhitespace(char *msgToSplit, char **pointer)
{
    char delimiter[] = " ";
    char *tmp;
    int iter = 0;

    tmp = strtok(msgToSplit, delimiter);

    while (tmp != NULL)
    {
        strcpy(pointer[iter], tmp);
        tmp = strtok(NULL, delimiter);
        iter++;
    }
    free(tmp);
    return pointer;
}

char *sendMsg(int sock_FD, char *message, int msgLength)
{
    int nbytes;

    nbytes = write(sock_FD, message, msgLength);
    if (nbytes != msgLength)
    {
        perror("Failure: Send Message");
        return NULL;
    }
    message[nbytes] = '\0';
    return message;
}

char *receiveMsg(int sock_FD, char *buffer)
{
    int nbytes;
    char tmp[BUF_SIZE];

    nbytes = read(sock_FD, tmp, sizeof(tmp) - 1);
    if (nbytes < 0)
    {
        perror("Failure: Receive Message");
        return NULL;
    }
    tmp[nbytes] = '\0';
    memcpy(buffer, tmp, sizeof(tmp));
    return buffer;
}

int writeFile(int sock, char *fileName, char *content)
{
    int transferedBytes, nbytes;
    FILE *fileStream;

    // receive content of dstFile and write in file
    if (fileStream = fopen(fileName, "w"))
    {
        while ((nbytes = read(sock, content, sizeof(content))) > 0)
        {
            transferedBytes = 0;
            // errorhandling read
            if (nbytes < 0)
            {
                perror("Failure: unable to receive file for put");
                return EXIT_FAILURE;
            }
            transferedBytes += nbytes;
            content[nbytes] = '\0';
            fprintf(fileStream, "%s", content);
        }
        fclose(fileStream);
        fprintf(stderr, "Received file saved in %s\n", fileName);
        return EXIT_SUCCESS;
    }
    else
    {
        perror("Failure: unable to open file for put");
        return EXIT_FAILURE;
    }
}