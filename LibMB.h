#ifndef _LIBMB_
#define _LIBMB_


#define SERVER_PORT 8080
#define BUF_SIZE 1024
#define LENGTH_FILE_NAME 255
/* 
    structure:
        [programm] [sub/pub] [topic] [msg]
    if less args -> something is not right
*/
#define TERMINAL_ARGS_NUMBER 3
// waiting queue for incoming clients
#define BACKLOG 5
// index of SUB or PUB
#define INDEX_MSG_TYPE 0
// index of subscriber
#define SUB_ADDR_INDEX 2
// length of row and collumn of a sub or pub entry
#define LENGTH_OF_ENTRIES 50

#define FILENAME_FOR_TOPICS "Topic.txt"
#define FILENAME_SUBSCRIBER "Subscriber.txt"

/*
    Methods ready - will be included
*/

char **splitMessageByWhitespace(char *msg, char **pointer);

char *sendMsg(int sock, char *buffer, int streamLength);

char* receiveMsg(int sock, char *buffer);

int writeFile(char *fileName, char *content);

/**
    Read the given file and store the content of the file in buffer.
    MAX 50 Entries!
    @param filenName    file to read from
    @param buffer       pointer to store content of file
    @return             length of entries 
    @exception          returns -1 if there is an error
*/
int readFileContent(char *fileName, char **buffer);




// // Not yet decided - in progress
// int getCommand(char *command);
// int sendACKMsg(int sock);
// int sendFile(int sock, char *fileName);



#endif
