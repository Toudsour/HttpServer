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

void SendFile(int socket_client, FILE *file_handle);

int IsFile(char *file_name);

void SendHead(int socket_client, int status_code, long file_length);

void SendPage(int client_socket, http_request *file);

#endif //HTTPSERVER_SENDPAGE_H
