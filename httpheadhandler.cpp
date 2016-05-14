//
// Created by 莫吉米 on 16/5/13.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "httpheadhandler.h"
#include "errorhandler.h"


static bool IsUpperAlphabet(char c)
{
    return c >= 'A' && c <= 'Z';
}


static bool IsNumber(char c)
{
    return c >= '0' && c <='9';
}


static bool IsEnd(char c)
{
    return c == '\r' || c == '\n';
}


http_request* CreateRequest()
{
    http_request *newrequest = (http_request *)malloc(sizeof(http_request));

    if(newrequest != NULL)
        Error("Can't malloc request!\n");

    memset(newrequest,0,sizeof(http_request));

    newrequest->buf = (char *) malloc(HTTP_REQUEST_BUF_MAX_LENGTH);

    return newrequest;
}


void DestroyRequest(http_request *request)
{
    if(request == NULL)
        return;

    free(request->buf);

    free(request);
}


int HandleRequest(http_request *request, char *buf)
{
    if(IsUpperAlphabet(*buf)||*buf == '_')
        return GetMethod(request, buf);
    else
        return 400;
}


int GetMethod(http_request *request, char *buf)
{
    request->head.method = buf;

    while(IsUpperAlphabet(*buf) || *buf == '_')
        buf++;

    if(*buf != ' ')
        return 400;

    *buf = 0;
    buf++;

    return GetUrl(request, buf);
}


int GetUrl(http_request *request,char *buf)
{
    char * url = buf;
    while(*buf && *buf != ' ' && *buf != '\r' && *buf != '\n')
        buf++;

    if(* buf != ' ')
        return 400;
    buf = 0;

    buf++;

    return GetVersion(request, buf);
}


int GetVersion(http_request *request, char *buf)
{
    char * version = buf;
    if(IsEnd(*buf))
        return EndHead(request,buf);
    if(*buf != 'H')
        return 400;
    buf++;

    if(*buf != 'T')
        return 400;
     buf++;

    if(*buf != 'T')
        return 400;
    buf++;

    if(*buf != 'P')
        return 400;
    buf++;

    if(!IsNumber(*buf))
        return 400;
    buf++;

    if(*buf != '.')
        return 400;

    if(!IsNumber(*buf))
        return 400;
    buf++;

    if(*buf == ' ')
    {
        *buf = 0;
        buf++;
    }
    request->head.version = version;

    if(IsEnd(*buf))
        return EndHead(request, buf);
    else
        return 400;
}


int EndHead(http_request *request, char *buf)
{
    if(*buf == '\r')
    {
        *buf = 0;
        buf++;
    }

    if(*buf != '\n')
        return 400;

    *buf = 0;
    buf++;

    return GetLine(request, buf);
}


int GetLine(http_request *request, char *buf)
{
    if(IsEnd(*buf))
        return End(request, buf);
    return GetLineName(request, buf);
}


int GetLineName(http_request *request,char *buf)
{
    int linecount = request->line_count;
    request->line[linecount].name = buf;
    while(*buf && *buf != ':')
        buf++;

    if(!(*buf))
        return 400;

    buf++;
    if(*buf != '\n')
        return 400;

    return GetLineContent(request, buf);

}


int GetLineContent(http_request *request,char *buf)
{
    int linecount = request->line_count;

    request->line[linecount].content = buf;

    while(*buf && (!IsEnd(*buf)))
        buf++;

    request->line_count++;

    if(*buf)
        return NextLine(request,buf);
    else
        return 400;
}


int NextLine(http_request *request, char *buf)
{
    if(*buf == '\r')
    {
        *buf = 0;
        buf++;
    }
    if(*buf != '\n')
        return 400;

    *buf = 0;
    buf++;


    return GetLine(request, buf);
}

int End(http_request *request, char *buf)
{
    if(*buf == '\r')
    {
        *buf = 0;
        buf++;
    }

    if(*buf != '\n')
        return 400;

    *buf=0;
    buf++;

    return 0;
}