//
// Created by 莫吉米 on 16/5/15.
//

#ifndef HTTPSERVER_SENDPAGE_H
#define HTTPSERVER_SENDPAGE_H

const size_t MAX_DIR_LENGTH = 1024;

const size_t MAX_BUF_SIZE = 1024;

const char SERVER_NAME[] = "Httpserver-1.0";

const char SOURCE_ROOT[] = "Source";

const char EXCEL_ROOT[] = "Excel";

void ChangeRoot();

int IsFile(char *);

void HandleRequest(int socket_client, http_request *request);

void SendPage(int socket_client, char * file);

#endif //HTTPSERVER_SENDPAGE_H
