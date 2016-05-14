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
    //printf("%s",buf);
    return 0;
}


char * GetRequestMethod(char ** http_request)
{
    char * buf=*http_request;
    char * method=buf;
    while(*buf&&!isspace(*buf))
        buf++;
    *buf='\0';
    *http_request=buf+1;
    return method;
}


char * GetRequestUrl(char ** http_request)
{

    char * buf=*http_request;
    while(*buf&&(isspace(*buf)))
        buf++;
    char * url= buf;
    while(*buf&&(!isspace(*buf)))
        buf++;
    *buf=0;
    *http_request=buf+1;
    return url;
}


/* Split the url into path and query
 * Check if exist query in the url
 */
int SpliteUrl(char *url ,char * url_path,char * url_query)
{

    char *path=url;
    while(*path&&isspace(*path))
        path++;
    char *query=url;
    while(*query&&*query!='?')
        query++;
    int is_exist_query=0;
    if(*query=='?')
        is_exist_query=1;
    *query='\0';
    sprintf(url_path,"hdoc%s",path);
    if(is_exist_query)
    {
        query++;
        strcpy(url_query,query);
    }
    else
        url_query[0]='\0';
    size_t len=strlen(url_path);
    if(len>0&&url_path[len-1]=='/')
        strcat(url_path,"index.html");
    return is_exist_query;
}

int FindFile(const char * file_path)
{


}

void * StarServer(void * client)
{
    /*
    char http_request[8192];
    char url_path[512];
    char url_query[512];
    int client_socket=*(int*)client;
    ssize_t http_status;
    if((http_status=GetRequest(client_socket,http_request,4096))<0)
    {
        if(http_status==-1)
            ReturnError(400,client_socket);
        if(http_status==-2)
            ReturnError(414,client_socket);
    }
    else
    {
        char *http_request_buf = http_request;

        char *http_request_method = GetRequestMethod(&http_request_buf);
        char *http_request_url = GetRequestUrl(&http_request_buf);
        memset(url_path,0,sizeof(url_path));
        memset(url_query,0,sizeof(url_query));
        if(SpliteUrl(http_request_url,url_path,url_query))
            printf("%s\n",url_query);
        printf("%s\n",url_path);
        if(strcasecmp(http_request_method,"GET")==0)
        {


        }
        else if(strcasecmp(http_request_method,"POST")==0)
        {


        }
        else
            ReturnError(302,client_socket);


    }
    ReturnError(404,client_socket);
    close(client_socket);
    return NULL;
     */
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