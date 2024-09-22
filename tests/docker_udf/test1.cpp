#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/long_array_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/pair_array_generated.h"
#include "../libdocker/inc/docker.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
extern "C" {
void write_to_file(const char* file_path, char* buffer, size_t size) {
    int fd = open(file_path, O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (write(fd, buffer, size) == -1) {
        perror("write");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void read_from_file(const char* file_path, char* buffer, size_t size) {
    int fd;
    while ((fd = open(file_path, O_RDONLY)) == -1) {
        usleep(1);  // Sleep for 1us
    }
    if (read(fd, buffer, size) == -1) {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    unlink(file_path);  // 删除文件
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
        char* data = R"({
        "Hostname": "vkernel",
        "Image": "sum",
        "NetworkMode": "host",
        "HostConfig": {
            "Binds": ["/mnt/ramdisk_segment_0:/mnt/ramdisk"]
        }
    })";
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
    for(int i=0; i<2; ++i){
        const char* host_to_docker_path = "/mnt/ramdisk/host_to_docker";
        const char* docker_to_host_path = "/mnt/ramdisk/docker_to_host";
        // 写入buffer到host_to_docker
        write_to_file(host_to_docker_path, (char *)buf, len);
        printf("Buffer sent to Docker.\n");
        
         // 从docker_to_host读取buffer
        read_from_file(docker_to_host_path, recv_buf, sizeof(recv_buf));
        printf("Buffer received from Docker.\n");
        const PairArray *pair_array =GetPairArray(recv_buf);
        //auto pairs = pair_array->pairs();
        auto value = pair_array->pairs();
        auto size = value->size();
        printf("len is %d\n",size);
        for (int i=0;i<size;i++) {
            printf("(%lld, %lld)\n", value->Get(i)->first(), value->Get(i)->second());
        }
    }
    const char* end_path = "/mnt/ramdisk/end";
    int fd1=open(end_path, O_WRONLY | O_CREAT, 0666);
    free(recv_buf);
}

}


