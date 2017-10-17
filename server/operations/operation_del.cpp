#include "operation_del.h"

OperationDel::OperationDel(int clientsocket, char* mailspooldir) : Operation(clientsocket, mailspooldir) {
}

int OperationDel::parseRequest() {
    int ret = 0;
    send(clientsocket, "You chose to delete an email. Please enter the following data.\nUsername: ",
         strlen("You chose to delete an email. Please enter the following data.\nUsername: "), 0);
    ret = getClientInput(9, &username);
    if(ret == 1){
        return 1;
    }

    char *nr;
    nr = new char[5];
    send(clientsocket, "Message Number: ", strlen("Message Number: "), 0);
    ret = getClientInput(5, &nr);
    if(ret == 1){
        delete[] nr;
        return 1;
    }

    std::stringstream s(nr);
    s >> messagenr;
    delete[] nr;
    if(s.fail()){
        return 1;
    }
    return 0;
}

int OperationDel::doOperation() {
    if(username == NULL || strcmp(username, "") == 0 || messagenr <= 0){
        return 1;
    }

    std::stringstream filepath;
    filepath << mailspooldir << "/" << username;
    char* filename = getNthMailFilename(filepath.str().c_str(), messagenr);
    if(filename == nullptr){
        return 1;
    }
    filepath << "/" << filename;

    std::string path = filepath.str();
    char* filenamewithpath = new char[path.length() + 1];
    std::copy(path.c_str(), path.c_str() + path.length() + 1, filenamewithpath);

    if(std::remove(filenamewithpath) == 0){
        send(clientsocket, "Email was successfully deleted.\n",
             strlen("Email was successfully deleted.\n"), 0);
        return 0;
    }else{
        perror( "Error deleting file: No such file or directory.\n" );
        return 1;
    }
}