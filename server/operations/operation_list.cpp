#include "operation_list.h"

OperationList::OperationList(int clientsocket, char* mailspooldir, ClientHandler* clientHandler) :
        Operation(clientsocket, mailspooldir, clientHandler) {
}

int OperationList::doPreparation() {
    if(clientHandler->getUsername() == nullptr){
        send(clientsocket, "Please login to use this command!\n", strlen("Please login to use this command!\n"), 0);
        return 1;
    }
    return 0;
}

int OperationList::parseRequest() {
    return 0;
}

int OperationList::doOperation() {
    int mailcount = 0;
    std::stringstream mailsubjects;

    // Open directory
    std::stringstream dirpath;
    dirpath << mailspooldir << "/" << clientHandler->getUsername();
    DIR* dir = opendir(dirpath.str().c_str());
    if(dir){
        // Directory exists -> mailcount != 0, loop through files
        struct dirent *ent;
        char *sender, *subject, *content;
        sender = new char[9];
        subject = new char[81];
        content = new char[MAXMSG];
        int parsing;
        while((ent = readdir(dir)) != nullptr){
            if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0
                    && std::string(ent->d_name).find("_attach") == std::string::npos) {
                std::stringstream filepath;
                filepath << mailspooldir << "/" << clientHandler->getUsername() << "/" << ent->d_name;
                parsing = parseMailFile(filepath.str().c_str(), &sender, &subject, &content);
                if (parsing == 0) {
                    mailcount++;
                    mailsubjects << "#" << mailcount << ": " << subject << std::endl;
                } else {
                    perror("MailFile Parse Error in LIST");
                }
            }
        }
        delete[] sender;
        delete[] subject;
        delete[] content;
        closedir(dir);
    }

    std::stringstream out;
    out << "Found " << mailcount << " emails!\n" << mailsubjects.str();

    send(clientsocket, out.str().c_str(), strlen(out.str().c_str()), 0);

    // Receive OK from client
    auto buf = new char[10];
    recv(clientsocket, buf, 10, 0);
    delete[] buf;

    return 0;
}