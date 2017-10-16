#ifndef MAILSERVER_OPERATION_H
#define MAILSERVER_OPERATION_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <strings.h>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>


#define MAXLINE 1024

class Operation {
protected:
    char buffer[MAXLINE];
    int clientsocket;
    char* mailspooldir;

    const int SENDER = 0;
    const int RECEIVER = 1;
    const int SUBJECT = 2;
    const int CONTENT = 3;


    /**
     * Reads from the client socket and writes read text into ptr.
     * @param maxsize the maximum number of characters to be received
     * @param ptr a pointer to a char* where the read text should be copied to
     * @return 0 if read was successful or
     *          1 if something went wrong (e.g. number of characters of read text exceeded maxsize limit)
     */
    int getClientInput(int maxsize, char* *ptr);

    /**
     * Parses a textfile containing an email (sender, subject and content)
     * @param filename the filename of the file to be parsed
     * @param sender a pointer to a char* where the sender should be put into
     * @param subject a pointer to a char* where the subject should be put into
     * @param content a pointer to a char* where the content should be put into
     * @return 0 if file could be parsed successfully or
     *          1 if something could not be parsed
     */
    int parseMailFile(const char* filename, char* *sender, char* *subject, char* *content);

    /**
     * Gets the filename of the nth read file in given directory
     * @param filepath the path to the directory where the nth file should be searched
     * @param n the number of the file
     * @return a char* containing the filename of the found file (without path) or
     *          nullptr, if no such file could be found
     */
    char* getNthMailFilename(const char* filepath, int n);

public:
    Operation(int clientsocket, char* mailspooldir);
    /**
     * Parses the client's input from the class' clientsocket
     * @return 0 if syntax of client was correct and request could be parsed or
     *          1 if syntax wasn't right or something else went wrong
     */
    virtual int parseRequest() = 0;
    /**
     * Executes the operation with the previously parsed arguments
     * @return 0 if operation was executed successfully or
     *          1 if something went wrong and operation wasn't executed properly
     */
    virtual int doOperation() = 0;
};

#endif //MAILSERVER_OPERATION_H
