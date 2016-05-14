//
// Created by 莫吉米 on 16/5/13.
//

#ifndef HTTPSERVER_HTTPHEADHANDLER_H
#define HTTPSERVER_HTTPHEADHANDLER_H
//const int SCHEME_MAX_LENGTH = 10;
//const int ADDRESS_MAX_LENGTH = 20;
//const int PORT_MAX_LENGHT = 10;
//const int RESOURCE_PATH_MAX_LENGTH = 100;
//const int

const unsigned int HTTP_REQUEST_LINE_MAX_COUNT = 10;
const unsigned int HTTP_REQUEST_BUF_MAX_LENGTH = 8192;
struct http_request_url
{
    char *scheme;
    char *host;
    char *port;
    char *path;
    char *query_str;
};
struct http_request_head
{
    char              *method;
    http_request_url  url     ;
    char              *version;
};
struct http_request_line
{
    char *name;
    char *content;

};
struct http_request
{
    char * buf;
    http_request_head head;
    int line_count;
    http_request_line line[HTTP_REQUEST_LINE_MAX_COUNT];
};

http_request* CreateRequest();

void DestroyRequest(http_request * request);

int End(http_request * request, char *buf);

int NextLine(http_request * request, char *buf);

int HandleRequest(http_request *request);

int GetMethod(http_request * request, char * buf);

int GetUrl(http_request * request,char * buf);

int GetLineName(http_request *request,char *buf);

int GetLineContent(http_request *request,char *buf);

int GetLine(http_request *request, char *buf);

int GetVersion(http_request *request, char *buf);

int EndHead(http_request *request, char *buf);

void RequestDebug(http_request *request);

int GetPath(http_request *request, char *buf);

int GetScheme(http_request *request, char *buf);

int GetHost(http_request *request, char *buf);

int GetPort(http_request *request, char *buf);

int GetQueryStr(http_request *request, char *buf);

int SpaceBeforeVersion(http_request *request, char *buf);

#endif //HTTPSERVER_HTTPHEADHANDLER_H
