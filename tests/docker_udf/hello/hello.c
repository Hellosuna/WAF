#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
int main(){
    printf("hello docker udf\n");
    // listen socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);   // IPPROTO_TCP
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    // cli_addr.sin_family = AF_INET;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(4888);
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
    while(nbytes += recv(conn, buf+nbytes, sizeof(buf), 0)){
        if(nbytes == -1)
            break;
        if(nbytes >= 4){
            int num;
            memcpy(&num, buf, 4);
            printf("num: %d\n", num);
            if(num + 4 == nbytes){  // data ready

            }
        }

    }
}