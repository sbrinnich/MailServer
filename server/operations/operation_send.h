#ifndef MAILSERVER_OPERATION_SEND_H
#define MAILSERVER_OPERATION_SEND_H

#include <sys/socket.h>
#include <strings.h>
#include <cstring>
#include <map>

#include "../operation.h"

class OperationSend : public Operation {


private:
    std::map <std::string, char *> Data;

public:
    OperationSend(int clientsocket);
    int getClientInput(int buffersize, std::string key);
    ~OperationSend();
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_SEND_H
