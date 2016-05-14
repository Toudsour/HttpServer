#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "errorhandler.h"
#include "httpheadhandler.h"


int CreateServer(int& host)
{
    int server_socket;
    struct sockaddr_in sock_info;
    server_socket=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket==-1)
        perror("Create Socket Failed!\n");
    memset(&sock_info,0,sizeof(sock_info));
    sock_info.sin_family=AF_INET;
    sock_info.sin_port=htons(host);
    sock_info.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(server_socket,(sockaddr*)&sock_info,sizeof(sock_info))<0)
        perror("Socket Bind Failed\n");
    if(host == 0)
    {
        socklen_t sock_info_len=sizeof(sock_info);
        if(getsockname(server_socket,(sockaddr*)&sock_info,&sock_info_len)==-1)
            perror("Get Socket Info Failed\n");
        host=ntohs(sock_info.sin_port);
    }
    if(listen(server_socket,5) < 0)
        perror("Listen Failed\n");
    return server_socket;
}


int  GetRequest(int client_socket, char * buf,size_t max_len)
{
    ssize_t num_bytes=0;
    ssize_t num_bytes_recv=0;
    while(!strstr(buf,"\r\n\r\n")&&!strstr(buf,"\n\n"))
    {
        num_bytes = recv(client_socket, buf, max_len - num_bytes_recv, 0);
        if (num_bytes == -1)
            return -1;
        num_bytes_recv += num_bytes;
        if (num_bytes_recv >= max_len)
            return -2;
    }
    buf[num_bytes_recv]='\0';
    printf("%s",buf);
    return 0;
}


int FindFile(const char * file_path)
{


}


void * StarServer(void * client)
{
    http_request * request = CreateRequest();

    int client_socket = *(int *)client;

    int server_status = GetRequest(client_socket, request->buf, HTTP_REQUEST_BUF_MAX_LENGTH);

    if(server_status)
    {
        if(server_status==-1)
            ReturnError(400,client_socket);
        if(server_status==-2)
            ReturnError(414,client_socket);
    }
    else
    {
        HandleRequest(request);
        RequestDebug(request);
    }
    ReturnError(404,client_socket);
    close(client_socket);
    return NULL;
}


int main(int argc,char * argv[])
{
    int server_socket=-1;
    int server_port=0;

    if(argc==2)
        server_port=atoi(argv[1]);

    //init server socket
    server_socket = CreateServer(server_port);
    printf("The Server Bind Ports: %d\n",server_port);

    int client_socket=-1;
    struct sockaddr_in client_info;
    socklen_t client_info_len=sizeof(client_info);

    //get client accept
    while((client_socket=accept(server_socket,(sockaddr*)&client_info,&client_info_len))!=-1)
    {
        if(client_socket==-1)
            perror("Accept Failed\n");
        pthread_t thread_id;
        if(pthread_create(&thread_id,NULL,StarServer,(void*) &client_socket)!=0)
           perror("Thread Create Failed\n");
    }
    close(server_port);
}