#ifndef MAILSERVER_OPERATION_DEL_H
#define MAILSERVER_OPERATION_DEL_H

#include "../operation.h"

class OperationDel : public Operation {

private:
    int messagenr;

public:
    OperationDel(int clientsocket, char* mailspooldir, ClientHandler* clientHandler);
    ~OperationDel();
    int doPreparation() override;
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_DEL_H
