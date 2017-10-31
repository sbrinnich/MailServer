#include <sys/time.h>

#include "operation_send.h"

OperationSend::OperationSend(int clientsocket, char* mailspooldir, ClientHandler* clientHandler) :
        Operation(clientsocket, mailspooldir, clientHandler) {
    receiver = new char[9];
    subject = new char[81];
    content = new char[MAXMSG];
}

int OperationSend::doPreparation() {
    if(clientHandler->getUsername() == nullptr){
        send(clientsocket, "Please login to use this command!\n", strlen("Please login to use this command!\n"), 0);
        return 1;
    }
    return 0;
}

int OperationSend::parseRequest() {

    int ret = 0;
    send(clientsocket, "You chose to send an email. Please enter the following data.\nReceiver: ",
         strlen("You chose to send an email. Please enter the following data.\nReceiver: "), 0);
    ret = getClientInput(9, &receiver);
    if(ret == 1 || ret == -1){
        return ret;
    }
    send(clientsocket, "Subject: ", strlen("Subject: "), 0);
    ret = getClientInput(81, &subject);
    if(ret == 1 || ret == -1){
        return ret;
    }

    char* cnt_tmp;
    cnt_tmp = new char[MAXLINE];
    std::stringstream cnt_stream;
    cnt_stream << "";

    do {
        send(clientsocket, "Content: ", strlen("Content: "), 0);
        ret = getClientInput(MAXLINE, &cnt_tmp);
        if (ret == 1 || ret == -1) {
            delete[] cnt_tmp;
            return ret;
        }
        if(strcmp(cnt_tmp, ".") == 0){
            break;
        }
        cnt_stream << cnt_tmp << "\n";
        if(strlen(cnt_stream.str().c_str()) >= MAXMSG){
            break;
        }
    }while(strcmp(cnt_tmp, ".") != 0);
    delete[] cnt_tmp;

    int len = strlen(cnt_stream.str().c_str());
    if(len >= MAXMSG){
        len = MAXMSG-1;
    }
    cnt_stream.str().copy(content, len, 0);
    content[len] = '\0';

    return 0;
}

int OperationSend::doOperation() {

    // Check if all parameters are set
    if(receiver == NULL || strcmp(receiver, "") == 0 ||
            subject == NULL || strcmp(subject, "") == 0 ||
            content == NULL || strcmp(content, "") == 0){
        return 1;
    }

    // Create filename
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    std::string filename = std::to_string(ms);

    std::stringstream dirpath;

    // Check if directory exists
    dirpath << mailspooldir << "/" << receiver;
    DIR* dir = opendir(dirpath.str().c_str());
    if(!dir){
        // Create directory if it doesn't exist yet
        mkdir(dirpath.str().c_str(), 0777); // TODO change 0777
    }
    closedir(dir);

    // Add filename to path
    dirpath << "/" << filename << ".txt";

    // Open file and write data into it
    std::ofstream file;
    file.open(dirpath.str().c_str(), std::ios::out);
    file << "Sender: " << clientHandler->getUsername() << std::endl;
    file << "Subject: " << subject << std::endl;
    file << "Content: " << content << std::endl;

    file.close();

    return 0;
}

OperationSend::~OperationSend() {
    delete[] receiver;
    delete[] subject;
    delete[] content;
}