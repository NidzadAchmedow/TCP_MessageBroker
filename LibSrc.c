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







// under construction
// Todo: how to write file content in a 2 dim array?

// https://stackoverflow.com/questions/2225850/c-c-how-to-copy-a-multidimensional-char-array-without-nested-loops
int readFileContent(char *fileName, char **buffer)
{
    FILE *fileStream;
    char tmp[25][25];
    // open file and send its content as msg
    if (fileStream = fopen(fileName, "r"))
    {
        int i = 0;
        while ((fgets(tmp[i], 25, fileStream)) != NULL)
        {
            tmp[i][strlen(tmp[i]) - 1] = '\0';
            i++;
        }
        fclose(fileStream);
        
        // pointer error at 7 entries
        for (int k = 0; k < i; k++) {
            memcpy(buffer[k], tmp[k], sizeof(tmp[0]));
            printf("membuf: %s\n",buffer[k]);
        }

        return i;
    }
    else
    {
        perror("Failure: unable to open file");
        return -1;
    }
}