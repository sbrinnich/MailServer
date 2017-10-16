#include "operation_list.h"

OperationList::OperationList(int clientsocket, char* mailspooldir) : Operation(clientsocket, mailspooldir) {
    username = new char[9];
}

int OperationList::parseRequest() {
    send(clientsocket, "You chose to list your emails. Please enter the following data.\nUsername: ",
         strlen("You choose to list your emails. Please enter the following data.\nUsername: "), 0);
    return getClientInput(9, &username);
}

int OperationList::doOperation() {
    // Check if username is set
    if(username == NULL || strcmp(username, "") == 0){
        perror("Username for LIST command not set");
        return 1;
    }

    int mailcount = 0;
    std::stringstream mailsubjects;

    // Open directory
    std::stringstream dirpath;
    dirpath << mailspooldir << "/" << username;
    DIR* dir = opendir(dirpath.str().c_str());
    if(dir){
        // Directory exists -> mailcount != 0, loop through files
        struct dirent *ent;
        char *sender, *subject, *content;
        sender = new char[9];
        subject = new char[81];
        content = new char[MAXLINE];
        int parsing;
        while((ent = readdir(dir)) != NULL){
            if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                std::stringstream filepath;
                filepath << mailspooldir << "/" << username << "/" << ent->d_name;
                parsing = parseMailFile(filepath.str().c_str(), &sender, &subject, &content);
                if (parsing == 0) {
                    mailcount++;
                    mailsubjects << subject << std::endl;
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

    return 0;
}

OperationList::~OperationList() {
    delete[] username;
}