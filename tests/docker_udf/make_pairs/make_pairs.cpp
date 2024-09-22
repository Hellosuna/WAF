#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/long_array_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/pair_array_generated.h"
#include<time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
extern "C" {
typedef struct {
    int64_t first;
    int64_t second;
} couple;
void write_to_file(const char* file_path,const char* docker_to_host_signal, char* buffer, size_t size) {
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
    if(close(fd)==-1)
    {
       perror("close");
       printf("close docker_to_host failed\n");
       exit(EXIT_FAILURE);
    }
    int fd1 = open(docker_to_host_signal, O_WRONLY | O_CREAT, 0666);
    if (fd1 == -1) {
        perror("open");
        //close(fd1);
        exit(EXIT_FAILURE);
    }
    if(close(fd1)==-1)
    {
        perror("close");
        printf("close docker_to_host signal failed\n");
        exit(EXIT_FAILURE);
    }
    //printf("write docker_to_host and signal successfully!\n");
}

void read_from_file(const char* file_path,const char* host_to_docker_signal, char* buffer, size_t size) {
    int fd,fd1,fd2;
    const char* end_path = "/mnt/ramdisk/end";
    while ((fd1 = open(host_to_docker_signal, O_RDONLY)) == -1){
        if((fd2 = open(end_path, O_RDONLY)) != -1)
        {
            if(close(fd2)==-1)
            {
                perror("close");
                printf("close end failed\n");
                exit(EXIT_FAILURE);
            }
            return;

        }
        usleep(1);  // Sleep for 1us
    }
    if(fd1!=-1)
    {
        if(close(fd1)==-1)
        {
            perror("close");
            printf("close host_to_docker_signal failed\n");
            exit(EXIT_FAILURE);
        }
    }
    else{
        printf("create docker_to_host signal failed\n");
    }
    if(unlink(host_to_docker_signal)==-1)
    {
        printf("delete host_to_docker_signal failed\n");
    }
    fd = open(file_path,O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (read(fd, buffer, size) == -1) {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if(close(fd)==-1)
    {
        perror("close");
        printf("close host_to_docker failed\n");
        exit(EXIT_FAILURE);
    }
    if(unlink(file_path)==-1)
    {
        printf("delete host_to_docker failed\n");
    } // 删除文件
    //printf("read host_to_docker and signal successfully!\n"); 
}
int main(){
 // listen socket
    struct timespec ts,te;
    clock_t id = CLOCK_MONOTONIC;
    char buf[1024]={'\0'};
    char send_buf[4096]={'\0'};
    int fd1;
    const char* host_to_docker_path = "/mnt/ramdisk/host_to_docker";
    const char* docker_to_host_path = "/mnt/ramdisk/docker_to_host";
    const char* host_to_docker_signal = "/mnt/ramdisk/host_to_docker_signal";
    const char* docker_to_host_signal = "/mnt/ramdisk/docker_to_host_signal";
    const char* end_path = "/mnt/ramdisk/end";
    while(true){
         // 从host_to_docker读取buffer
        read_from_file(host_to_docker_path,host_to_docker_signal, buf, sizeof(buf));
        //printf("Buffer received from Host\n");
        if((fd1 = open(end_path, O_RDONLY)) != -1)
        {
            close(fd1);
            break;
        }
        struct timespec *tc=(struct timespec *)buf;
        clock_gettime(id, &ts);
       
        auto longArr = GetLongArray(buf+16);
        auto value = longArr->values();
        auto size = value->size();
        clock_gettime(id, &te);
        int64_t *time=(int64_t *)send_buf;
        time[3]=(ts.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + ts.tv_nsec - tc->tv_nsec;//第一次copy时间
        time[0]=(te.tv_sec - ts.tv_sec) * 1000 * 1000 * 1000 + te.tv_nsec - ts.tv_nsec;
        clock_gettime(id, &ts);
        
        int len=size*(size-1)/2;
        couple *twins=(couple *)malloc(len*sizeof(couple));
        int k=0;
        for (int i = 0; i < size; ++i) {
            for (int j = i + 1; j < size; ++j) {
                twins[k].first=value->Get(i);
                twins[k].second=value->Get(j);
                //printf("(%lld, %lld)\n", value->Get(i), value->Get(j));
                k++;
            }
        }
        clock_gettime(id, &te);
        time[1]=(te.tv_sec - ts.tv_sec) * 1000 * 1000 * 1000 + te.tv_nsec - ts.tv_nsec;
        clock_gettime(id, &ts);
        flatbuffers::FlatBufferBuilder builder(1024);
        std::vector<flatbuffers::Offset<Pair>> pair_vector;
        //int len=size*(size-1)/2;
        //int k=0;
        //flatbuffers::Offset<Pair> *pair_vector=(flatbuffers::Offset<Pair> *)malloc(len*sizeof(flatbuffers::Offset<Pair>));
        //flatbuffers::Offset<Pair> pair_vector[1000];
        /*for (int i = 0; i < size; ++i) {
            for (int j = i + 1; j < size; ++j) {
                auto pair = CreatePair(builder, value->Get(i), value->Get(j));
                printf("(%lld, %lld)\n", value->Get(i), value->Get(j));
                pair_vector.push_back(pair);
                //pair_vector[k]=pair;
                //k++;
            }
        }*/
        for (int i = 0; i < len; ++i) {
            auto pair = CreatePair(builder, twins[i].first, twins[i].second);
            pair_vector.push_back(pair);
        }
        auto pairs_vec = builder.CreateVector(pair_vector);
        auto pair_array = CreatePairArray(builder, pairs_vec);
        builder.Finish(pair_array);
        uint8_t *return_buf = builder.GetBufferPointer();
        int buflen=builder.GetSize();
        clock_gettime(id, &te);
        time[2]=(te.tv_sec - ts.tv_sec) * 1000 * 1000 * 1000 + te.tv_nsec - ts.tv_nsec;
        clock_gettime(id, (struct timespec *)(send_buf+32));
        //printf("buflen is %d\n",buflen);
       
        memcpy(send_buf+48,return_buf,buflen);
        // 写入buffer到docker_to_host
        write_to_file(docker_to_host_path,docker_to_host_signal, send_buf, buflen+48);
        //printf("Buffer sent to Host.\n");
    }
    printf("can run here!\n");
    close(fd1);
    unlink(end_path);
    
}
}