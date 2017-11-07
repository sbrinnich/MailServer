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
#include "client_handler.h"



class Operation {
protected:
    char buffer[MAXLINE];
    int clientsocket;
    char* mailspooldir;
    ClientHandler* clientHandler;


    /**
     * Reads from the client socket and writes read text into ptr.
     * @param maxsize the maximum number of characters to be received
     * @param ptr a pointer to a char* where the read text should be copied to
     * @return 0 if read was successful or
     *          1 if something went wrong (e.g. number of characters of read text exceeded maxsize limit) or
     *          -1 if client closed socket and receive failed
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
    /**
     * Creates a new instance of an operation
     * @param clientsocket the socket of the client requesting the operation
     * @param mailspooldir the directory where all the mails are saved to
     * @param clientHandler an instance to the ClientHandler handling this client's requests
     */
    Operation(int clientsocket, char* mailspooldir, ClientHandler* clientHandler);

    /**
     * Prepares the operation for parsing input later
     * @return 0 if operation was prepared successfully or
     *          1 if something went wrong and operation wasn't prepared properly (e.g. client not logged in)
     */
    virtual int doPreparation() = 0;

    /**
     * Parses the client's input from the class' clientsocket
     * @return 0 if syntax of client was correct and request could be parsed or
     *          1 if syntax wasn't right or something else went wrong or
     *          -1 if client closed socket and receive failed
     */
    virtual int parseRequest() = 0;
    /**
     * Executes the operation with the previously parsed arguments
     * @return 0 if operation was executed successfully or
     *          1 if something went wrong and operation wasn't executed properly
     */
    virtual int doOperation() = 0;

    virtual ~Operation();
};

#endif //MAILSERVER_OPERATION_H
