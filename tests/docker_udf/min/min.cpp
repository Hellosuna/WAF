#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/long_array_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/double_array_generated.h"
#include<time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

extern "C" {
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
/*void write_to_file(const char* file_path, char* buffer, size_t size, sem_t* sem_docker) {
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

    // 增加信号量，通知读进程写操作完成
    if (sem_post(sem_docker) == -1) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }

    printf("docker Write completed.\n");
}

void read_from_file(const char* file_path, char* buffer, size_t size, sem_t* sem_pg) {
    // 等待写进程写操作完成
    if (sem_wait(sem_pg) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (read(fd, buffer, size) == -1) {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    unlink(file_path);  // 删除文件
}
*/
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
       
        auto longArr = GetDArray(buf+24);
        auto value = longArr->values();
        auto size = value->size();
        clock_gettime(id, &te);
        int64_t *time=(int64_t *)send_buf;
        time[2]=(ts.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + ts.tv_nsec - tc->tv_nsec;//第一次copy时间
        time[0]=(te.tv_sec - ts.tv_sec) * 1000 * 1000 * 1000 + te.tv_nsec - ts.tv_nsec;
        clock_gettime(id, &ts);
        double* sum=(double*)buf;
        for (int i = 0; i < size; ++i) {
            if((value->Get(i))<sum[2])sum[2]=value->Get(i);
        }
        clock_gettime(id, &te);
        time[1]=(te.tv_sec - ts.tv_sec) * 1000 * 1000 * 1000 + te.tv_nsec - ts.tv_nsec;
        struct timespec *td=(struct timespec *)(send_buf+24);
        clock_gettime(id, td);
        double* x=(double*)send_buf;
        x[5]=sum[2];
        // 写入buffer到docker_to_host
        write_to_file(docker_to_host_path,docker_to_host_signal, send_buf, 48);
        //printf("Buffer sent to Host and sum is %d.\n",sum);
    }
    printf("can run here!\n");
    close(fd1);
    unlink(end_path);
    //unlink(host_to_docker_path);
    //unlink(docker_to_host_path);
    
}
}