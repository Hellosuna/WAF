#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/long_array_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/pair_array_generated.h"
#include<time.h>
extern "C" {
typedef struct {
    int64_t first;
    int64_t second;
} couple;
int main(){
 // listen socket
    struct timespec ts,te;
    clock_t id = CLOCK_MONOTONIC;
    
    int fd = socket(AF_INET, SOCK_STREAM,0);   // IPPROTO_TCP
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    // cli_addr.sin_family = AF_INET;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(4888);//4888
    if(bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        printf("bind error\n");
    if(listen(fd, 20) == -1)
        printf("listen error\n");

    socklen_t len = 0;
    int conn = accept(fd, (struct sockaddr*)&cli_addr, &len);
    if(conn == -1){
        perror("connect error: ");
    }
    char buf[1024]={'\0'};
    char send_buf[4096]={'\0'};
    int recv_bytes = 0;
    while((recv_bytes = recv(conn, buf, sizeof(buf), 0)) > 0){
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
        //printf("buflen is %d\n",buflen);
        clock_gettime(id, (struct timespec *)(send_buf+32));
        memcpy(send_buf+48,return_buf,buflen);
        if(send(conn, send_buf, buflen+48, 0) == -1){
            perror("send error: ");
        }
    }
    
    
}
}