#include "operation.h"

Operation::Operation(int clientsocket, char* mailspooldir, ClientHandler* clientHandler) :
        clientsocket(clientsocket), mailspooldir(mailspooldir),  clientHandler(clientHandler) {}

int Operation::getClientInput(int maxsize, char* *ptr){
    char buffer[MAXLINE];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    // Receive text from client, write to buffer
    ssize_t size = recv(clientsocket, buffer, MAXLINE-1, 0);

    // Client closed connection
    if(size == 0){
        return -1;
    }

    // Recv error or max size exceeded
    if(size > maxsize || size < 0){
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
    if(*ptr != nullptr){
        strcpy(*ptr, buffer);
    }else{
        perror("Could not parse client input.\n");
        return 1;
    }
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
        s = "Content: ";

        std::stringstream cnt_stream;
        cnt_stream << "";
        int first = 1;

        while(getline(file, line)){
            if(line.length() > 0) {
                if (first == 1) {
                    if (strncmp(line.c_str(), s.c_str(), strlen(s.c_str())) != 0) {
                        perror("MailFile Parse Error, Content");
                        return 1;
                    }
                    first = 0;
                    cnt_stream << line.substr(strlen(s.c_str())).c_str() << "\n";
                } else {
                    cnt_stream << line << "\n";
                }
            }
        }

        int len = strlen(cnt_stream.str().c_str());
        if(len >= MAXMSG){
            len = MAXMSG-1;
        }
        cnt_stream.str().copy(*content, len, 0);
        (*content)[len] = '\0';

        file.close();
        return 0;
    }
    return 1;
}

char* Operation::getNthMailFilename(const char *filepath, int n) {
    DIR* dir = opendir(filepath);
    if(!dir) {
        return nullptr;
    }
    struct dirent *ent;
    int count = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 && std::string(ent->d_name).find("_attachment") == -1) {//ignores attachments
            count++;
            if(count == n){
                return ent->d_name;
            }
        }
    }
    return nullptr;
}

Operation::~Operation() {}