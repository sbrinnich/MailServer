#include "operation_del.h"

OperationDel::OperationDel(int clientsocket, char* mailspooldir, ClientHandler* clientHandler) :
        Operation(clientsocket, mailspooldir, clientHandler) {
}

int OperationDel::doPreparation() {
    if(clientHandler->getUsername() == nullptr){
        send(clientsocket, "Please login to use this command!\n", strlen("Please login to use this command!\n"), 0);
        return 1;
    }
    return 0;
}

int OperationDel::parseRequest() {
    int ret = 0;

    char *nr;
    nr = new char[5];
    send(clientsocket, "You chose to delete an email. Please enter the following data.\nMessage Number: ",
         strlen("You chose to delete an email. Please enter the following data.\nMessage Number: "), 0);
    ret = getClientInput(5, &nr);
    if(ret == 1 || ret == -1){
        delete[] nr;
        return ret;
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
    if(messagenr <= 0){
        return 1;
    }

    std::stringstream filepath;
    filepath << mailspooldir << "/" << clientHandler->getUsername();
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

OperationDel::~OperationDel() {
}