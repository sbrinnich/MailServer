#ifndef MAILSERVER_OPERATION_LOGIN_H
#define MAILSERVER_OPERATION_LOGIN_H

#include "../operation.h"
#include <ldap.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#define LDAP_HOST "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE

class OperationLogin : public Operation {
private:
    char *username;
    char *password;

public:
    /**
     * Creates a new instance of a login-operation
     * @param clientsocket the socket of the client requesting the operation
     * @param mailspooldir the directory where all the mails are saved to
     * @param clientHandler an instance to the ClientHandler handling this client's requests
     */
    OperationLogin(int clientsocket, char* mailspooldir, ClientHandler* clientHandler);
    ~OperationLogin() override;
    int doPreparation() override;
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_LOGIN_H
