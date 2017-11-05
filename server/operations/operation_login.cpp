#include "operation_login.h"

OperationLogin::OperationLogin(int clientsocket, char *mailspooldir, ClientHandler *clientHandler) :
    Operation(clientsocket, mailspooldir, clientHandler){
    username = new char[9];
    password = new char[MAXLINE];
}

int OperationLogin::doPreparation() {
    if(clientHandler->getUsername() != nullptr){
        send(clientsocket, "You're already logged in!\n", strlen("You're already logged in!\n"), 0);
        return 1;
    }
    return 0;
}

int OperationLogin::parseRequest() {
    int ret = 0;
    send(clientsocket, "Username: ", strlen("Username: "), 0);
    ret = getClientInput(9, &username);
    if(ret == 1 || ret == -1){
        return ret;
    }

    send(clientsocket, "Password: ", strlen("Password: "), 0);
    ret = getClientInput(MAXLINE, &password);
    if(ret == 1 || ret == -1){
        return ret;
    }

    return 0;
}

int OperationLogin::doOperation() {
    if(username == NULL || strcmp(username, "") == 0 ||
            password == NULL || strcmp(password, "") == 0){
        return 1;
    }

    LDAP *ld;
    LDAPMessage *result, *e;

    int rc=0;

    char *attribs[3], *dn;

    attribs[0]=strdup("uid");
    attribs[1]=strdup("cn");
    attribs[2]=NULL;


    // setup LDAP connection
    if ((ld=ldap_init(LDAP_HOST, LDAP_PORT)) == NULL){
        perror("ldap_init failed");
        return 1;
    }

    // anonymous bind
    rc = ldap_simple_bind_s(ld,NULL,NULL);

    if (rc != LDAP_SUCCESS){
        fprintf(stderr,"LDAP error: %s\n",ldap_err2string(rc));
        return 1;
    }

    // Create filter with uid
    std::stringstream filter("");
    filter << "(uid=" << username << ")";
    // perform ldap search
    rc = ldap_search_s(ld, SEARCHBASE, SCOPE, filter.str().c_str(), attribs, 0, &result);

    if (rc != LDAP_SUCCESS){
        fprintf(stderr,"LDAP search error: %s\n",ldap_err2string(rc));
        return 1;
    }

    // Check if user was found
    if(ldap_count_entries(ld, result) < 1){
        // User not found
        clientHandler->incrementFailedLogins();
        return 1;
    }

    // Get DN of found user
    e = ldap_first_entry(ld, result);
    dn = ldap_get_dn(ld,e);

    // Check if login is possible
    rc = ldap_simple_bind_s(ld,dn,password);

    if (rc != LDAP_SUCCESS){
        // Wrong password
        clientHandler->incrementFailedLogins();
        return 1;
    }


    // free memory used for result
    ldap_msgfree(result);
    free(attribs[0]);
    free(attribs[1]);

    ldap_unbind(ld);

    clientHandler->setUsername(username);
    return 0;
}

OperationLogin::~OperationLogin() {
    delete[] username;
    delete[] password;
}