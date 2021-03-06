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
    char *bufMsg = (char *)malloc(BUF_SIZE * sizeof(char));
    memcpy(bufMsg, msgToSplit, strlen(msgToSplit));

    char *tmp;
    int iter = 0;

    tmp = strtok(bufMsg, token);

    while (tmp != NULL)
    {
        strcpy(splittedMsg[iter], tmp);
        tmp = strtok(NULL, token);
        iter++;
    }
    free(tmp);
    free(bufMsg);
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

        for (int indexOfBuffer = 0; indexOfBuffer < fileEntryLength; indexOfBuffer++)
        {
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

char *getRequestedTopic(char *nameOfTopic, char *buffer)
{
    FILE *fileStream;
    if (fileStream = fopen(FILENAME_FOR_TOPICS, "r"))
    {
        while ((fgets(buffer, BUF_SIZE, fileStream)))
        {
            if ((strncmp(buffer, nameOfTopic, strlen(nameOfTopic))) == 0)
            {
                return buffer;
            }
        }
    }
    return NULL;
}

char *buildSubscriberMessage(char *topicToSubscribe, char *buffer)
{
    sprintf(buffer, "SUB %s", topicToSubscribe);
    return buffer;
}

char *buildPublisherMessage(char *topic, char *msg, char *buffer) {
    sprintf(buffer, "PUB %s < %s", topic, msg);
    return buffer;
}

int checkMessageType(char *type)
{
    char *tmp;
    tmp = (char *)malloc(BUF_SIZE * sizeof(char));

    char **splittedMsg;
    splittedMsg = (char **)malloc(LENGTH_OF_ENTRIES * sizeof(char *));
    for (int k = 0; k < LENGTH_OF_ENTRIES; k++)
    {
        splittedMsg[k] = (char *)malloc(LENGTH_OF_ENTRIES * sizeof(char));
    }

    memcpy(tmp, type, strlen(type));

    splittedMsg = splitMessageByToken(tmp, " ", splittedMsg);

    if ((strncmp("SUB", splittedMsg[0], 3)) == 0)
    {
        return 0;
    }

    else if ((strncmp("PUB", splittedMsg[0], 3)) == 0)
    {
        return 1;
    }

    else
    {
        return -1;
    }
}

char *concat2DimArray(char **arrToConcat, char *storeConcat)
{
    int cnt = 0;
    char *tmp = (char *)malloc(BUF_SIZE * sizeof(char));

    memcpy(tmp, storeConcat, strlen(storeConcat));
    memset(tmp, 0, strlen(storeConcat));

    while (strcmp(arrToConcat[cnt], "") != 0)
    {
        strcat(tmp, arrToConcat[cnt]);
        strcat(tmp, "\n");
        cnt++;
    }

    memcpy(storeConcat, tmp, strlen(tmp));
    free(tmp);
    return storeConcat;
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

int getDelimiterIndex(char *src[], int size, const char *delimiter) 
{
    for (int i = 0; i < size; i++)
    {
        if (!(strcmp(src[i], delimiter)))
        {
            return i;
        }
    }
    return -1;
}

char *incomingMessagePrefixHandler(char *message){
    char **tmpSplitMsg;
    tmpSplitMsg = (char **)malloc(LENGTH_OF_ENTRIES * sizeof(char *));
    for (int k = 0; k < LENGTH_OF_ENTRIES; k++)
    {
        tmpSplitMsg[k] = (char *)malloc(LENGTH_OF_ENTRIES * sizeof(char));
    }

    // make storage empty
    for (int k = 0; k < LENGTH_OF_ENTRIES; k++)
    {
        memset(tmpSplitMsg[k], 0, sizeof(tmpSplitMsg));
    }

    tmpSplitMsg = splitMessageByToken(message, PUB_SPLIT_TOKEN, tmpSplitMsg);
    tmpSplitMsg = splitMessageByToken(tmpSplitMsg[0], " ", tmpSplitMsg);
    
    // beginn index 1 because index 0 is PUB which is not needed
    int splitIndex = 1;

    // empty message to concat prefix in loop
    memset(message, 0, sizeof(message));

    // loop to concat topic
    while ((strcmp(tmpSplitMsg[splitIndex], "")) != 0)
    {
        strcat(message, tmpSplitMsg[splitIndex]);
        strcat(message, " ");
        splitIndex++;
    }
    return message;
}

int getUserInput(char *dest) 
{
    dest[0] = '\0';
    fprintf(stderr, "$ "); // to signal user input
    if(fgets(dest, MAX_STRING_SIZE, stdin))
    {
        dest[strlen(dest) -1] = '\0'; // remove new line character at the end
        return EXIT_SUCCESS;    
    }
    return EXIT_FAILURE;
}

void deleteLine(FILE *src, FILE *tmp, const int line) {
    char *buffer = (char *)malloc(BUF_SIZE * sizeof(char));

    int cnt = 0;

    while (fgets(buffer, BUF_SIZE, src))
    {
        if (cnt != line) {
            fputs(buffer, tmp);
        }
        cnt++;
    }
    free(buffer);
}

int removeLineFromFile(int line, char *fileName) {
    FILE *srcFileStream;
    FILE *tmpFileStream;

    char *tmpFileName = TEMP_FILE_TO_DELETE;

    int lineToDelete = line;

    srcFileStream = fopen(fileName, "r");
    tmpFileStream = fopen(tmpFileName, "a");

    if (srcFileStream == NULL || tmpFileStream == NULL)
    {
        perror("Failure: unable to open file!");
        return -1;
    }

    deleteLine(srcFileStream, tmpFileStream, lineToDelete);

    fclose(srcFileStream);
    fclose(tmpFileStream);

    remove(fileName);
    rename(tmpFileName, fileName);
    return 0;
}

int getLineOfTopic(char *nameOfTopic)
{
    FILE *fileStream;
    char *buffer = (char *)malloc(BUF_SIZE * sizeof(char));
    int line = 0;
    if (fileStream = fopen(FILENAME_FOR_TOPICS, "r"))
    {
        while ((fgets(buffer, BUF_SIZE, fileStream)))
        {
            if ((strncmp(buffer, nameOfTopic, strlen(nameOfTopic))) == 0)
            {
                free(buffer);
                return line;
            }
            line++;
        }
    }
    free(buffer);
    return -1;
}

int checkUpdateTopicFile(char *topicToUpdate) {
    int lineOfOldTopic;

    // no topic to update
    if ((lineOfOldTopic = getLineOfTopic(topicToUpdate)) < 0)
    {
        return -1;
    }

    removeLineFromFile(lineOfOldTopic, FILENAME_FOR_TOPICS);
    return 1;
}