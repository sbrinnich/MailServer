#include <sys/time.h>
#include <thread>

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

    char* attachment = new char [8];
    send(clientsocket, "Add file attachment?(y/n) ", strlen("Add file attachment?(y/n) "), 0);
    ret = getClientInput(8, &attachment);
    if(ret == 1 || ret == -1){
        delete [] attachment;
        return ret;
    }
    if(strcmp(attachment, "y") == 0){
        fileattached = true;
    }else{
        fileattached = false;
    }

    delete [] attachment;
    return 0;
}


int OperationSend::sendFileAttachment(std::string filename) {
    send(clientsocket, "Which file do you want to add? ", strlen("Which file do you want to add? "), 0);

    char* localfile = new char[MAXLINE];
    int ret = getClientInput(MAXLINE, &localfile);
    if (ret == 1 || ret == -1) {
        delete[] localfile;
        return ret;
    }

    // Send Press enter to continue to client
    send(clientsocket, "Press enter to continue", strlen("Press enter to continue"), 0);


    // Get filesize from client
    char* FileSizeChar = new char[MAXLINE];
    ret = getClientInput(MAXLINE, &FileSizeChar);
    if (ret == 1 || ret == -1) {
        delete[] FileSizeChar;
        delete[] localfile;
        return ret;
    }
    long filesize = 0;
    std::stringstream filesizestream(FileSizeChar);
    filesizestream >> filesize;
    if(filesizestream.fail()){
        perror("Could not parse filesize in sending attachment!");
        delete[] FileSizeChar;
        delete[] localfile;
        return 1;
    }

    // Save fileinfo
    std::string infoname = filename + "info";
    std::ofstream fileinfo;
    fileinfo.open(infoname.c_str());
    if(!fileinfo.is_open()){
        perror("Could not save attachment info!");

        delete[] FileSizeChar;
        delete[] localfile;
        return 1;
    }

    fileinfo << localfile << std::endl;
    fileinfo << FileSizeChar << std::endl;
    fileinfo.close();

    // Save file
    std::string name = filename + "file";
    std::ofstream file;
    file.open(name.c_str(), std::ios::out | std::ios::binary);
    if(!file.is_open()){
        perror("Could not save attachment!");

        delete[] FileSizeChar;
        delete[] localfile;

        // Delete fileinfo file
        if(std::remove(infoname.c_str()) != 0){
            perror("Could not delete unused attachment info file");
        }
        return 1;
    }

    char* readBuffer = new char[MAXMSG];
    long read_size = 0;
    do{
        // Send OK to client
        send(clientsocket, "OK", strlen("OK"), 0);
        std::fill(readBuffer, readBuffer + sizeof(readBuffer), 0);

        // Receive from client
        if(filesize-read_size > MAXMSG-1){
            recv(clientsocket, readBuffer, MAXMSG-1, 0);
            file.write(readBuffer, MAXMSG-1);
            read_size += MAXMSG-1;
        }else{
            recv(clientsocket, readBuffer, filesize-read_size, 0);
            file.write(readBuffer, filesize-read_size);
            break;
        }
    }while(true);

    file.close();

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
    std::stringstream filename;
    filename << std::to_string(ms) << "_" << std::this_thread::get_id();

    std::stringstream dirpath;

    // Check if directory exists
    dirpath << mailspooldir << "/" << receiver;

    DIR* dir = opendir(dirpath.str().c_str());
    if(!dir){
        // Create directory if it doesn't exist yet
        mkdir(dirpath.str().c_str(), 0755);
    }
    closedir(dir);

    // Add filename to path
    dirpath << "/" << filename.str();

    // Open file and write data into it
    std::string filepath = dirpath.str()+".txt";
    std::ofstream file;
    file.open(filepath.c_str(), std::ios::out);
    file << "Sender: " << clientHandler->getUsername() << std::endl;
    file << "Subject: " << subject << std::endl;
    file << "Content: " << content << std::endl;

    file.close();

    //handle attachment
    if(fileattached){
        filepath = dirpath.str()+"_attach";
        int ret = sendFileAttachment(filepath);
        if(ret == 1 || ret == -1){
            send(clientsocket, "", strlen(""), 0);
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
