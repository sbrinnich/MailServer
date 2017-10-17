#include "operation_read.h"

OperationRead::OperationRead(int clientsocket, char* mailspooldir) : Operation(clientsocket, mailspooldir) {
    username = new char[9];
}

int OperationRead::parseRequest() {
    int ret = 0;
    send(clientsocket, "You chose to read an email. Please enter the following data.\nUsername: ",
         strlen("You chose to read an email. Please enter the following data.\nUsername: "), 0);
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

int OperationRead::doOperation() {
    if(username == NULL || strcmp(username, "") == 0 || messagenr <= 0){
        return 1;
    }

    // Get filepath
    std::stringstream filepath;
    filepath << mailspooldir << "/" << username;
    char* filename = getNthMailFilename(filepath.str().c_str(), messagenr);
    if(filename == nullptr){
        return 1;
    }
    filepath << "/" << filename;

    // Parse file
    char *sender, *subject, *content;
    sender = new char[9];
    subject = new char[81];
    content = new char[MAXMSG];
    int parsing = parseMailFile(filepath.str().c_str(), &sender, &subject, &content);

    // Check if parsing was successful
    if(parsing != 0){
        return 1;
    }

    // Print email (send to socket)
    std::stringstream out;
    out << "Sender: " << sender << "\nReceiver: " << username <<
        "\nSubject: " << subject << "\nContent: " << content << "\n";

    send(clientsocket, out.str().c_str(), strlen(out.str().c_str()), 0);

    // Delete allocations
    delete[] sender;
    delete[] subject;
    delete[] content;

    return 0;
}

OperationRead::~OperationRead() {
    delete[] username;
}