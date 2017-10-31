#ifndef MAILSERVER_OPERATION_READ_H
#define MAILSERVER_OPERATION_READ_H

#include "../operation.h"

class OperationRead : public Operation {
private:
    int messagenr;

public:
    OperationRead(int clientsocket, char* mailspooldir, ClientHandler* clientHandler);
    ~OperationRead();
    int doPreparation() override;
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_READ_H
