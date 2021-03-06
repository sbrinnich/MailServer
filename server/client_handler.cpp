#include <strings.h>
#include <cstring>
#include <sys/socket.h>

#include "client_handler.h"
#include "operation.h"
#include "operations/operation_send.h"
#include "operations/operation_list.h"
#include "operations/operation_read.h"
#include "operations/operation_del.h"
#include "operations/operation_login.h"

ClientHandler::ClientHandler(char *mailspooldir) :
        mailspooldir(mailspooldir), username(nullptr), failedLogins(0) {}

Operation* ClientHandler::getOperation(char* buffer, int clientsocket) {
    buffer[strlen(buffer)-1] = '\0';
    if(strcasecmp(buffer, OPERATION_LOGIN) == 0){
        return new OperationLogin(clientsocket, mailspooldir, this);
    }else if(strcasecmp(buffer, OPERATION_SEND) == 0){
        return new OperationSend(clientsocket, mailspooldir, this);
    }else if(strcasecmp(buffer, OPERATION_LIST) == 0){
        return new OperationList(clientsocket, mailspooldir, this);
    }else if(strcasecmp(buffer, OPERATION_READ) == 0){
        return new OperationRead(clientsocket, mailspooldir, this);
    }else if(strcasecmp(buffer, OPERATION_DELETE) == 0){
        return new OperationDel(clientsocket, mailspooldir, this);
    }
    return nullptr;
}

int ClientHandler::handleClient(int clientsocket) {
    char buffer[MAXLINE];
    int status = -1;
    char msg[MAXLINE];
    while(true) {
        // Check if login counter is too high
        if(getFailedLogins() >= 3) {
            return 1;
        }

        // Display message to client
        std::stringstream out_message;
        switch(status){
            case 0:
                out_message << "OK\n\n";
                break;
            case 1:
                out_message << "ERR\n\n";
                break;
            default:
                break;
        }
        out_message << INFO_TEXT;
        strcpy(msg, out_message.str().c_str());
        send(clientsocket, msg, strlen(msg), 0);

        // Get Operation from first input line of client
        std::fill(buffer, buffer + sizeof(buffer), 0);
        ssize_t size = recv(clientsocket, buffer, MAXLINE-1, 0);

        if(size <= 0) {
            // Could not read line, display error
            strcpy(buffer, "ERR\n");
            send(clientsocket, buffer, strlen(buffer), 0);
        }else {
            buffer[size] = '\0';

            std::stringstream quit;
            quit << OPERATION_QUIT << "\n";
            if (strcasecmp(buffer, quit.str().c_str()) == 0) {
                return 0;
            } else {
                status = handleClientRequest(clientsocket, buffer);
                if(status == -1){
                    return -1;
                }
            }
        }
    }
}

int ClientHandler::handleClientRequest(int clientsocket, char* request_msg) {
    char buffer[MAXLINE];
    std::fill(buffer, buffer + sizeof(buffer), 0);

    Operation *op = getOperation(request_msg, clientsocket);

    // Return 1 if no operation for handling client's command could be found
    if(op == nullptr) {
        return 1;
    }

    // Let operation prepare
    int prepare = op->doPreparation();
    if(prepare != 0){
        // Prepare failed
        delete op;
        return 1;
    }

    // Let operation parse the rest of client input
    int parse = op->parseRequest();
    if(parse > 0){
        // Operation could not parse input of client
        perror("Could not parse client input.\n");
        delete op;
        return 1;
    }
    if(parse < 0){
        // Client closed socket
        perror("Could not receive client input. Client socket closed by remote.\n");
        delete op;
        return -1;
    }

    // Let operation do something
    int done = op->doOperation();
    if(done != 0){
        // Operation could not do something
        delete op;
        return 1;
    }

    // Delete operation
    delete op;

    // Everything done
    return 0;
}

void ClientHandler::setUsername(char *username) {
    this->username = new char[9];
    strcpy(this->username, username);
}

char* ClientHandler::getUsername() {
    return this->username;
}

void ClientHandler::incrementFailedLogins() {
    this->failedLogins++;
}

int ClientHandler::getFailedLogins() {
    return this->failedLogins;
}