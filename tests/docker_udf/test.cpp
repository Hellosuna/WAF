#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/long_array_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/pair_array_generated.h"
#include "../libdocker/inc/docker.h"
extern "C" {
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
        char* data = "{ \"Hostname\": \"vkernel\", \"image\": \"sum\", \"NetworkMode\":\"host\"}";  // networkmode : host
        response = docker_post(docker, url, data);
        printf("start container failed with status: %d\n", response);
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
    int64_t arr[] = {0,1,2,3,4,5,6,7,8,9,10,10000,32498};
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arr,13);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf =builder.GetBufferPointer();
    int len=builder.GetSize();
    printf("len is %d\n",len);
    char recv_buf[4096];
    for(int i=0; i<1; ++i){
        if(send(cli, buf, len, 0) == -1)
            printf("send error\n");
        recv(cli, recv_buf, sizeof(recv_buf), 0);
        const PairArray *pair_array =GetPairArray(recv_buf);
        //auto pairs = pair_array->pairs();
        auto value = pair_array->pairs();
        auto size = value->size();
        printf("len is %d\n",size);
        for (int i=0;i<size;i++) {
            printf("(%lld, %lld)\n", value->Get(i)->first(), value->Get(i)->second());
        }
    }
    close(cli);
    free(recv_buf);
}

}