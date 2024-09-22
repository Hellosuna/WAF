#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "include/int_array_builder.h"
#include "include/flatbuffers_common_builder.h"
#include "../libdocker/inc/docker.h"

int init_connect(){
    int cli = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (cli == -1) {
    perror("socket error");
    return -1;
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(4888);//4888
    if(connect(cli, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        printf("connect error\n");
    return cli;
}

char* sum_of_array_buf(int* len){
    flatcc_builder_t builder, *B;
    B = &builder;
    int size;
    flatcc_builder_init(B);
    int arr[] = {0,1,2,3,4,5,6,7,8,9,10,10000,32498};
    flatbuffers_int32_vec_ref_t values = flatbuffers_int32_vec_create(B, arr, sizeof(arr)/sizeof(int));
    IntArray_create_as_root(B, values);
    char* flatbuf = flatcc_builder_finalize_buffer(B, &size);

    char* buf = (char*)malloc(4 + size);
    memcpy(buf, &size, 4);
    memcpy(buf+4, flatbuf, size);
    *len = 4 + size;
    return buf;
}

int main(){
    // run docker
    // 1. system is block interface
    // int status = system("docker run --network host sum_of_array");
    // if(status){
    //     printf("status: %d\n", status);
    //     perror("system return error");
    // }
    // 2. use docker restful api
    DOCKER *docker=docker_init("v1.43");
    
    CURLcode response;
    if(docker){
        printf("docker run sum_of_array\n");
        char* url = "http://localhost/v1.43/containers/create";   // create container
        char* data = "{ \"Hostname\": \"sum_of_array\", \"image\": \"sum_of_array\", \"NetworkMode\":\"host\"}";  // networkmode : host
        response = docker_post(docker, url, data);
        //printf("start container failed with status: %d\n", response);
        if(response == CURLE_OK)printf("response: %s\n", docker_buffer(docker));
        int idx;
        for(int i=0; i<docker->buffer->size; ++i){
            if(docker->buffer->data[i] == 'I' && docker->buffer->data[i+1] == 'd'){
                idx = i;
                break;
            }
        }
        char id[65];
        memcpy(id, docker->buffer->data+idx+5, sizeof(id)-1);
        id[10] = '\0';
        char start_ulr[256] = {'\0'};
        strcat(start_ulr, "http://localhost/v1.43/containers/");
        printf("id: %s\n", id);
        strcat(start_ulr, id);
        strcat(start_ulr, "/start\0");
        printf("start container: %s url size: %d\n", start_ulr, strlen(start_ulr));
        data = "{}";
        response = docker_post(docker, start_ulr,data);
        if(response == CURLE_OK)
            printf("response: %s\n", docker_buffer(docker));
        else {
            printf("response status: %d\n", response);
            perror("ERROR: Failed to get get a docker client!");
        }
        docker_destroy(docker);
    }

    
    int cli = init_connect();
    int len = 0;
    char* send_buf = sum_of_array_buf(&len);
    char recv_buf[128];
    for(int i=0; i<5; ++i){
        if(send(cli, send_buf, len, 0) == -1)
            printf("send error\n");
        recv(cli, recv_buf, len, 0);
        int sum;
        memcpy(&sum, recv_buf, 4);
        printf("sum :%d\n", sum);
    }
    close(cli);
    free(send_buf);
}