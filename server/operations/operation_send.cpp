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

    char * attachment = new char [8];
    send(clientsocket, "Add file attachment?(y/n) ", strlen("Add file attachment?(y/n) "), 0);
    ret = getClientInput(8, &attachment);
    if(ret == 1 || ret == -1){
        delete [] attachment;
        return ret;
    }
    if(strcmp(attachment, "y") == 0){
        fileattached = true;
        delete [] attachment;
    }
    delete [] attachment;
    return 0;
}


int OperationSend::sendFileAttachment(std::string filename) {

    //todo save file from dir
    //todo list name of file attachment
    //todo delete attachment

    send(clientsocket, "Which file do you want to add? ", strlen("Which file do you want to add? "), 0);

    auto * localfile = new char [MAXLINE];
    int ret = getClientInput(MAXLINE, &localfile);
    if (ret == 1 || ret == -1) {
        delete[] localfile;
        return ret;
    }

    send(clientsocket, "Opening file...", strlen("Opening file..."), 0);

    //get filesize from client
    char * FileSizeChar = new char[MAXLINE];
    ret = getClientInput(MAXLINE, &FileSizeChar);
    char *end;
    auto FileSize = static_cast<size_t>(strtol(FileSizeChar,&end,10));
    if (ret == 1 || ret == -1) {
        delete[] FileSizeChar;
        return ret;
    }

    //adds the attachment name from client to filename of mail
    filename += "_";
    filename += localfile;

    //convert string to char array
    const char *filenamefin = filename.c_str();

    //save file at server
    FILE *file = fopen(filenamefin, "w");//creates empty file to write into
    char* copyhelper;
    long SizeCheck = 0;
    copyhelper = (char*)malloc(FileSize + 1);
    while(SizeCheck < FileSize){
        ssize_t Received = recv(clientsocket, copyhelper, FileSize, 0);
        ssize_t  Written = fwrite(copyhelper, sizeof(char), static_cast<size_t>(Received), file);
        SizeCheck += Written;
        for(int i = 0; i < Written; i++){
            if(copyhelper[i] == '\n'){
                SizeCheck += 1;//because \n is 2 byte
            }
        }
    }
    fclose(file);
    free(copyhelper);
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

    //handle attachment
    if(fileattached){
        std::stringstream attachment;
        attachment << mailspooldir << "/" << receiver;
        dir = opendir(attachment.str().c_str());
        int ret = sendFileAttachment(filename);
        closedir(dir);
        if(ret == 1 || ret == -1){
            return ret;
        }
    }


    return 0;
}

OperationSend::~OperationSend() {
    delete[] receiver;
    delete[] subject;
    delete[] content;
}
