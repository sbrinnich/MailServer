#include "operation_read.h"

OperationRead::OperationRead(int clientsocket, char* mailspooldir, ClientHandler* clientHandler) :
        Operation(clientsocket, mailspooldir, clientHandler) {
}

int OperationRead::doPreparation() {
    if(clientHandler->getUsername() == nullptr){
        send(clientsocket, "Please login to use this command!\n", strlen("Please login to use this command!\n"), 0);
        return 1;
    }
    return 0;
}

int OperationRead::parseRequest() {
    int ret = 0;

    char *nr;
    nr = new char[5];
    send(clientsocket, "You chose to read an email. Please enter the following data.\nMessage Number: ",
         strlen("You chose to read an email. Please enter the following data.\nMessage Number: "), 0);
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

int OperationRead::doOperation() {
    if(messagenr <= 0){
        return 1;
    }

    // Get filepath
    std::stringstream filepath;
    filepath << mailspooldir << "/" << clientHandler->getUsername();
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
    out << "Reading Mail...\nSender: " << sender << "\nReceiver: " << clientHandler->getUsername() <<
        "\nSubject: " << subject << "\nContent: " << content << "\n";

    send(clientsocket, out.str().c_str(), strlen(out.str().c_str()), 0);

    // Receive OK from client
    auto tmp_buf = new char[10];
    recv(clientsocket, tmp_buf, 10, 0);

    // Check if read file has attachment
    std::string fname = filename;
    fname.erase(fname.length()-4);

    std::stringstream filepathAttachInfo;
    filepathAttachInfo << mailspooldir << "/" << clientHandler->getUsername() << "/" << fname << "_attachinfo";

    std::ifstream fileAttachInfo;
    fileAttachInfo.open(filepathAttachInfo.str().c_str());
    if(fileAttachInfo.is_open()){
        auto question = new char[MAXLINE];
        send(clientsocket, "This mail has an attachment. Do you want to download it now? (y/n) ",
            strlen("This mail has an attachment. Do you want to download it now? (y/n) "), 0);
        int ret = getClientInput(MAXLINE, &question);
        if(ret == -1 || ret == 1){
            fileAttachInfo.close();
            delete[] sender;
            delete[] subject;
            delete[] content;
            delete[] tmp_buf;
            return ret;
        }
        if(strcmp(question, "y") == 0){
            // Send downloading info to client
            send(clientsocket, "Downloading file...", strlen("Downloading file..."), 0);

            // Receive OK from client
            std::fill(tmp_buf, tmp_buf + sizeof(tmp_buf), 0);
            recv(clientsocket, tmp_buf, 10, 0);

            // Read filename and size from info file
            std::string info_name, info_size;
            getline(fileAttachInfo, info_name);
            getline(fileAttachInfo, info_size);

            fileAttachInfo.close();

            // Parse filesize
            long filesize;
            std::stringstream sizeStream(info_size);
            sizeStream >> filesize;
            if(sizeStream.fail()){
                perror("Could not parse file size");
                delete[] sender;
                delete[] subject;
                delete[] content;
                delete[] tmp_buf;
                return 1;
            }

            // Send filename to client
            send(clientsocket, info_name.c_str(), strlen(info_name.c_str()), 0);

            // Receive OK from client
            std::fill(tmp_buf, tmp_buf + sizeof(tmp_buf), 0);
            recv(clientsocket, tmp_buf, 10, 0);

            // Send filesize to client
            send(clientsocket, info_size.c_str(), strlen(info_size.c_str()), 0);

            // Get path for attachment file
            std::stringstream filepathAttachFile;
            filepathAttachFile << mailspooldir << "/" << clientHandler->getUsername() << "/" << fname << "_attachfile";

            // Open file
            std::ifstream fileAttach;
            fileAttach.open(filepathAttachFile.str().c_str());
            if(fileAttach.is_open()){
                // Read file and send
                auto readBuffer = new char[MAXMSG];
                long read_size = 0;
                do{
                    // Receive OK from client
                    std::fill(tmp_buf, tmp_buf + sizeof(tmp_buf), 0);
                    recv(clientsocket, tmp_buf, 10, 0);
                    std::fill(readBuffer, readBuffer + sizeof(readBuffer), 0);

                    // Send to client
                    if(filesize-read_size > MAXMSG-1){
                        fileAttach.read(readBuffer, MAXMSG-1);
                        send(clientsocket, readBuffer, MAXMSG-1, 0);
                        read_size += MAXMSG-1;
                    }else{
                        fileAttach.read(readBuffer, filesize-read_size);
                        send(clientsocket, readBuffer, filesize-read_size, 0);
                        break;
                    }
                }while(true);

                fileAttach.close();
                delete[] readBuffer;
            }else{
                perror("Could not open attachment file");
                delete[] sender;
                delete[] subject;
                delete[] content;
                delete[] tmp_buf;
                return 1;
            }

        }
    }

    // Delete allocations
    delete[] sender;
    delete[] subject;
    delete[] content;
    delete[] tmp_buf;
    return 0;
}