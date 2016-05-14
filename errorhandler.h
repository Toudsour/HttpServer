//
// Created by 莫吉米 on 16/5/13.
//

#ifndef HTTPSERVER_ERRORHANDLER_H
#define HTTPSERVER_ERRORHANDLER_H


void ReturnError(int id,int client_socket);

void DealWith404(int client_socket);

void Error(const char * str);


#endif //HTTPSERVER_ERRORHANDLER_H
