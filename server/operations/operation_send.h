#ifndef MAILSERVER_OPERATION_SEND_H
#define MAILSERVER_OPERATION_SEND_H

#include "../operation.h"

class OperationSend : public Operation {
public:
    OperationSend(int clientsocket);
    ~OperationSend();
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_SEND_H
