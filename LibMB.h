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

#define BACKLOG 5

// index of SUB or PUB
#define INDEX_MSG_TYPE 0

// split token between [TOPIC] [MESSAGE]
#define PUB_SPLIT_TOKEN "<"

#define MAX_STRING_SIZE 255

// index of subscriber
#define ADDR_INDEX 1

// length of row and collumn of a sub or pub entry
#define LENGTH_OF_ENTRIES 50

#define FILENAME_FOR_TOPICS "Topic.txt"
#define FILENAME_SUBSCRIBER "Subscriber.txt"

/*
    Methods ready
*/

/**
 * Check if message is a SUB or PUB request.
 * @param type  String which includes PUB or SUB
 * @return      0 for SUB and 1 for PUB
*/
int checkMessageType(char *type);

/**
 * Split a message with any given token and store it in a 2 dim. array.
 * @param msgToSplit    String you want to split
 * @param token         Token to split string with
 * @param splittedMsg   2 dim array to store splitted message
 * @return              Splitted string as 2 dim array
*/
char **splitMessageByToken(char *msgToSplit, char *token, char **splittedMsg);

/**
 * Build a message to subscribe a topic.
 * @param topicToSubscripe  String which describes the requested topic to subscribe
 * @param buffer            Storage for builded SUB message
 * @return                  Builded message to subscribe a topic
*/
char *buildSubscriberMessage(char *topicToSubscribe, char *buffer);

/**
 * Build a message to publish a topic.
 * @param topic     Topic you want to publish
 * @param msg       Message for topic
 * @param buffer    Storage for builded message
 * @return          Builded message to publish topic
 */
char *buildPublisherMessage(char *topic, char *msg, char *buffer);

/**
 * Client: send a message to server or another client.
 * @param sock          File descriptor of socket
 * @param buffer        Content of message to send
 * @param streamLenght  Length of message
 * @return              String of send message
 * @exception           NULL
*/
char *sendMsg(int sock, char *buffer, int streamLength);

/**
 * Client: receive a message from server or another client.
 * @param sock      File descriptor of socket
 * @param buffer    Array to store received message
 * @return          Received message
 * @exception       NULL
*/
char* receiveMsg(int sock, char *buffer);

/**
 * Write a string in a specific file.
 * @param fileName      Name of file to write content
 * @param content       Content to write in file
 * @return              0 if successful
 * @exception           -1 if error occurs
*/
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

/**
 * Search for specific topic in file and return topic message.
 * @param nameOfTopic   Topic to search for
 * @param buffer        Storage for topic message
 * @return              Topic message
 * @exception           NULL if topic does not exist
*/
char *getRequestedTopic(char *nameOfTopic, char *buffer);

/**
 * Concatenates array of Strings (Call by reference)
 * @param src Array of Strings to concatenate
 * @param dest String to fill
 * @param start Starting index of src
 * @param end End index of src
 * @param size Elements in src
 * @param delimiter String to put between elements in dest
 * @return 0 = Success / 1 = Failure
 */
int concatArrayOfStrings(char *src[], char *dest, int start, int end, int size, const char *delimiter);

// --- Not yet decided - in progress ---
// int getCommand(char *command);
// int sendACKMsg(int sock);
// int sendFile(int sock, char *fileName);


#endif
