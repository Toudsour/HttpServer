//
// Created by 莫吉米 on 16/5/15.
//

#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "string.h"

#include "httpheadhandler.h"
#include "errorhandler.h"
#include "executecgi.h"
#include "sendpage.h"

int SetEnv(http_request *request)
{
    char env_buf[MAX_ENVIRONMENT_LENGTH];

    sprintf(env_buf, "REQUEST_METHOD=%s", request->head.method);
    if(putenv(env_buf) == -1)
        return -1;

    if (strcasecmp(env_buf, "GET") == 0) {
        sprintf(env_buf, "QUERY_STRING=%s", request->head.url.query_str);
        if(putenv(env_buf))
            return -1;
    }
    else {   /* POST */
        sprintf(env_buf, "CONTENT_LENGTH=%lu", strlen(request->content));
        if(putenv(env_buf))
            return -1;
    }
    return 0;

}
void HandlePost(int client_socket, http_request *request)
{
    char * post_content;

    GetPostContent(client_socket, request);

    post_content = request->content;

    if(post_content == NULL)
        return;

    char file_name[MAX_DIR_LENGTH];

    GetDir(file_name);

    sprintf(file_name + strlen(file_name), "/%s%s.cgi", EXCEL_ROOT, request->head.url.path);

    struct stat file_stat;

    int file_state = stat(file_name, &file_stat);

    if(file_state)
        return ReturnError(404, client_socket);

    int father_to_son[2];
    int son_to_father[2];

    if(pipe(father_to_son) < 0)
        return ReturnError(500, client_socket);


    if(pipe(son_to_father) < 0)
        return ReturnError(500, client_socket);


    int pid;
    pid = fork();

    if(pid < 0)
        return ReturnError(500, client_socket);

    if(pid)
    {
        char buf[MAX_BUF_SIZE];
        close(father_to_son[0]);
        close(son_to_father[1]);

        if(strcasecmp(request->head.method, "POST"))
        {
            write(father_to_son[1],request->content,strlen(request->content));
        }

        SendHead(client_socket,200,3);

        while(read(son_to_father[0], buf, MAX_BUF_SIZE))
        {
            printf("%s",buf);
            send(client_socket, buf, MAX_BUF_SIZE, 0);
        }

        close(father_to_son[1]);
        close(son_to_father[0]);

        int status;
        waitpid(pid ,&status, 0);
        if(status)
            ReturnError(500,client_socket);
    }
    else
    {
        if(SetEnv(request))
            Error("Set Env Error\n");

        dup2(father_to_son[0], 0);
        dup2(son_to_father[1], 1);
        close(father_to_son[1]);
        close(son_to_father[0]);
        execl(file_name,file_name,NULL);
        exit(0);
    }
}

void GetPostContent(int client_socket, http_request * request)
{
    int content_length = 0;

    char * post_content;
    for(int i = 0; i < request->line_count; i++)
    {
        if(!strcasecmp(request->line[i].name, "Content-Length"))
        {
            content_length = atoi(request->line[i].content);
            break;
        }
    }

    if(content_length <= 0)
        return ReturnError(411, client_socket);


    post_content = (char *)malloc((size_t)(content_length + EXTRA_SIZE));

    if(post_content == NULL)
        return ReturnError(503, client_socket);




    char *temp_post = post_content;
    char *temp_end = request->end;
    int left_space = content_length + EXTRA_SIZE;
    while(*temp_end && left_space--)
    {
        *temp_post = *temp_end;
        temp_post++;
        temp_end++;
    }
    *temp_post = 0;

    ssize_t num_bytes = 0;

    if(strlen(post_content) < content_length)
    {
        num_bytes = recv(client_socket, temp_post, left_space, 0);
        if (num_bytes < 0)
            return ReturnError(500, client_socket);

        temp_post += num_bytes;
        *temp_post = 0;
    }

    if(strlen(post_content) != content_length)
    {
        free(post_content);
        return ReturnError(400,client_socket);
    }

    request->content = post_content;

}
