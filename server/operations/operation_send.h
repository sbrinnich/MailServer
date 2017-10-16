#ifndef MAILSERVER_OPERATION_SEND_H
#define MAILSERVER_OPERATION_SEND_H

#include "../operation.h"

class OperationSend : public Operation {


private:
    char *sender, *receiver, *subject, *content;

public:
    OperationSend(int clientsocket, char* mailspooldir);
    ~OperationSend();
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_SEND_H
