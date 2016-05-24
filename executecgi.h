//
// Created by 莫吉米 on 16/5/15.
//

#ifndef HTTPSERVER_EXECUTECGI_H
#define HTTPSERVER_EXECUTECGI_H

const int EXTRA_SIZE = 40;

const int MAX_ENVIRONMENT_LENGTH = 256;

extern int  GetContent(int client_socket, char * buf,size_t max_len);

void HandlePost(int client_socket, http_request *request);

void GetPostContent(int client_socket, http_request * request);

extern void GetDir(char *dir);
#endif //HTTPSERVER_EXECUTECGI_H
