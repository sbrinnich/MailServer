#ifndef MAILSERVER_OPERATION_SEND_H
#define MAILSERVER_OPERATION_SEND_H

#include "../operation.h"

class OperationSend : public Operation {


private:
    char *receiver, *subject, *content;
    bool fileattached;
    /**
     * Handles the transfer of an attachment file from the client to the Mailspool-Directory on the server.
     * Saves a [MAIL_ID]_attachinfo file containing the filename and filesize of the attachment and a
     * [MAIL_ID]_attachfile file containing the file data of the attachment
     * @param filename should be a string containing "[MAIL_ID]_attach" where [MAIL_ID] is the name of the mail-file
     * @return 1 if the attachment could not be saved correctly or
     *          0 if the attachment was saved successfully
     */
    int sendFileAttachment(std::string filename);

public:
    /**
     * Creates a new instance of a send-operation
     * @param clientsocket the socket of the client requesting the operation
     * @param mailspooldir the directory where all the mails are saved to
     * @param clientHandler an instance to the ClientHandler handling this client's requests
     */
    OperationSend(int clientsocket, char* mailspooldir, ClientHandler* clientHandler);
    ~OperationSend();
    int doPreparation() override;
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_SEND_H
