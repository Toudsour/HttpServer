//
// Created by 莫吉米 on 16/5/13.
//
#include "errorhandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>


void ReturnError(int id,int client_socket)
{
    switch(id)
    {
        case 404:
            DealWith404(client_socket);
            break;
        default:
            break;

    }

}


void DealWith404(int client_socket)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client_socket, buf, strlen(buf), 0);
    sprintf(buf, "Server: jdbhttpd/0.1.0\r\n");
    send(client_socket, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client_socket, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client_socket, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>404 Not Found</TITLE>\r\n");
    send(client_socket, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>Hello!</P>\r\n");
    send(client_socket, buf, strlen(buf), 0);
    sprintf(buf, "<P>your request because the resource specified</P>\r\n");
    send(client_socket, buf, strlen(buf), 0);
    sprintf(buf, "<P>is unavailable or nonexistent.</P>\r\n");
    send(client_socket, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client_socket, buf, strlen(buf), 0);

}

void Error(const char * str)
{
    perror(str);
    exit(EXIT_FAILURE);
}
