#ifndef MAILSERVER_OPERATION_H
#define MAILSERVER_OPERATION_H

#include <iostream>
#include <fstream>
#include <sstream>


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
