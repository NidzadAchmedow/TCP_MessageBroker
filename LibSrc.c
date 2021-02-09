#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>

#include "LibMB.h"


char **splitMessageByToken(char *msgToSplit, char *token, char **splittedMsg)
{
    char *delimiter = token;
    char *tmp;
    int iter = 0;

    tmp = strtok(msgToSplit, delimiter);

    while (tmp != NULL)
    {
        strcpy(splittedMsg[iter], tmp);
        tmp = strtok(NULL, delimiter);
        iter++;
    }
    free(tmp);
    return splittedMsg;
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

int writeFile(char *fileName, char *content)
{
    int transferedBytes, nbytes;
    FILE *fileStream;

    // receive content of dstFile and write in file
    if (fileStream = fopen(fileName, "a"))
    {

        fprintf(fileStream, "%s\n", content);
        fclose(fileStream);
        fprintf(stderr, "Wrote message in %s\n", fileName);
        return EXIT_SUCCESS;
    }
    else
    {
        perror("Failure: unable to open file for put");
        return -1;
    }
}

int readFileContent(char *fileName, char **buffer)
{
    FILE *fileStream;
    char tmp[LENGTH_OF_ENTRIES][LENGTH_OF_ENTRIES];
    if (fileStream = fopen(fileName, "r"))
    {
        int fileEntryLength = 0;
        while ((fgets(tmp[fileEntryLength], BUF_SIZE, fileStream)) != 0)
        {
            tmp[fileEntryLength][strlen(tmp[fileEntryLength]) - 1] = '\0';
            fileEntryLength++;
        }
        fclose(fileStream);
        
        for (int indexOfBuffer = 0; indexOfBuffer < fileEntryLength; indexOfBuffer++) {
            memcpy(buffer[indexOfBuffer], tmp[indexOfBuffer], sizeof(tmp[0]));
        }

        return fileEntryLength;
    }
    else
    {
        perror("Failure: unable to open file");
        return -1;
    }
}
// https://stackoverflow.com/questions/2225850/c-c-how-to-copy-a-multidimensional-char-array-without-nested-loops

char *getRequestedTopic(char *nameOfTopic, char *buffer)
{
    char **tmp;
    tmp = (char **)malloc(LENGTH_OF_ENTRIES * sizeof(char *));
    for (int k = 0; k < LENGTH_OF_ENTRIES; k++)
    {
        tmp[k] = (char *)malloc(LENGTH_OF_ENTRIES * sizeof(char));
    }

    int entryLength = readFileContent(FILENAME_FOR_TOPICS, tmp);

    for (int indexOfBuffer = 0; indexOfBuffer < entryLength; indexOfBuffer++)
    {
        if (buffer = strstr(tmp[indexOfBuffer], nameOfTopic))
        {
            return buffer;
        }
    }
    return NULL;
}

char *buildSubscriberMessage(char *topicToSubscribe, char *buffer) {
    sprintf(buffer, "SUB %s", topicToSubscribe);
    return buffer;
}

char *buildPublisherMessage(char *topic, char *msg, char *buffer) {
    sprintf(buffer, "PUB %s < %s", topic, msg);
    return buffer;
}

int checkMessageType(char *type) {
    if (strncmp(type, "SUB", 3) == 0)
    {
        return 0;
    }
    else {
        return 1;
    }
}

int concatArrayOfStrings(char *src[], char *dest, int start, int end, int size, const char *delimiter)
{
    if (!(start > end || end > size))
    {
        dest[0] = '\0'; // leert String
        for (int i = start; i < end; i++) {
            if ((strlen(dest) + strlen(src[i])) < MAX_STRING_SIZE) // check if max size has been reached
            {
                if (i > start) strcat(dest, delimiter); // add delimiter if beyond first string
                strcat(dest, src[i]);
            }
        }
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}