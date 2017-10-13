#ifndef MAILSERVER_OPERATION_READ_H
#define MAILSERVER_OPERATION_READ_H

#include "../operation.h"

class OperationRead : public Operation {
public:
    OperationRead(int clientsocket);
    ~OperationRead();
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_READ_H
