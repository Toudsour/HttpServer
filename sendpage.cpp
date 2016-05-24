//
// Created by 莫吉米 on 16/5/15.
//


#include <sys/file.h>
#include <pwd.h>
#include <stdio.h>
#include <zconf.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "httpheadhandler.h"
#include "errorhandler.h"
#include "sendpage.h"
#include "executecgi.h"


void GetDir(char *dir)
{
    memset(dir,0,MAX_DIR_LENGTH);
    getcwd(dir,MAX_DIR_LENGTH);
}

static void PrintDir(char *dir)
{
    GetDir(dir);
    printf("%s\n",dir);
}

void ChangeRoot()
{
    struct passwd *user_pwd;
    char current_dir[MAX_DIR_LENGTH];

    /*
    if(getuid() != 0)
    {
        Error("to ensure safe,you should run by root");
    }
    */

    user_pwd = getpwnam("nobody");
    if(user_pwd == NULL)
    {
        Error("can't get noboby's pwd");
    }

    chdir("Doc");

    PrintDir(current_dir);

    //chroot(current_dir);

    PrintDir(current_dir);
}

static bool IsDir(const struct stat *file_stat)
{
    return S_ISDIR(file_stat->st_mode);
}

static long  GetFileLen(FILE * file_handle)
{
    fseek(file_handle, 0, SEEK_END);
    return ftell(file_handle);
}


int IsFile(char *file_name)
{

    struct stat file_stat;

    if(stat(file_name, &file_stat))
        return 404;

    if(IsDir(&file_stat))
    {
        strcat(file_name,"index.html");
        if(!stat(file_name, &file_stat))
            return 0;
        else
            return 403;
    }

    else
        return 0;
}

void SendHead(int socket_client, int status_code, long file_length)
{

    char buf[MAX_BUF_SIZE];

    sprintf(buf, "HTTP/1.1 %d OK\n", status_code);
    send(socket_client, buf, strlen(buf), 0);

    sprintf(buf,"Server: %s\n",SERVER_NAME);
    send(socket_client, buf, strlen(buf), 0);

    sprintf(buf,"Content-Length: %ld\n\n", file_length);
    send(socket_client, buf, strlen(buf), 0);
}

void SendFile(int socket_client, FILE *file_handle)
{
    char buf[MAX_BUF_SIZE];

    fseek(file_handle, 0, SEEK_SET);

    while(!feof(file_handle))
    {
        fread(buf, 1, MAX_BUF_SIZE, file_handle);
        if(send(socket_client, buf, strlen(buf), 0) == -1)
            return;
    }

}
void SendPage(int client_socket,http_request *request)
{

    char file_name[MAX_DIR_LENGTH];

    GetDir(file_name);

    sprintf(file_name + strlen(file_name), "/%s%s", SOURCE_ROOT, request->head.url.path);

    printf("%s\n",file_name);
    int file_state = IsFile(file_name);

    if(file_state)
    {
        ReturnError(file_state, client_socket);
        return;
    }

    FILE* file_handle;

    file_handle = fopen(file_name,"rb");

    if(file_handle == NULL)
        return ;

    SendHead(client_socket, 200, GetFileLen(file_handle));

    SendFile(client_socket, file_handle);

    fclose(file_handle);

}


void HandleRequest(int socket_client, http_request *request)
{


    if(!strcasecmp(request->head.method,"GET"))
    {

        if(request->head.url.query_str == NULL)
            SendPage(socket_client, request);
        else
        {
            HandlePost(socket_client, request);
        }
    }
    else if(!strcasecmp(request->head.method, "POST"))
    {
        HandlePost(socket_client, request);
    }
    else
        ReturnError(400, socket_client);
}