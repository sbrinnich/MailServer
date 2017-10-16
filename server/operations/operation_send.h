#ifndef MAILSERVER_OPERATION_SEND_H
#define MAILSERVER_OPERATION_SEND_H

#include <sys/socket.h>
#include <sys/stat.h>
#include <strings.h>
#include <cstring>
#include <map>

#include "../operation.h"

class OperationSend : public Operation {


private:
    char *sender, *receiver, *subject, *content;

public:
    OperationSend(int clientsocket, char* mailspooldir);
    int getClientInput(int buffersize, char* *ptr);
    ~OperationSend();
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_SEND_H
