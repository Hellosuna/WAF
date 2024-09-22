#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "../include/int_array_reader.h"
#include<time.h>
int main(){
 // listen socket
    struct timespec ts,te;
    clock_t id = CLOCK_MONOTONIC;
    clock_gettime(id, &ts);
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

    int len = 0;
    int conn = accept(fd, (struct sockaddr*)&cli_addr, &len);
    if(conn == -1){
        perror("connect error: ");
    }

    int nbytes = 0;
    char buf[1024];
    int recv_bytes = 0;
    while((recv_bytes = recv(conn, buf+nbytes, sizeof(buf), 0)) > 0){
        nbytes += recv_bytes;
        if(nbytes >= 4){
            int num;
            memcpy(&num, buf, 4);
            // printf("num: %d nbytes: %d\n", num, nbytes);
            if(num + 4 == nbytes){  // data ready
                IntArray_table_t intArr = IntArray_as_root(buf+4);
                flatbuffers_int32_vec_t values = IntArray_values(intArr);
                size_t len = flatbuffers_int32_vec_len(values);
                int sum = 0;
                for(int i=0; i<len; ++i){
                    sum += flatbuffers_int32_vec_at(values, i);
                }
                memcpy(buf, &sum, sizeof(sum));
                // printf("calculate sum: %d\n", sum);
                if(send(conn, buf, sizeof(sum), 0) == -1){
                    perror("send error: ");
                }
                recv_bytes = 0;
                nbytes = 0;
            }

        }

    }
    clock_gettime(id, &te);
        // record time interval
    printf("%ld ns\n", (te.tv_sec - ts.tv_sec) * 1000 * 1000 * 1000 + te.tv_nsec - ts.tv_nsec);
    
}