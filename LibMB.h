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
#define ARGS_NUMBER 3
// waiting queue for incoming clients
#define BACKLOG 5
// index of SUB or PUB
#define INDEX_MSG_TYPE 0
// index of subscriber
#define SUB_ADDR_INDEX 2

/*
    Methods ready - will be included
*/

char **splitMessageByWhitespace(char *msg, char **pointer);

char *sendMsg(int sock, char *buffer, int streamLength);

char* receiveMsg(int sock, char *buffer);




// Not yet decided - in progress
int getCommand(char *command);
int writeInFile(int sock, char *fileName, char *content);
int sendACKMsg(int sock);
int sendFile(int sock, char *fileName);



#endif
