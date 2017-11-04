#ifndef MAILSERVER_OPERATION_SEND_H
#define MAILSERVER_OPERATION_SEND_H

#include "../operation.h"

class OperationSend : public Operation {


private:
    char *receiver, *subject, *content;
    bool fileattached = false;
    int sendFileAttachment();

public:
    OperationSend(int clientsocket, char* mailspooldir, ClientHandler* clientHandler);
    ~OperationSend();
    int doPreparation() override;
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_SEND_H
