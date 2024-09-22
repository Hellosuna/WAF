#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(){
    int cli = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(4888);
    if(connect(cli, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        printf("connect error\n");
    char buf[1024];
    int num = 100;
    memcpy(buf, &num, 4);
    if(send(cli, buf, sizeof(buf), 0) == -1)
        printf("send error\n");
    recv(cli, buf, sizeof(buf), 0);
    close(cli);
}