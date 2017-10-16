#include "operation.h"

Operation::Operation(int clientsocket, char* mailspooldir) : clientsocket(clientsocket), mailspooldir(mailspooldir) {}

int Operation::getClientInput(int maxsize, char* *ptr){
    char buffer[MAXLINE];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    // Receive text from client, write to buffer
    ssize_t size = recv(clientsocket, buffer, MAXLINE-1, 0);

    if(size > maxsize){
        return 1;
    }

    // Append null-terminator to buffer
    if(size >= maxsize) {
        buffer[maxsize-1] = '\0';
    }else if(buffer[size-1] == '\n'){
        buffer[size-1] = '\0';
    }else{
        buffer[size] = '\0';
    }

    // Copy text in buffer to ptr
    strcpy(*ptr, buffer);
    return 0;
}

int Operation::parseMailFile(const char *filename, char **sender, char **subject, char **content) {
    std::ifstream file;
    file.open(filename, std::ios::in);
    if(file.is_open()){
        std::string line;
        std::string s;

        // Read line by line

        // Sender
        getline(file, line);
        s = "Sender: ";
        if(strncmp(line.c_str(), s.c_str(), strlen(s.c_str())) != 0){
            perror("MailFile Parse Error, Sender");
            return 1;
        }
        strcpy(*sender, line.substr(strlen(s.c_str())).c_str());

        // Subject
        getline(file, line);
        s = "Subject: ";
        if(strncmp(line.c_str(), s.c_str(), strlen(s.c_str())) != 0){
            perror("MailFile Parse Error, Subject");
            return 1;
        }
        strcpy(*subject, line.substr(strlen(s.c_str())).c_str());

        // Content
        getline(file, line);
        s = "Content: ";
        if(strncmp(line.c_str(), s.c_str(), strlen(s.c_str())) != 0){
            perror("MailFile Parse Error, Content");
            return 1;
        }
        strcpy(*content, line.substr(strlen(s.c_str())).c_str());

        file.close();
        return 0;
    }
    return 1;
}