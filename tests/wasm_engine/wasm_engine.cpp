extern "C" {
    #include <postgres.h>
    #include <fmgr.h>
    #include <utils/builtins.h>
    #include <access/htup_details.h>
    #include <utils/typcache.h>
    #include <funcapi.h>
    #include <sys/shm.h>
    #include "utils/array.h"
    #include <sys/socket.h>
    #include <string.h>
    #include <unistd.h> 
    #include <stdlib.h>
    #include "libdocker/inc/docker.h"
    #include "wasm_export.h"
    #include "utils/lsyscache.h"
    #include <time.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <stdio.h>
    #include <sys/socket.h>
    //#include <arpa/inet.h>
    #include <errno.h>
    #include "executor/spi.h"
    #include "executor/executor.h"
    #include "utils/numeric.h"
    #include "catalog/pg_type.h"
    #include "cdb/cdbvars.h"
    #include <sys/file.h>
    #include <semaphore.h>
    extern Oid TypenameGetTypid(const char *typname);
  
    #include <netinet/in.h>
    PG_MODULE_MAGIC;

 
    PG_FUNCTION_INFO_V1(sum_share);
    PG_FUNCTION_INFO_V1(sum_native);
    PG_FUNCTION_INFO_V1(sum_with_ser);
    PG_FUNCTION_INFO_V1(sum_with_docker);
    PG_FUNCTION_INFO_V1(min_share);
    PG_FUNCTION_INFO_V1(min_with_ser);
    PG_FUNCTION_INFO_V1(min_with_docker);
    PG_FUNCTION_INFO_V1(product_share);
    PG_FUNCTION_INFO_V1(product_with_ser);
    PG_FUNCTION_INFO_V1(product_with_docker);
    PG_FUNCTION_INFO_V1(count_share);
    PG_FUNCTION_INFO_V1(count_native);
    PG_FUNCTION_INFO_V1(count_with_ser);
    PG_FUNCTION_INFO_V1(count_with_docker);
    PG_FUNCTION_INFO_V1(extract_sentiment_share);
    PG_FUNCTION_INFO_V1(extract_sentiment_native);
    PG_FUNCTION_INFO_V1(extract_sentiment_with_ser);
    PG_FUNCTION_INFO_V1(extract_sentiment_with_docker);
    PG_FUNCTION_INFO_V1(make_pairs_share);
    PG_FUNCTION_INFO_V1(make_pairs_native);
    PG_FUNCTION_INFO_V1(make_pairs_with_ser);
    PG_FUNCTION_INFO_V1(make_pairs_with_docker);
    //PG_FUNCTION_INFO_V1(print_time);

    PG_FUNCTION_INFO_V1(min_agg_init_1);
    PG_FUNCTION_INFO_V1(min_agg_trans_1);
    PG_FUNCTION_INFO_V1(min_agg_combine_1);
    PG_FUNCTION_INFO_V1(min_agg_final_1);
    
    PG_FUNCTION_INFO_V1(min_agg_init_2);
    PG_FUNCTION_INFO_V1(min_agg_trans_2);
    PG_FUNCTION_INFO_V1(min_agg_combine_2);
    PG_FUNCTION_INFO_V1(min_agg_final_2);


    PG_FUNCTION_INFO_V1(min_agg_init_3);
    PG_FUNCTION_INFO_V1(min_agg_trans_3);
    PG_FUNCTION_INFO_V1(min_agg_combine_3);
    PG_FUNCTION_INFO_V1(min_agg_final_3);

    PG_FUNCTION_INFO_V1(min_agg_init_4);
    PG_FUNCTION_INFO_V1(min_agg_trans_4);
    PG_FUNCTION_INFO_V1(min_agg_combine_4);
    PG_FUNCTION_INFO_V1(min_agg_final_4);



    PG_FUNCTION_INFO_V1(product_agg_init_1);
    PG_FUNCTION_INFO_V1(product_agg_trans_1);
    PG_FUNCTION_INFO_V1(product_agg_combine_1);
    PG_FUNCTION_INFO_V1(product_agg_final_1);

    PG_FUNCTION_INFO_V1(product_agg_init_2);
    PG_FUNCTION_INFO_V1(product_agg_trans_2);
    PG_FUNCTION_INFO_V1(product_agg_combine_2);
    PG_FUNCTION_INFO_V1(product_agg_final_2);

    PG_FUNCTION_INFO_V1(product_agg_init_3);
    PG_FUNCTION_INFO_V1(product_agg_trans_3);
    PG_FUNCTION_INFO_V1(product_agg_combine_3);
    PG_FUNCTION_INFO_V1(product_agg_final_3);

    PG_FUNCTION_INFO_V1(product_agg_init_4);
    PG_FUNCTION_INFO_V1(product_agg_trans_4);
    PG_FUNCTION_INFO_V1(product_agg_combine_4);
    PG_FUNCTION_INFO_V1(product_agg_final_4);


    typedef struct {
        int64 first;
        int64 second;
    } couple;

    
}     
#include "module_wrapper.h"
#include <unordered_map>       
#include <string>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
using namespace std;
typedef struct emo{
    char judge[5];
    char word[50];
    char sentence[3000];
}emo;
//#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/int_array_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/flat_str_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/flat_str_arr_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/date_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/point_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/long_array_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/pair_array_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/double_array_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/str_array_generated.h"






static std::unordered_map<std::string, wasm_wrapper_t*> func_map;
static std::unordered_map<std::string, int> fd_map;

long long data_trans[1000*10000] = {0};//[100*1000]
long long execution[1000*10000] = {0};//[100*1000]（除了参数解析和结果封装）UDF调用时间
long long inner_diser[1000*10000] = {0};
long long inner_exec[1000*10000] = {0};
long long inner_ser[1000*10000] = {0};
long long outer_diser[1000*10000] = {0};
long long copy1[1000*10000] = {0};//输入拷贝时间
long long copy2[1000*10000] = {0};//输出拷贝时间
long long call[1000*10000] = {0};//UDF每次调用时间
int64_t row = 0;


//内存文件写
void write_to_file(const char* file_path,const char* host_to_docker_signal, char* buffer, size_t size) {
    int fd = open(file_path, O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to open file: %s. Error: %s", file_path, error_message);
    }
    if (write(fd, buffer, size) == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        close(fd);
        elog(ERROR, "Failed to write to file: %s. Error: %s", file_path, error_message);
        exit(EXIT_FAILURE);
    }
    if(close(fd)==-1)elog(ERROR,"Row %d close host_to_docker failed!\n",row);
    int fd1 = open(host_to_docker_signal, O_WRONLY | O_CREAT, 0666);
    if (fd1 == -1) {
        //close(fd1);
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to create host_to_docker_signal: %s. Error: %s", host_to_docker_signal, error_message);
    }
    if(close(fd1)==-1)elog(ERROR,"Row %d close host_to_docker signal failed!\n",row);;
    //elog(NOTICE,"Row %d write host_to_docker and signal successfully!\n",row);
}

//内存文件读
void read_from_file(const char* file_path,const char* docker_to_host_signal, char* buffer, size_t size) {
    int fd,fd1;
    while ((fd1 = open(docker_to_host_signal, O_RDONLY)) == -1) {
        usleep(1);  // Sleep for 1us
    }
    if(fd1!=-1)
    {
        close(fd1);
        //elog(NOTICE,"Row %d open docker_to_host signal successfully!\n",row);
    }
    else elog(ERROR,"Row %d open docker_to_host signal failed!\n",row);
    if(unlink(docker_to_host_signal)==-1)
    {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Row %d delete docker_to_host signal failed! Failed to delete file: %s. Error: %s", row,docker_to_host_signal, error_message);
    }
    
    fd = open(file_path,O_RDONLY);
    if (fd == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to open file: %s. Error: %s", file_path, error_message);
    }
    if (read(fd, buffer, size) == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        perror("read");
        close(fd);
        elog(ERROR, "Failed to read file: %s. Error: %s", file_path, error_message);
        exit(EXIT_FAILURE);
    }
    if(close(fd)==-1)elog(ERROR,"Row %d close docker_to_host failed!\n",row);
    if(unlink(file_path)==-1)
    {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Row %d delete docker_to_host failed! Failed to delete file: %s. Error: %s", row,file_path, error_message);
    }
    //elog(NOTICE,"Row %d read docker_to_host and signal successfully!\n",row);
}

/*void write_to_file1(const char* file_path, char* buffer, size_t size, sem_t* sem_pg) {
    int fd = open(file_path, O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to open file: %s. Error: %s", file_path, error_message);
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (write(fd, buffer, size) == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to write to file: %s. Error: %s", file_path, error_message);
        perror("write");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    if (sem_post(sem_pg) == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to post pg semaphore: %s. Error: %s", file_path, error_message);
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
    elog(NOTICE,"pg Write completed.\n");
}

void read_from_file1(const char* file_path, char* buffer, size_t size, sem_t* sem_docker) {
    if (sem_wait(sem_docker) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
    int fd, fd1;
    const char* end_path = "/mnt/ramdisk/end";
    while ((fd = open(file_path, O_RDONLY)) == -1) {
        if ((fd1 = open(end_path, O_RDONLY)) == -1) {
            usleep(1);  // Sleep for 1us
        } else {
            close(fd1);  // Close end_path file descriptor
            return;
        }
    }
    if (read(fd, buffer, size) == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to read file: %s. Error: %s", file_path, error_message);
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    unlink(file_path);  // 删除文件
    elog(NOTICE,"get docker udf output.\n");

}
*/


/*void communicate(const char* file_path1,const char* file_path, char* buffer, size_t size, sem_t* sem_pg,sem_t* sem_docker) {
    int fd = open(file_path, O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to open file: %s. Error: %s", file_path, error_message);
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (write(fd, buffer, size) == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to write to file: %s. Error: %s", file_path, error_message);
        perror("write");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    if (sem_post(sem_pg) == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to post pg semaphore: %s. Error: %s", file_path, error_message);
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
    elog(NOTICE,"pg Write completed.\n");

    

    if (sem_wait(sem_docker) == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to wait docker semaphore: %s. Error: %s", file_path, error_message);
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
    int fd, fd1;
    const char* end_path = "/mnt/ramdisk/end";
    while ((fd = open(file_path1, O_RDONLY)) == -1) {
        if ((fd1 = open(end_path, O_RDONLY)) == -1) {
            usleep(1);  // Sleep for 1us
        } else {
            close(fd1);  // Close end_path file descriptor
            return;
        }
    }
    if (read(fd, buffer, size) == -1) {
        char *error_message = strerror(errno);
        // 使用elog报告错误并输出错误信息
        elog(ERROR, "Failed to read file: %s. Error: %s", file_path1, error_message);
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    unlink(file_path1);  // 删除文件
    elog(NOTICE,"get docker udf output.\n");

}
*/


//打印时间开销
void print_time(){
 //   elog(NOTICE, "load module timing: %ld", load);
 // elog(NOTICE, "instantiate module timing: %ld", instaniate);    
// std::string transTiming;
    elog(NOTICE,"row is %d\n",row);
    long long transTiming = 0;
    for(int i=0; i<row; ++i){
//	transTiming.append(std::to_string(data_trans[i]) + " ");
	transTiming += data_trans[i];
    }
    elog(NOTICE, "%d rows data_trans timing: %ld", row, transTiming/row);
 // elog(NOTICE, "%s", transTiming.c_str());
//    std::string execTiming;
    long long execTiming = 0;
    for(int i=0; i<row; ++i){
//	execTiming.append(std::to_string(execution[i]) + " ");
	execTiming += execution[i];
    } 
    elog(NOTICE, "%d rows execution timing: %ld", row, execTiming/row);//-1
//   elog(NOTICE, "%s", execTiming.c_str());
    long long callTiming = 0;
    for(int i=0; i<row; ++i){

	callTiming += call[i];
    } 
    elog(NOTICE, "%d rows call timing: %ld", row, callTiming/row);//-1
    long long inner_diserTiming = 0;
    for(int i=0; i<row; ++i){

	inner_diserTiming += inner_diser[i];
    } 
    elog(NOTICE, "%d rows inner_diser timing: %ld", row, inner_diserTiming/row);//-1
    long long inner_execTiming = 0;
    for(int i=0; i<row; ++i){

	inner_execTiming += inner_exec[i];
    } 
    elog(NOTICE, "%d rows inner_exec timing: %ld", row, inner_execTiming/row);
    long long inner_serTiming = 0;
    for(int i=0; i<row; ++i){

	inner_serTiming += inner_ser[i];
    } 
    elog(NOTICE, "%d rows inner_ser timing: %ld", row, inner_serTiming/row);
    long long outer_diserTiming = 0;
    for(int i=0; i<row; ++i){
	outer_diserTiming += outer_diser[i];
    } 
    elog(NOTICE, "%d rows outer_diser timing: %ld", row, outer_diserTiming/row);
    long long copy1Timing = 0;
    for(int i=0; i<row; ++i){
	copy1Timing += copy1[i];
    } 
    elog(NOTICE, "%d rows copy1 timing: %ld", row, copy1Timing/row);
    long long copy2Timing = 0;
    for(int i=0; i<row; ++i){
	copy2Timing += copy2[i];
    } 
    elog(NOTICE, "%d rows copy2 timing: %ld", row, copy2Timing/row);
    int segment_id = GpIdentity.segindex;
    char record[128];
    snprintf(record, sizeof(record), "/home/xiaqian/wasm-udf/pgsql/output/output_%d.csv", segment_id);
    
    FILE *file = fopen(record, "a");
    if (file == NULL)
    {
        ereport(ERROR,
                (errcode(ERRCODE_EXTERNAL_ROUTINE_EXCEPTION),
                 errmsg("Could not open file for writing")));
    }

    // 写入数据到CSV文件
    fprintf(file, "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",
            callTiming/row,execTiming/row,transTiming/row,copy1Timing/row, inner_diserTiming/row,inner_execTiming/row, inner_serTiming/row, copy2Timing/row, outer_diserTiming/row);

    // 关闭文件
    fclose(file);
    //PG_RETURN_VOID(); 
}



Datum sum_native(PG_FUNCTION_ARGS){
    if(row==7000000)print_time();
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    
    arrayLength = (ARR_DIMS(inputArray))[0];
    
    TimingArray *t_exec = new TimingArray(execution, row);
    
    int64 sum=0;
    for(int i=0;i<arrayLength;i++)
    {
        sum+=arrayData[i];
    }
    delete t_exec;
    delete t_call;
    
    ++row;
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    PG_RETURN_INT64(sum);
}

Datum sum_share(PG_FUNCTION_ARGS){
    
    std::string funcName("sum");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "sum";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }

    if(row==7000000)print_time();
    TimingArray *t_call = new TimingArray(call, row);

    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    
    arrayLength = (ARR_DIMS(inputArray))[0];
    
    TimingArray *t_exec = new TimingArray(execution, row);
    
    int argc = 5;
    uint32 argv[5];  
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    
    
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+3);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    
    memcpy(wasm->native, arrayData, arrayLength*8);
    
    argv[0] = wasm->buffer;
    argv[1] = arrayLength;
    argv[2] = wasm->buffer2;
    
    
    if(!runtime_call_wasm_func(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_exec[row]=time[0];
    copy1[row]=time[1];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[2]);
    delete t_exec;
    delete t_call;
    
    ++row;
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    PG_RETURN_INT64(argv[0]);
}

Datum sum_with_ser(PG_FUNCTION_ARGS){
    std::string funcName("sum");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "sum";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];

    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 4;
    uint32 argv[4];  
    
    //elog(NOTICE, "array length: %d", arrayLength);
    // serialize
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arrayData, arrayLength);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();
    delete t_trans;
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+2);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    
    memcpy(wasm->native, buf, size);
    //memset(wasm->native2,0,24);
    //delete copy_time;
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;
    
    
    if(!runtime_call_wasm_func_with_ser(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    
    
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[3]);
    
    
    delete t_exec;
    delete t_call;
    ++row;
    if(row==7000000)print_time();
    PG_RETURN_INT64(argv[0]);
}


Datum sum_with_docker(PG_FUNCTION_ARGS){
    if(row==7000000)print_time();
    TimingArray *t_call = new TimingArray(call, row);  
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    //elog(NOTICE, "array length: %d", arrayLength);
    // serialize
    TimingArray *t_exec = new TimingArray(execution, row);
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arrayData, arrayLength);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();
    delete t_trans;
    
    char send_buf[4096];
    clock_t id = CLOCK_MONOTONIC;
     
    clock_gettime(id, (struct timespec *)send_buf);
    memcpy(send_buf+16,buf,size);
    //TimingPrint *send_time = new TimingPrint("send");
 // 动态生成路径
    int segment_id = GpIdentity.segindex;
    char host_to_docker_path[128];
    char docker_to_host_path[128];
    char host_to_docker_signal[64];
    char docker_to_host_signal[64];
    snprintf(host_to_docker_path, sizeof(host_to_docker_path), "/mnt/ramdisk_segment_%d/host_to_docker", segment_id);
    snprintf(docker_to_host_path, sizeof(docker_to_host_path), "/mnt/ramdisk_segment_%d/docker_to_host", segment_id);
    snprintf(host_to_docker_signal, sizeof(host_to_docker_signal), "/mnt/ramdisk_segment_%d/host_to_docker_signal", segment_id);
    snprintf(docker_to_host_signal, sizeof(docker_to_host_signal), "/mnt/ramdisk_segment_%d/docker_to_host_signal", segment_id);
    // 写入buffer到host_to_docker
    //elog(NOTICE, "Host to Docker path: %s", host_to_docker_path);
    //elog(NOTICE, "Docker to Host path: %s", docker_to_host_path);
    
    write_to_file(host_to_docker_path, host_to_docker_signal,(char *)send_buf, size+16);
    //elog(ERROR, "can run here\n");
    //elog(NOTICE,"Buffer sent to Docker.\n");
    //delete send_time;
    char recv_buf[4096]={0};
    memset(recv_buf, 0, sizeof(recv_buf));
    //elog(NOTICE,"recv buffer addr is %x\n",recv_buf);
    //TimingPrint *recv_time = new TimingPrint("recv");
    // 从docker_to_host读取buffer
    read_from_file(docker_to_host_path,docker_to_host_signal, recv_buf, sizeof(recv_buf));
    //elog(NOTICE,"Buffer received from Docker.\n");
    struct timespec *tc=(struct timespec *)(recv_buf+24);
    struct timespec td;
    clock_gettime(id, &td);
    copy2[row]=(td.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + td.tv_nsec - tc->tv_nsec;//第二次copy时间+
    //delete recv_time;
    
    int64_t *time=(int64_t *)recv_buf;
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    delete t_exec;
    delete t_call;
    ++row;  
    PG_RETURN_INT64(time[5]);
}


Datum min_share(PG_FUNCTION_ARGS){
    if(row==1000)print_time();
    
    std::string funcName("min");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "min";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    //elog(NOTICE,"length is %d and first member is %d\n",arrayLength,arrayData[0]);
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 5;
    uint32 argv[5];  
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+3);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, arrayData, arrayLength*8);
    
    argv[0] = wasm->buffer;
    argv[1] = arrayLength;
    argv[2] = wasm->buffer2;
    
    if(!runtime_call_wasm_func(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_exec[row]=time[0];
    copy1[row]=time[1];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[2]);
    delete t_exec;
    delete t_call;
    
    ++row;
    
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    //elog(NOTICE,"row is %d\n",row);
    //PG_RETURN_INT64(argv[0]);
    PG_RETURN_INT64(0);
}






Datum min_with_ser(PG_FUNCTION_ARGS){
    if(row==1000)print_time();
    std::string funcName("min");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "min";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 4;
    uint32 argv[4];  
    
    //elog(NOTICE, "array length: %d", arrayLength);
    // serialize
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arrayData, arrayLength);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();
    delete t_trans;
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+2);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, buf, size);
    
    //memset(wasm->native2,0,24);
    //delete copy_time;
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;
    
    if(!runtime_call_wasm_func_with_ser(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    
    
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[3]);
    delete t_exec;
    delete t_call;
    ++row;
    PG_RETURN_INT64(argv[0]);
}

Datum min_with_docker(PG_FUNCTION_ARGS){
    if(row==1000)print_time();
    TimingArray *t_call = new TimingArray(call, row);  
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    //elog(NOTICE, "array length: %d", arrayLength);
    // serialize
    TimingArray *t_exec = new TimingArray(execution, row);
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arrayData, arrayLength);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();
    delete t_trans;
    
    char send_buf[4096];
    clock_t id = CLOCK_MONOTONIC;
     
    clock_gettime(id, (struct timespec *)send_buf);
    memcpy(send_buf+16,buf,size);
    //TimingPrint *send_time = new TimingPrint("send");
 // 动态生成路径
    int segment_id = GpIdentity.segindex;
    char host_to_docker_path[128];
    char docker_to_host_path[128];
    char host_to_docker_signal[64];
    char docker_to_host_signal[64];
    snprintf(host_to_docker_path, sizeof(host_to_docker_path), "/mnt/ramdisk_segment_%d/host_to_docker", segment_id);
    snprintf(docker_to_host_path, sizeof(docker_to_host_path), "/mnt/ramdisk_segment_%d/docker_to_host", segment_id);
    snprintf(host_to_docker_signal, sizeof(host_to_docker_signal), "/mnt/ramdisk_segment_%d/host_to_docker_signal", segment_id);
    snprintf(docker_to_host_signal, sizeof(docker_to_host_signal), "/mnt/ramdisk_segment_%d/docker_to_host_signal", segment_id);
    // 写入buffer到host_to_docker
    //elog(NOTICE, "Host to Docker path: %s", host_to_docker_path);
    //elog(NOTICE, "Docker to Host path: %s", docker_to_host_path);
    write_to_file(host_to_docker_path, host_to_docker_signal, (char *)send_buf, size+16);
    //elog(ERROR, "can run here\n");
    //elog(NOTICE,"Buffer sent to Docker.\n");
    //delete send_time;
    char recv_buf[4096];
    //TimingPrint *recv_time = new TimingPrint("recv");
    // 从docker_to_host读取buffer
    read_from_file(docker_to_host_path,docker_to_host_signal, recv_buf, sizeof(recv_buf));
    //elog(NOTICE,"Buffer received from Docker.\n");
    struct timespec *tc=(struct timespec *)(recv_buf+24);
    struct timespec td;
    clock_gettime(id, &td);
    copy2[row]=(td.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + td.tv_nsec - tc->tv_nsec;//第二次copy时间+
    //delete recv_time;
    
    int64_t *time=(int64_t *)recv_buf;
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    delete t_exec;
    delete t_call;
    ++row;  
    PG_RETURN_INT64(time[5]);
}


Datum count_native(PG_FUNCTION_ARGS){
    if(row==700000)print_time();
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];

    TimingArray *t_exec = new TimingArray(execution, row);
    int64 count=0;
    for(int i=0;i<arrayLength;i++)count++;
    delete t_exec;
    delete t_call;
    ++row; 
    //elog(NOTICE,"row is %d\n",row);
    //elog(NOTICE,"row is %d\n",row);
    //elog(NOTICE,"count is %d\n",argv[0]);
    //print_time();
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    PG_RETURN_INT64(count);
}



Datum count_share(PG_FUNCTION_ARGS){
    if(row==700000)print_time();
    std::string funcName("count");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "count";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];

    TimingArray *t_exec = new TimingArray(execution, row);
    
    int argc = 5;
    uint32 argv[5];  
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+3);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
   
    memcpy(wasm->native, arrayData, arrayLength*8);
    
    argv[0] = wasm->buffer;
    argv[1] = arrayLength;
    argv[2] = wasm->buffer2;
    
    
    if(!runtime_call_wasm_func(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_exec[row]=time[0];
    copy1[row]=time[1];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[2]);
    delete t_exec;
    delete t_call;
    ++row; 
    //elog(NOTICE,"row is %d\n",row);
    //elog(NOTICE,"row is %d\n",row);
    //elog(NOTICE,"count is %d\n",argv[0]);
    //print_time();
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    PG_RETURN_INT64(argv[0]);
}

Datum count_with_ser(PG_FUNCTION_ARGS){
    if(row==700000)print_time();
    
    std::string funcName("count");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "count";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    //elog(NOTICE,"array length is %d\n",arrayLength);
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 4;
    uint32 argv[4];  
    
    //elog(NOTICE, "array length: %d", arrayLength);
    // serialize
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arrayData, arrayLength);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();
    delete t_trans;
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+2);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, buf, size);
    //memset(wasm->native2,0,24);
    
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;
    
    
    if(!runtime_call_wasm_func_with_ser(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[3]);
    //elog(NOTICE,"inner_diser and inner_exec time is %d and %d",time[0],time[1]);
    delete t_exec;
    delete t_call;
    ++row;
    //print_time();
    PG_RETURN_INT64(argv[0]);
}

Datum count_with_docker(PG_FUNCTION_ARGS){
    if(row==700000)print_time();
    TimingArray *t_call = new TimingArray(call, row);  
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    //elog(NOTICE, "array length: %d", arrayLength);
    // serialize
    TimingArray *t_exec = new TimingArray(execution, row);
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arrayData, arrayLength);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();
    delete t_trans;
    
    char send_buf[4096];
    clock_t id = CLOCK_MONOTONIC;
     
    clock_gettime(id, (struct timespec *)send_buf);
    memcpy(send_buf+16,buf,size);
    //TimingPrint *send_time = new TimingPrint("send");
 // 动态生成路径
    int segment_id = GpIdentity.segindex;
    char host_to_docker_path[128];
    char docker_to_host_path[128];
    char host_to_docker_signal[64];
    char docker_to_host_signal[64];
    snprintf(host_to_docker_path, sizeof(host_to_docker_path), "/mnt/ramdisk_segment_%d/host_to_docker", segment_id);
    snprintf(docker_to_host_path, sizeof(docker_to_host_path), "/mnt/ramdisk_segment_%d/docker_to_host", segment_id);
    snprintf(host_to_docker_signal, sizeof(host_to_docker_signal), "/mnt/ramdisk_segment_%d/host_to_docker_signal", segment_id);
    snprintf(docker_to_host_signal, sizeof(docker_to_host_signal), "/mnt/ramdisk_segment_%d/docker_to_host_signal", segment_id);
    // 写入buffer到host_to_docker
    //elog(NOTICE, "Host to Docker path: %s", host_to_docker_path);
    //elog(NOTICE, "Docker to Host path: %s", docker_to_host_path);
    write_to_file(host_to_docker_path, host_to_docker_signal, (char *)send_buf, size+16);
    //elog(ERROR, "can run here\n");
    //elog(NOTICE,"Buffer sent to Docker.\n");
    //delete send_time;
    char recv_buf[4096];
    //TimingPrint *recv_time = new TimingPrint("recv");
    // 从docker_to_host读取buffer
    read_from_file(docker_to_host_path,docker_to_host_signal, recv_buf, sizeof(recv_buf));
    //elog(NOTICE,"Buffer received from Docker.\n");
    struct timespec *tc=(struct timespec *)(recv_buf+24);
    struct timespec td;
    clock_gettime(id, &td);
    copy2[row]=(td.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + td.tv_nsec - tc->tv_nsec;//第二次copy时间+
    //delete recv_time;
    
    int64_t *time=(int64_t *)recv_buf;
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    delete t_exec;
    delete t_call;
    ++row;  
    PG_RETURN_INT64(time[5]);
}


Datum product_share(PG_FUNCTION_ARGS){
    
    std::string funcName("product");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "product";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    ArrayType *inputArray1 = PG_GETARG_ARRAYTYPE_P(1);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    
    double *arrayData1;
    int arrayLength1; 
    // 获取数组数据和长度
    arrayData1 = (double *) ARR_DATA_PTR(inputArray1);
    arrayLength1 = (ARR_DIMS(inputArray1))[0];
    
    
    if(arrayLength!=arrayLength1)elog(ERROR, "array length not equal\n");
    //elog(NOTICE,"arraylength is %d\n",arrayLength);
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 5;
    uint32 argv[5];  
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+3);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, arrayData, arrayLength*8);
    memcpy(wasm->native2, arrayData1, arrayLength1*8);
    
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;
    argv[2] = arrayLength;
    
    //elog(NOTICE, "length is %d\n",arrayLength1);
    
    if(!runtime_call_wasm_func(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    double *result=(double *)argv;
    //elog(NOTICE,"sum is %f\n",argv[0]);
    int64_t *time=(int64_t *)(wasm->native);
    //int8_t *time=(int8_t *)(result_buf);
    inner_exec[row]=time[0];
    copy1[row]=time[1];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[2]);
    delete t_exec;
    delete t_call;
    ++row;
    print_time();
    //elog(NOTICE,"row is %d\n",row);
    PG_RETURN_FLOAT8(result[0]);
}

Datum product_with_ser(PG_FUNCTION_ARGS){
    
    std::string funcName("product");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "product";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    ArrayType *inputArray1 = PG_GETARG_ARRAYTYPE_P(1);
    double *arrayData1;
    int arrayLength1; 
    // 获取数组数据和长度
    arrayData1 = (double *) ARR_DATA_PTR(inputArray1);
    arrayLength1 = (ARR_DIMS(inputArray1))[0];
    if(arrayLength!=arrayLength1)elog(ERROR, "array length not equal\n");
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 4;
    uint32 argv[4];  
    
    //elog(NOTICE, "array length: %d", arrayLength);
    // serialize
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arrayData, arrayLength);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();

    flatbuffers::FlatBufferBuilder builder1(1024*1024);
    auto vec1 = builder1.CreateVector(arrayData1, arrayLength1);
    auto root1 = CreateDArray(builder1, vec1);
    FinishDArrayBuffer(builder1, root1);
    // 获取缓冲区地址和长度
    uint8_t *buf1 = builder1.GetBufferPointer();
    int size1 = builder1.GetSize();

    delete t_trans;
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+2);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, buf, size);
    memcpy(wasm->native2, buf1, size1);
    
    
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;
    
    
    if(!runtime_call_wasm_func_with_ser(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    
    
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[3]);
    delete t_exec;
    delete t_call;
    ++row;
    double *result=(double *)argv;
    print_time();
    PG_RETURN_FLOAT8(result[0]);
}


Datum product_with_docker(PG_FUNCTION_ARGS){
    
    TimingArray *t_call = new TimingArray(call, row);  
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    //elog(NOTICE, "array length: %d", arrayLength);
    // serialize
    ArrayType *inputArray1 = PG_GETARG_ARRAYTYPE_P(1);
    double *arrayData1;
    int arrayLength1; 
    // 获取数组数据和长度
    arrayData1 = (double *) ARR_DATA_PTR(inputArray1);
    arrayLength1 = (ARR_DIMS(inputArray1))[0];
    if(arrayLength!=arrayLength1)elog(ERROR, "array length not equal\n");
    TimingArray *t_exec = new TimingArray(execution, row);
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arrayData, arrayLength);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();
    flatbuffers::FlatBufferBuilder builder1(1024*1024);
    auto vec1 = builder1.CreateVector(arrayData1, arrayLength1);
    auto root1 = CreateDArray(builder1, vec1);
    FinishDArrayBuffer(builder1, root1);
    // 获取缓冲区地址和长度
    uint8_t *buf1 = builder1.GetBufferPointer();
    int size1 = builder1.GetSize();
    delete t_trans;
    
    char send_buf[1000000];//assume array length less than 50000
    clock_t id = CLOCK_MONOTONIC;
     
    clock_gettime(id, (struct timespec *)send_buf);
    int64_t* arraylen=(int64_t*)(send_buf+16);
    arraylen[0]=size;
    memcpy(send_buf+24,buf,size);
    memcpy(send_buf+24+size,buf1,size1);
    //TimingPrint *send_time = new TimingPrint("send");
 // 动态生成路径
    int segment_id = GpIdentity.segindex;
    char host_to_docker_path[128];
    char docker_to_host_path[128];
    char host_to_docker_signal[64];
    char docker_to_host_signal[64];
    snprintf(host_to_docker_path, sizeof(host_to_docker_path), "/mnt/ramdisk_segment_%d/host_to_docker", segment_id);
    snprintf(docker_to_host_path, sizeof(docker_to_host_path), "/mnt/ramdisk_segment_%d/docker_to_host", segment_id);
    snprintf(host_to_docker_signal, sizeof(host_to_docker_signal), "/mnt/ramdisk_segment_%d/host_to_docker_signal", segment_id);
    snprintf(docker_to_host_signal, sizeof(docker_to_host_signal), "/mnt/ramdisk_segment_%d/docker_to_host_signal", segment_id);
    // 写入buffer到host_to_docker
    //elog(NOTICE, "Host to Docker path: %s", host_to_docker_path);
    //elog(NOTICE, "Docker to Host path: %s", docker_to_host_path);
    write_to_file(host_to_docker_path, host_to_docker_signal, (char *)send_buf, size+24+size1);
    //elog(ERROR, "can run here\n");
    //elog(NOTICE,"Buffer sent to Docker.\n");
    //delete send_time;
    char recv_buf[4096];
    //TimingPrint *recv_time = new TimingPrint("recv");
    // 从docker_to_host读取buffer
    read_from_file(docker_to_host_path,docker_to_host_signal, recv_buf, sizeof(recv_buf));
    //elog(NOTICE,"Buffer received from Docker.\n");
    struct timespec *tc=(struct timespec *)(recv_buf+24);
    struct timespec td;
    clock_gettime(id, &td);
    copy2[row]=(td.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + td.tv_nsec - tc->tv_nsec;//第二次copy时间+
    //delete recv_time;
    
    int64_t *time=(int64_t *)recv_buf;
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    delete t_exec;
    delete t_call;
    ++row;  
    double* temp=(double*)(recv_buf+40);
    print_time();
    PG_RETURN_FLOAT8(temp[0]);
}

std::vector<std::string> split_sentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::string sentence;
    for (char ch : text) {
        sentence += ch;
        if (ch == '.' || ch == '!' || ch == '?') {
            sentences.push_back(sentence);
            sentence.clear();
        }
    }
    if (!sentence.empty()) {
        sentences.push_back(sentence);
    }
    return sentences;
}

// 分词函数
std::vector<std::string> tokenize(const std::string& sentence) {
    std::vector<std::string> tokens;
    std::string word;
    for (char ch : sentence) {
        if (std::isspace(ch) || std::ispunct(ch)) {
            if (!word.empty()) {
                tokens.push_back(word);
                word.clear();
            }
        } else {
            word += ch;
        }
    }
    if (!word.empty()) {
        tokens.push_back(word);
    }
    return tokens;
}
const std::unordered_set<std::string> positive_words = {"adorable", "accepted", "achievement", "active", "admire", "adventure", "affirmative", "affluent", "agree", "agreeable", "amazing", "angelic", "appealing", "approve", "aptitude", "attractive", "awesome", "beaming", "beautiful", "believe", "beneficial", "bliss", "bountiful", "bounty", "brave", "bravo", "brilliant", "celebrated", "champ", "champion", "charming", "cheery", "classic", "classical", "clean", "commend", "composed", "congratulation", "constant", "cool", "courageous", "creative", "cute", "dazzling", "delight", "delightful", "distinguished", "divine", "earnest", "easy", "ecstatic", "effective", "effervescent", "efficient", "effortless", "electrifying", "elegant", "enchanting", "encouraging", "endorsed", "energetic", "energized", "engaging", "enthusiastic", "essential", "esteemed", "ethical", "excellent", "exciting", "exquisite", "fabulous", "fair", "familiar", "famous", "fantastic", "favorable", "fetching", "fine", "fitting", "flourishing", "fortunate", "free", "fresh", "friendly", "fun", "funny", "generous", "genius", "genuine", "giving", "glamorous", "glowing", "good", "gorgeous", "graceful", "great", "green", "grin", "growing", "handsome", "happy", "harmonious", "healing", "healthy", "hearty", "heavenly", "honest", "honorable", "honored", "ideal", "imaginative", "imagine", "impressive", "independent", "innovate", "innovative", "instant", "instantaneous", "instinctive", "intuitive", "intellectual", "intelligent", "inventive", "jovial", "joy", "jubilant", "keen", "kind", "knowing", "knowledgeable", "laugh", "legendary", "light", "learned", "lively", "lovely", "lucid", "lucky", "luminous", "marvelous", "masterful", "meaningful", "merit", "meritorious", "miraculous", "motivating", "moving", "natural", "nice", "novel", "nurturing", "nutritious", "okay", "open", "optimistic", "paradise", "perfect", "phenomenal", "pleasurable", "plentiful", "pleasant", "poised", "polished", "popular", "positive", "powerful", "prepared", "pretty", "principled", "productive", "progress", "prominent", "protected", "proud", "quality", "quick", "quiet", "ready", "reassuring", "refined", "refreshing", "rejoice", "reliable", "remarkable", "resounding", "respected", "restored", "reward", "rewarding", "right", "robust", "safe", "satisfactory", "secure", "seemly", "simple", "skilled", "skillful", "smile", "soulful", "sparkling", "special", "spirited", "spiritual", "stirring", "stupendous", "stunning", "success", "successful", "sunny", "super", "superb", "supporting", "surprising", "terrific", "thorough", "thrilling", "thriving", "tops", "tranquil", "transforming", "transformative", "trusting", "truthful", "unreal", "unwavering", "upbeat", "upright", "upstanding", "valued", "vibrant", "victorious", "victory", "vigorous", "virtuous", "vital", "vivacious", "wealthy", "welcome", "well", "whole", "wholesome", "willing", "wonderful", "wondrous", "worthy", "wow", "yummy", "zeal", "zealous"};
const std::unordered_set<std::string> negative_words = {"abysmal", "adverse", "alarming", "angry", "annoy", "anxious", "apathy", "appalling", "atrocious", "awful", "bad", "banal", "barbed", "belligerent", "bemoan", "beneath", "boring", "broken", "callous", "clumsy", "coarse", "cold", "collapse", "confused", "contradictory", "contrary", "corrosive", "corrupt", "crazy", "creepy", "criminal", "cruel", "cry", "cutting", "decaying", "damage", "damaging", "dastardly", "deplorable", "depressed", "deprived", "deformed", "deny", "despicable", "detrimental", "dirty", "disease", "disgusting", "disheveled", "dishonest", "dishonorable", "dismal", "distress", "dreadful", "dreary", "enraged", "eroding", "evil", "fail", "faulty", "feeble", "fight", "filthy", "foul", "frighten", "frightful", "gawky", "ghastly", "grave", "greed", "grim", "grimace", "gross", "grotesque", "gruesome", "guilty", "haggard", "hard", "hard-hearted", "harmful", "hate", "hideous", "homely", "horrendous", "horrible", "hostile", "hurt", "hurtful", "icky", "ignore", "ignorant", "ill", "immature", "imperfect", "impossible", "inane", "inelegant", "infernal", "injure", "injurious", "insane", "insidious", "insipid", "jealous", "junky", "lose", "lousy", "lumpy", "malicious", "mean", "menacing", "messy", "misshapen", "missing", "misunderstood", "moan", "moldy", "monstrous", "naive", "nasty", "naughty", "negate", "negative", "never", "nobody", "nondescript", "nonsense", "noxious", "objectionable", "odious", "offensive", "old", "oppressive", "pain", "perturb", "pessimistic", "petty", "plain", "poisonous", "poor", "prejudice", "questionable", "quirky", "quit", "reject", "renege", "repellant", "reptilian", "repulsive", "repugnant", "revenge", "revolting", "rocky", "rotten", "rude", "ruthless", "sad", "savage", "scare", "scary", "scream", "severe", "shoddy", "shocking", "sick", "sickening", "sinister", "slimy", "smelly", "sobbing", "sorry", "spiteful", "sticky", "stinky", "stormy", "stressful", "stuck", "stupid", "substandard", "suspect", "suspicious", "tense", "terrible", "terrifying", "threatening", "ugly", "undermine", "unfair", "unfavorable", "unhappy", "unhealthy", "unjust", "unlucky", "unpleasant", "upset", "unsatisfactory", "unsightly", "untoward", "unwanted", "unwelcome", "unwholesome", "unwieldy", "unwise", "upset", "vice", "vicious", "vile", "villainous", "vindictive", "wary", "weary", "wicked", "woeful", "worthless", "wound", "yell", "yucky"};

// 定义正面和反面词汇
Datum extract_sentiment_native(PG_FUNCTION_ARGS){
    //elog(NOTICE,"row is %d\n",row);
    if(row==40000)print_time();
    TimingArray *t_call = new TimingArray(call, row);
    int64 id = PG_GETARG_INT64(0);
    text *input_text = PG_GETARG_TEXT_PP(1);
    TimingArray *t_exec = new TimingArray(execution, row);
    char *text_str = text_to_cstring(input_text);
    
    std::string texta(text_str);
    //elog(NOTICE,"string is %s",texta.c_str());
    auto sentences = split_sentences(texta);
    emo results[100];//假设最多100个情感词
    int64_t len=0;
    for (const auto& sentence : sentences) {
        auto words = tokenize(sentence);
        for (const auto& word : words) {
            std::string lower_word = word;
            std::transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);
            if (positive_words.find(lower_word) != positive_words.end()) {
                //std::cout << "Type: positive, Word: " << word << ", Sentence: " << sentence << std::endl;
                std::strncpy(results[len].judge, "POS", 3);
                results[len].judge[3]='\0';
                //std::cout<<word.c_str()<<word.length()<<"  "<<sentence.c_str()<<sentence.length()<<std::endl;
                std::strncpy(results[len].word, word.c_str(), word.length());
                results[len].word[word.length()]='\0';
                std::strncpy(results[len].sentence, sentence.c_str(), sentence.length());
                results[len].sentence[sentence.length()]='\0';
                len++;
                
                
            } else if (negative_words.find(lower_word) != negative_words.end()) {
                //std::cout << "Type: negative, Word: " << word.c_str() << ", Sentence: " << sentence.c_str() << std::endl;
                std::strncpy(results[len].judge, "NEG", 3);
                results[len].judge[3]='\0';
                //std::cout<<word.c_str()<<sizeof(word.c_str())<<"  "<<sentence.c_str()<<sizeof(sentence.c_str())<<std::endl;
                std::strncpy(results[len].word, word.c_str(),word.length());
                results[len].word[word.length()]='\0';
                std::strncpy(results[len].sentence, sentence.c_str(), sentence.length());
                results[len].sentence[sentence.length()]='\0';
                len++;
                
            }
        }
    }
    delete t_exec;
    TupleDesc pair_desc;
    Oid pair_type_oid = TypenameGetTypid("string_array");
    bool nulls[4] = {false, false,false, false};
    int16 pairlen;
    bool pairbyval;
    char pairalign;

    // 获取 pair 类型的信息
    get_typlenbyvalalign(pair_type_oid, &pairlen, &pairbyval, &pairalign);
    //elog(NOTICE,"len is %d,byval is %d,align is %c\n",pairlen,pairbyval,pairalign);
    if (!OidIsValid(pair_type_oid)) {
        ereport(ERROR, (errmsg("Type \"string_array\" does not exist.")));
    }

    pair_desc = TypeGetTupleDesc(pair_type_oid, NIL);

    Datum* result_pairs = (Datum *) palloc(len * sizeof(Datum));

    for (int i = 0; i < len; i++) {
        Datum values[4];
        values[0] = Int64GetDatum(id);
        values[1] = CStringGetTextDatum(results[i].sentence);
        values[2] = CStringGetTextDatum(results[i].judge);
        values[3] = CStringGetTextDatum(results[i].word);
        //elog(NOTICE,"(%d,%d)",value->Get(i)->first(),value->Get(i)->second());
        result_pairs[i] = HeapTupleGetDatum(heap_form_tuple(pair_desc, values, nulls));
    }
    ArrayType *result_array = construct_array(result_pairs, len, pair_type_oid, pairlen, pairbyval, pairalign);
    //elog(NOTICE, "row is %d",row);
    delete t_call;
    ++row;
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    PG_RETURN_ARRAYTYPE_P(result_array);
}


Datum extract_sentiment_share(PG_FUNCTION_ARGS){
    if(row==40000)print_time();
    
    std::string funcName("extract_sentiment");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024*8];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "extract_sentiment";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str()))
        {
            elog(ERROR, "wasm lookup function failed");
        }
        if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    
    TimingArray *t_call = new TimingArray(call, row);
    int64 id = PG_GETARG_INT64(0);
    text *input_text = PG_GETARG_TEXT_PP(1);
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 4;
    uint32 argv[4];
    char *text_str = text_to_cstring(input_text);
    wasm_wrapper_t *wasm = func_map.at(funcName);
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+2);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, text_str, strlen(text_str));
    memset(wasm->native+strlen(text_str),'\0',1);
    //elog(NOTICE,"string is %s\n",wasm->native);
    

    
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;
    //elog(NOTICE, "length is %d\n",arrayLength1);
    
    if(!runtime_call_wasm_func(wasm, argc, argv))
    {
        elog(NOTICE,"string is %s\n",wasm->native);
        elog(ERROR, "call wasm function falied");
    }
    
    //elog(NOTICE,"signal is %d\n",argv[0]);
    emo *result_buf=(emo *)(wasm_runtime_addr_app_to_native(wasm->module_inst,argv[0]));
    //elog(NOTICE,"sum is %f\n",argv[0]);
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_exec[row]=time[0];
    //elog(NOTICE,"inner_exec time is %dns\n",time[0]);
    //elog(NOTICE,"len is %d\n",time[1]);
    int len=(int)(time[1]);
    copy1[row]=time[2];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[3]);
    delete t_exec;
    TupleDesc pair_desc;
    Oid pair_type_oid = TypenameGetTypid("string_array");
    bool nulls[4] = {false, false,false, false};
    int16 pairlen;
    bool pairbyval;
    char pairalign;

    // 获取 pair 类型的信息
    get_typlenbyvalalign(pair_type_oid, &pairlen, &pairbyval, &pairalign);
    //elog(NOTICE,"len is %d,byval is %d,align is %c\n",pairlen,pairbyval,pairalign);
    if (!OidIsValid(pair_type_oid)) {
        ereport(ERROR, (errmsg("Type \"string_array\" does not exist.")));
    }

    pair_desc = TypeGetTupleDesc(pair_type_oid, NIL);

    Datum* result_pairs = (Datum *) palloc(len * sizeof(Datum));

    for (int i = 0; i < len; i++) {
        Datum values[4];
        values[0] = Int64GetDatum(id);
        values[1] = CStringGetTextDatum(result_buf[i].sentence);
        values[2] = CStringGetTextDatum(result_buf[i].judge);
        values[3] = CStringGetTextDatum(result_buf[i].word);
        //elog(NOTICE,"(%d,%d)",value->Get(i)->first(),value->Get(i)->second());
        result_pairs[i] = HeapTupleGetDatum(heap_form_tuple(pair_desc, values, nulls));
    }
    ArrayType *result_array = construct_array(result_pairs, len, pair_type_oid, pairlen, pairbyval, pairalign);
    //elog(NOTICE, "row is %d",row);
    delete t_call;
    ++row;
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    PG_RETURN_ARRAYTYPE_P(result_array);
}


Datum extract_sentiment_with_ser(PG_FUNCTION_ARGS){
    if(row==500000)print_time();
    
    std::string funcName("extract_sentiment");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024*8];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "extract_sentiment";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str()))
        {
            elog(ERROR, "wasm lookup function failed");
        }
        if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    int64 id = PG_GETARG_INT64(0);
    text *input_text = PG_GETARG_TEXT_PP(1);
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 4;
    uint32 argv[4];
    char *text_str = text_to_cstring(input_text);
    wasm_wrapper_t *wasm = func_map.at(funcName);
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+2);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, text_str, strlen(text_str));
    memset(wasm->native+strlen(text_str),'\0',1);
    //elog(NOTICE,"string is %s\n",wasm->native);
    

    
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;
    //elog(NOTICE, "length is %d\n",arrayLength1);
    
    if(!runtime_call_wasm_func_with_ser(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    
    int64_t *result_buf=(int64_t *)(wasm_runtime_addr_app_to_native(wasm->module_inst,argv[0]));
    //elog(NOTICE,"sum is %f\n",argv[0]);
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_exec[row]=time[0];
    inner_ser[row]=time[1];
    copy1[row]=time[2];
    
    
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[3]);
    
    //elog(NOTICE,"inner_exec time is %dns\n",time[0]);
    TimingArray *t_out_diser = new TimingArray(outer_diser, row);
    const StringArray* pair_array=GetStringArray(result_buf);
    auto value = pair_array->arrays();
    auto len = value->size();
    delete t_out_diser;
    delete t_exec;
    TupleDesc pair_desc;
    Oid pair_type_oid = TypenameGetTypid("string_array");
    bool nulls[4] = {false, false,false, false};
    int16 pairlen;
    bool pairbyval;
    char pairalign;

    // 获取 pair 类型的信息
    get_typlenbyvalalign(pair_type_oid, &pairlen, &pairbyval, &pairalign);
    //elog(NOTICE,"len is %d,byval is %d,align is %c\n",pairlen,pairbyval,pairalign);
    if (!OidIsValid(pair_type_oid)) {
        ereport(ERROR, (errmsg("Type \"string_array\" does not exist.")));
    }

    pair_desc = TypeGetTupleDesc(pair_type_oid, NIL);

    Datum* result_pairs = (Datum *) palloc(len * sizeof(Datum));

    for (int i = 0; i < len; i++) {
        Datum values[4];
        values[0] = Int64GetDatum(id);
        values[1] = CStringGetTextDatum(value->Get(i)->sentence()->c_str());
        values[2] = CStringGetTextDatum(value->Get(i)->sentiment()->c_str());
        values[3] = CStringGetTextDatum(value->Get(i)->word()->c_str());
        //elog(NOTICE,"(%d,%d)",value->Get(i)->first(),value->Get(i)->second());
        result_pairs[i] = HeapTupleGetDatum(heap_form_tuple(pair_desc, values, nulls));
    }
    ArrayType *result_array = construct_array(result_pairs, len, pair_type_oid, pairlen, pairbyval, pairalign);
    //elog(NOTICE, "row is %d",row);
    delete t_call;
    ++row;
    PG_RETURN_ARRAYTYPE_P(result_array);
}

Datum extract_sentiment_with_docker(PG_FUNCTION_ARGS){
    if(row==500000)print_time();
    TimingArray *t_call = new TimingArray(call, row);
    int64 id = PG_GETARG_INT64(0);
    text *input_text = PG_GETARG_TEXT_PP(1);
    TimingArray *t_exec = new TimingArray(execution, row);
    char *text_str = text_to_cstring(input_text);
    
    char send_buf[102400];
    clock_t ida = CLOCK_MONOTONIC;
    clock_gettime(ida, (struct timespec *)send_buf);
    //elog(NOTICE,"string length is %d\n",strlen(text_str));
    memcpy(send_buf+16,text_str,strlen(text_str));
    memset(send_buf+16+strlen(text_str),'\0',1);
    //elog(NOTICE, "length is %d\n",arrayLength1);
    
    int segment_id = GpIdentity.segindex;
    char host_to_docker_path[128];
    char docker_to_host_path[128];
    char host_to_docker_signal[64];
    char docker_to_host_signal[64];
    snprintf(host_to_docker_path, sizeof(host_to_docker_path), "/mnt/ramdisk_segment_%d/host_to_docker", segment_id);
    snprintf(docker_to_host_path, sizeof(docker_to_host_path), "/mnt/ramdisk_segment_%d/docker_to_host", segment_id);
    snprintf(host_to_docker_signal, sizeof(host_to_docker_signal), "/mnt/ramdisk_segment_%d/host_to_docker_signal", segment_id);
    snprintf(docker_to_host_signal, sizeof(docker_to_host_signal), "/mnt/ramdisk_segment_%d/docker_to_host_signal", segment_id);
    
    // 写入buffer到host_to_docker
    //elog(NOTICE, "Host to Docker path: %s", host_to_docker_path);
    //elog(NOTICE, "Docker to Host path: %s", docker_to_host_path);
    write_to_file(host_to_docker_path, host_to_docker_signal, (char *)send_buf, 17+strlen(text_str));
    //elog(ERROR, "can run here\n");
    //elog(NOTICE,"Buffer sent to Docker.\n");
    //delete send_time;
    char recv_buf[102400];
    //TimingPrint *recv_time = new TimingPrint("recv");
    // 从docker_to_host读取buffer
    read_from_file(docker_to_host_path,docker_to_host_signal, recv_buf, sizeof(recv_buf));
    //elog(NOTICE,"Buffer received from Docker.\n");
    struct timespec *tc=(struct timespec *)(recv_buf+24);
    struct timespec td;
    clock_gettime(ida, &td);
    copy2[row]=(td.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + td.tv_nsec - tc->tv_nsec;//第二次copy时间+
    
    
   
    int64_t *time=(int64_t *)recv_buf;
    inner_exec[row]=time[0];
    inner_ser[row]=time[1];
    copy1[row]=time[2];
    //elog(NOTICE,"inner_exec time is %dns\n",time[0]);
    TimingArray *t_out_diser = new TimingArray(outer_diser, row);
    const StringArray* pair_array=GetStringArray(recv_buf+40);
    auto value = pair_array->arrays();
    auto len = value->size();
    delete t_out_diser;
    delete t_exec;
    TupleDesc pair_desc;
    Oid pair_type_oid = TypenameGetTypid("string_array");
    bool nulls[4] = {false, false,false, false};
    int16 pairlen;
    bool pairbyval;
    char pairalign;

    // 获取 pair 类型的信息
    get_typlenbyvalalign(pair_type_oid, &pairlen, &pairbyval, &pairalign);
    //elog(NOTICE,"len is %d,byval is %d,align is %c\n",pairlen,pairbyval,pairalign);
    if (!OidIsValid(pair_type_oid)) {
        ereport(ERROR, (errmsg("Type \"string_array\" does not exist.")));
    }

    pair_desc = TypeGetTupleDesc(pair_type_oid, NIL);

    Datum* result_pairs = (Datum *) palloc(len * sizeof(Datum));

    for (int i = 0; i < len; i++) {
        Datum values[4];
        values[0] = Int64GetDatum(id);
        values[1] = CStringGetTextDatum(value->Get(i)->sentence()->c_str());
        values[2] = CStringGetTextDatum(value->Get(i)->sentiment()->c_str());
        values[3] = CStringGetTextDatum(value->Get(i)->word()->c_str());
        //elog(NOTICE,"(%d,%d)",value->Get(i)->first(),value->Get(i)->second());
        result_pairs[i] = HeapTupleGetDatum(heap_form_tuple(pair_desc, values, nulls));
    }
    ArrayType *result_array = construct_array(result_pairs, len, pair_type_oid, pairlen, pairbyval, pairalign);
    //elog(NOTICE, "row is %d",row);
    delete t_call;
    ++row;
    PG_RETURN_ARRAYTYPE_P(result_array);
}

Datum make_pairs_native(PG_FUNCTION_ARGS){
    if(row==3000)print_time();
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    TimingArray *t_exec = new TimingArray(execution, row);
    int n = arrayLength * (arrayLength - 1) / 2;
    couple* result = (couple*)malloc(n * sizeof(couple));
    if (result == NULL) {
        elog(ERROR, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    int index = 0;
    for (int i = 0; i < arrayLength; ++i) {
        for (int j = i + 1; j < arrayLength; ++j) {
            result[index].first = arrayData[i];
            result[index].second = arrayData[j];
            ++index;
            //elog(NOTICE,"the pair is (%d,%d)",result[index].first,result[index].second);
        }
    }
    delete t_exec;
    TupleDesc pair_desc;
    Oid pair_type_oid = TypenameGetTypid("pair");
    
    bool nulls[2] = {false, false};
    /*
    if (!OidIsValid(pair_type_oid)) {
        ereport(ERROR, (errmsg("Type \"pair\" does not exist.")));
    }
    */
    pair_desc = TypeGetTupleDesc(pair_type_oid, NULL);
    int16 pairlen;
    bool pairbyval;
    char pairalign;

    // 获取 pair 类型的信息
    get_typlenbyvalalign(pair_type_oid, &pairlen, &pairbyval, &pairalign);
    //elog(NOTICE,"len is %d,byval is %d,align is %c\n",pairlen,pairbyval,pairalign);
    int len=arrayLength*(arrayLength-1)/2;
    Datum* result_pairs = (Datum *) palloc(len * sizeof(Datum));

    for (int i = 0; i < len; i++) {
        Datum values[2];
        values[0] = Int64GetDatum(result[i].first);
        values[1] = Int64GetDatum(result[i].second);
        //elog(NOTICE,"(%d,%d)",result_buf[i].first,result_buf[i].second);
        result_pairs[i] = HeapTupleGetDatum(heap_form_tuple(pair_desc, values, nulls));
    }
    
    ArrayType *result_array = construct_array(result_pairs, len, pair_type_oid, pairlen,pairbyval, pairalign);
    
    delete t_call;
    ++row;
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    //elog(NOTICE,"row is %d\n",row);
    PG_RETURN_ARRAYTYPE_P(result_array);
}

Datum make_pairs_share(PG_FUNCTION_ARGS){
    if(row==3000)print_time();
    
    std::string funcName("make_pairs");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "make_pairs";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    TimingArray *t_exec = new TimingArray(execution, row);
    wasm_wrapper_t *wasm = func_map.at(funcName);
    
    int argc = 5;
    uint32 argv[5];  
    
    
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+3);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, arrayData, arrayLength*8);
    
    argv[0] = wasm->buffer;
    argv[1] = arrayLength;
    argv[2] = wasm->buffer2;
    
    
    if(!runtime_call_wasm_func(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_exec[row]=time[0];
    copy1[row]=time[1];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[2]);
    couple *result_buf=(couple *)(wasm_runtime_addr_app_to_native(wasm->module_inst,argv[0]));
    delete t_exec;
    TupleDesc pair_desc;
    Oid pair_type_oid = TypenameGetTypid("pair");
    
    bool nulls[2] = {false, false};
    /*
    if (!OidIsValid(pair_type_oid)) {
        ereport(ERROR, (errmsg("Type \"pair\" does not exist.")));
    }
    */
    pair_desc = TypeGetTupleDesc(pair_type_oid, NULL);
    int16 pairlen;
    bool pairbyval;
    char pairalign;

    // 获取 pair 类型的信息
    get_typlenbyvalalign(pair_type_oid, &pairlen, &pairbyval, &pairalign);
    //elog(NOTICE,"len is %d,byval is %d,align is %c\n",pairlen,pairbyval,pairalign);
    int len=arrayLength*(arrayLength-1)/2;
    Datum* result_pairs = (Datum *) palloc(len * sizeof(Datum));

    for (int i = 0; i < len; i++) {
        Datum values[2];
        values[0] = Int64GetDatum(result_buf[i].first);
        values[1] = Int64GetDatum(result_buf[i].second);
        //elog(NOTICE,"(%d,%d)",result_buf[i].first,result_buf[i].second);
        result_pairs[i] = HeapTupleGetDatum(heap_form_tuple(pair_desc, values, nulls));
    }
    
    ArrayType *result_array = construct_array(result_pairs, len, pair_type_oid, pairlen,pairbyval, pairalign);
    
    delete t_call;
    ++row;
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    //elog(NOTICE,"row is %d\n",row);
    PG_RETURN_ARRAYTYPE_P(result_array);
}

Datum make_pairs_with_ser(PG_FUNCTION_ARGS){
    if(row==3000)print_time();
    
    std::string funcName("make_pairs");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "make_pairs";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 4;
    uint32 argv[4];  
    
    //elog(NOTICE, "array length: %d", arrayLength);
    // serialize
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arrayData, arrayLength);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();
    delete t_trans;
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+2);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, buf, size);
    //memset(wasm->native2,0,24);
   
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;
    
    
    if(!runtime_call_wasm_func_with_ser(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    
    int8_t *result_buf=(int8_t *)(wasm_runtime_addr_app_to_native(wasm->module_inst,argv[0]));
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    inner_ser[row]=time[2];
    copy1[row]=time[3];
    
    
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[4]);
    
    TimingArray *t_out_diser = new TimingArray(outer_diser, row);
    const PairArray *pair_array =GetPairArray(result_buf);
    auto value = pair_array->pairs();
    auto len = value->size();
    delete t_out_diser;
    delete t_exec;
    //elog(NOTICE,"native addr is %ld,app addr is %ld,converted native addr is %ld\n",(int64_t)wasm->native2,wasm->buffer2,wasm_runtime_addr_app_to_native(wasm->module_inst,wasm->buffer2));
    
    TupleDesc pair_desc;
    Oid pair_type_oid = TypenameGetTypid("pair");
    bool nulls[2] = {false, false};
    int16 pairlen;
    bool pairbyval;
    char pairalign;

    // 获取 pair 类型的信息
    get_typlenbyvalalign(pair_type_oid, &pairlen, &pairbyval, &pairalign);
    //elog(NOTICE,"len is %d,byval is %d,align is %c\n",pairlen,pairbyval,pairalign);
    if (!OidIsValid(pair_type_oid)) {
        ereport(ERROR, (errmsg("Type \"pair\" does not exist.")));
    }

    pair_desc = TypeGetTupleDesc(pair_type_oid, NIL);

    Datum* result_pairs = (Datum *) palloc(len * sizeof(Datum));

    for (int i = 0; i < len; i++) {
        Datum values[2];
        values[0] = Int64GetDatum(value->Get(i)->first());
        values[1] = Int64GetDatum(value->Get(i)->second());
        //elog(NOTICE,"(%d,%d)",value->Get(i)->first(),value->Get(i)->second());
        result_pairs[i] = HeapTupleGetDatum(heap_form_tuple(pair_desc, values, nulls));
    }
    ArrayType *result_array = construct_array(result_pairs, len, pair_type_oid, pairlen, pairbyval, pairalign);

    delete t_call;
    ++row;
    PG_RETURN_ARRAYTYPE_P(result_array);
}

Datum make_pairs_with_docker(PG_FUNCTION_ARGS){
    if(row==290000)print_time();
    TimingArray *t_call = new TimingArray(call, row); 
    ArrayType *inputArray = PG_GETARG_ARRAYTYPE_P(0);
    int64 *arrayData;
    int arrayLength; 
    // 获取数组数据和长度
    arrayData = (int64 *) ARR_DATA_PTR(inputArray);
    arrayLength = (ARR_DIMS(inputArray))[0];
    //elog(NOTICE, "array length: %d", arrayLength);
    // serialize
    TimingArray *t_exec = new TimingArray(execution, row);
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder(1024*1024);
    auto vec = builder.CreateVector(arrayData, arrayLength);
    auto root = CreateLongArray(builder, vec);
    FinishLongArrayBuffer(builder, root);
    // 获取缓冲区地址和长度
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();
    delete t_trans;
    
    char send_buf[4096];
    clock_t id = CLOCK_MONOTONIC;
     
    clock_gettime(id, (struct timespec *)send_buf);
    memcpy(send_buf+16,buf,size);
    //TimingPrint *send_time = new TimingPrint("send");
    // 动态生成路径
    int segment_id = GpIdentity.segindex;
    char host_to_docker_path[128];
    char docker_to_host_path[128];
    char host_to_docker_signal[64];
    char docker_to_host_signal[64];
    snprintf(host_to_docker_path, sizeof(host_to_docker_path), "/mnt/ramdisk_segment_%d/host_to_docker", segment_id);
    snprintf(docker_to_host_path, sizeof(docker_to_host_path), "/mnt/ramdisk_segment_%d/docker_to_host", segment_id);
    snprintf(host_to_docker_signal, sizeof(host_to_docker_signal), "/mnt/ramdisk_segment_%d/host_to_docker_signal", segment_id);
    snprintf(docker_to_host_signal, sizeof(docker_to_host_signal), "/mnt/ramdisk_segment_%d/docker_to_host_signal", segment_id);
    
    // 写入buffer到host_to_docker
    write_to_file(host_to_docker_path, host_to_docker_signal, (char *)send_buf, size+16);
    //elog(ERROR, "can run here\n");
    //elog(NOTICE,"Buffer sent to Docker.\n");
    //delete send_time;
    char recv_buf[4096];
    //TimingPrint *recv_time = new TimingPrint("recv");
    // 从docker_to_host读取buffer
    read_from_file(docker_to_host_path,docker_to_host_signal, recv_buf, sizeof(recv_buf));
    //elog(NOTICE,"Buffer received from Docker.\n");
    struct timespec *tc=(struct timespec *)(recv_buf+32);
    struct timespec td;
    clock_gettime(id, &td);
    copy2[row]=(td.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + td.tv_nsec - tc->tv_nsec;//第二次copy时间+
    //delete recv_time;
    
    int64_t *time=(int64_t *)recv_buf;
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    inner_ser[row]=time[2];
    copy1[row]=time[3];
    TimingArray *t_out_diser = new TimingArray(outer_diser, row);
    const PairArray *pair_array =GetPairArray(recv_buf+48);
    auto value = pair_array->pairs();
    auto len = value->size();
    delete t_out_diser;
    delete t_exec;
    TupleDesc pair_desc;
    Oid pair_type_oid = TypenameGetTypid("pair");
    bool nulls[2] = {false, false};
    int16 pairlen;
    bool pairbyval;
    char pairalign;

    // 获取 pair 类型的信息
    get_typlenbyvalalign(pair_type_oid, &pairlen, &pairbyval, &pairalign);
    //elog(NOTICE,"len is %d,byval is %d,align is %c\n",pairlen,pairbyval,pairalign);
    if (!OidIsValid(pair_type_oid)) {
        ereport(ERROR, (errmsg("Type \"pair\" does not exist.")));
    }

    pair_desc = TypeGetTupleDesc(pair_type_oid, NIL);

    Datum* result_pairs = (Datum *) palloc(len * sizeof(Datum));

    for (int i = 0; i < len; i++) {
        Datum values[2];
        values[0] = Int64GetDatum(value->Get(i)->first());
        values[1] = Int64GetDatum(value->Get(i)->second());
        //elog(NOTICE,"(%d,%d)",value->Get(i)->first(),value->Get(i)->second());
        result_pairs[i] = HeapTupleGetDatum(heap_form_tuple(pair_desc, values, nulls));
    }
    ArrayType *result_array = construct_array(result_pairs, len, pair_type_oid, pairlen, pairbyval, pairalign);

    delete t_call;
    ++row;  
    PG_RETURN_ARRAYTYPE_P(result_array);
}

Datum
min_agg_init_1(PG_FUNCTION_ARGS)
{
    PG_RETURN_NULL();
}


Datum
min_agg_trans_1(PG_FUNCTION_ARGS)
{
    //int segment_id = GpIdentity.segindex;
    //elog(NOTICE,"segment is %d\n",segment_id);
    std::string funcName("min");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "min";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    float8 state = PG_ARGISNULL(0) ? NULL :PG_GETARG_FLOAT8(0);
    Numeric value = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERIC(1);
    Datum float8_datum = DirectFunctionCall1(numeric_float8, NumericGetDatum(value));
    double new_value = DatumGetFloat8(float8_datum);
    int arrayLength=1;
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 5;
    uint32 argv[5]; 
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    double *min=(double *)(wasm->native2);
    min[3]=state;
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+3);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, &new_value, arrayLength*8);
    argv[0] = wasm->buffer;
    argv[1] = arrayLength;
    argv[2] = wasm->buffer2;  
    //PG_RETURN_VOID();
    if(!runtime_call_wasm_func(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_exec[row]=time[0];
    copy1[row]=time[1];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[2]);
    delete t_exec;
    delete t_call;
    ++row;
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    if(row==801000)print_time();
    PG_RETURN_FLOAT8(min[3]);
}


Datum
min_agg_combine_1(PG_FUNCTION_ARGS)
{
    float8 state1 = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    float8 state2 = PG_ARGISNULL(1) ? NULL : PG_GETARG_FLOAT8(1);

    if (state1 == NULL)
        PG_RETURN_NUMERIC(state2);
    if (state2 == NULL)
        PG_RETURN_NUMERIC(state1);
    if(state1<state2)PG_RETURN_FLOAT8(state1);
    else PG_RETURN_FLOAT8(state2);
}

Datum
min_agg_final_1(PG_FUNCTION_ARGS)
{
    float8 final_min = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    if (final_min == NULL)
        PG_RETURN_NULL();
    //elog(NOTICE,"min is %f\n",final_min);
    char buf[64];
    Numeric result;
    Datum numeric_datum;
    snprintf(buf, sizeof(buf), "%.2f", final_min);
    numeric_datum = DirectFunctionCall3(numeric_in,
                                        CStringGetDatum(buf),
                                        ObjectIdGetDatum(InvalidOid),
                                        Int32GetDatum(-1));

    result = DatumGetNumeric(numeric_datum);
    PG_RETURN_NUMERIC(result);
}

Datum
min_agg_init_2(PG_FUNCTION_ARGS)
{
    PG_RETURN_NULL();
}


Datum
min_agg_trans_2(PG_FUNCTION_ARGS)
{
    
    std::string funcName("min");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "min";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    float8 state = PG_ARGISNULL(0) ? NULL :PG_GETARG_FLOAT8(0);
    Numeric value = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERIC(1);
    Datum float8_datum = DirectFunctionCall1(numeric_float8, NumericGetDatum(value));
    double new_value = DatumGetFloat8(float8_datum);
    int arrayLength=1;
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 4;
    uint32 argv[4];

    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder1(1024*1024);
    auto vec1 = builder1.CreateVector(&new_value, arrayLength);
    auto root1 = CreateDArray(builder1, vec1);
    FinishDArrayBuffer(builder1, root1);
    // 获取缓冲区地址和长度
    uint8_t *buf1 = builder1.GetBufferPointer();
    int size1 = builder1.GetSize();

    delete t_trans;

    wasm_wrapper_t *wasm = func_map.at(funcName);
    double *min=(double *)(wasm->native2);
    min[4]=state;
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+2);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, buf1, size1);
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;  
    //PG_RETURN_VOID();
    if(!runtime_call_wasm_func_with_ser(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[3]);
    delete t_exec;
    delete t_call;
    ++row;
    if(row==801000)print_time();
    PG_RETURN_FLOAT8(min[4]);
}


Datum
min_agg_combine_2(PG_FUNCTION_ARGS)
{
    float8 state1 = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    float8 state2 = PG_ARGISNULL(1) ? NULL : PG_GETARG_FLOAT8(1);

    if (state1 == NULL)
        PG_RETURN_NUMERIC(state2);
    if (state2 == NULL)
        PG_RETURN_NUMERIC(state1);
    if(state1<state2)PG_RETURN_FLOAT8(state1);
    else PG_RETURN_FLOAT8(state2);
}

Datum
min_agg_final_2(PG_FUNCTION_ARGS)
{
    float8 final_min = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    if (final_min == NULL)
        PG_RETURN_NULL();
    //elog(NOTICE,"min is %f\n",final_min);
    char buf[64];
    Numeric result;
    Datum numeric_datum;
    snprintf(buf, sizeof(buf), "%.2f", final_min);
    numeric_datum = DirectFunctionCall3(numeric_in,
                                        CStringGetDatum(buf),
                                        ObjectIdGetDatum(InvalidOid),
                                        Int32GetDatum(-1));

    result = DatumGetNumeric(numeric_datum);
    PG_RETURN_NUMERIC(result);
}

Datum
min_agg_init_3(PG_FUNCTION_ARGS)
{
    PG_RETURN_NULL();
}


Datum
min_agg_trans_3(PG_FUNCTION_ARGS)
{
    if(row==801000)print_time();
    TimingArray *t_call = new TimingArray(call, row);  
    float8 state = PG_ARGISNULL(0) ? NULL :PG_GETARG_FLOAT8(0);
    Numeric value = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERIC(1);
    Datum float8_datum = DirectFunctionCall1(numeric_float8, NumericGetDatum(value));
    double new_value = DatumGetFloat8(float8_datum);
    int arrayLength=1;
    TimingArray *t_exec = new TimingArray(execution, row);
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder1(1024*1024);
    auto vec1 = builder1.CreateVector(&new_value, arrayLength);
    auto root1 = CreateDArray(builder1, vec1);
    FinishDArrayBuffer(builder1, root1);
    // 获取缓冲区地址和长度
    uint8_t *buf1 = builder1.GetBufferPointer();
    int size1 = builder1.GetSize();

    delete t_trans;
    
    char send_buf[4096];
    clock_t id = CLOCK_MONOTONIC;
     
    clock_gettime(id, (struct timespec *)send_buf);
    double* cur_min=(double*)send_buf;
    cur_min[2]=state;
    memcpy(send_buf+24,buf1,size1);
    int segment_id = GpIdentity.segindex;
    char host_to_docker_path[128];
    char docker_to_host_path[128];
    char host_to_docker_signal[64];
    char docker_to_host_signal[64];
    snprintf(host_to_docker_path, sizeof(host_to_docker_path), "/mnt/ramdisk_segment_%d/host_to_docker", segment_id);
    snprintf(docker_to_host_path, sizeof(docker_to_host_path), "/mnt/ramdisk_segment_%d/docker_to_host", segment_id);
    snprintf(host_to_docker_signal, sizeof(host_to_docker_signal), "/mnt/ramdisk_segment_%d/host_to_docker_signal", segment_id);
    snprintf(docker_to_host_signal, sizeof(docker_to_host_signal), "/mnt/ramdisk_segment_%d/docker_to_host_signal", segment_id);
    
    write_to_file(host_to_docker_path, host_to_docker_signal, (char *)send_buf, size1+24);
    
    char recv_buf[4096];
    
    read_from_file(docker_to_host_path,docker_to_host_signal, recv_buf, sizeof(recv_buf));
    //elog(NOTICE,"Buffer received from Docker.\n");
    struct timespec *tc=(struct timespec *)(recv_buf+24);
    struct timespec td;
    clock_gettime(id, &td);
    copy2[row]=(td.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + td.tv_nsec - tc->tv_nsec;//第二次copy时间+
    //delete recv_time;
    
    int64_t *time=(int64_t *)recv_buf;
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    delete t_exec;
    delete t_call;
    ++row;
    double *min=(double*)recv_buf;
    PG_RETURN_FLOAT8(min[5]);
}


Datum
min_agg_combine_3(PG_FUNCTION_ARGS)
{
    float8 state1 = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    float8 state2 = PG_ARGISNULL(1) ? NULL : PG_GETARG_FLOAT8(1);

    if (state1 == NULL)
        PG_RETURN_NUMERIC(state2);
    if (state2 == NULL)
        PG_RETURN_NUMERIC(state1);
    if(state1<state2)PG_RETURN_FLOAT8(state1);
    else PG_RETURN_FLOAT8(state2);
}

Datum
min_agg_final_3(PG_FUNCTION_ARGS)
{
    float8 final_min = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    if (final_min == NULL)
        PG_RETURN_NULL();
    //elog(NOTICE,"min is %f\n",final_min);
    char buf[64];
    Numeric result;
    Datum numeric_datum;
    snprintf(buf, sizeof(buf), "%.2f", final_min);
    numeric_datum = DirectFunctionCall3(numeric_in,
                                        CStringGetDatum(buf),
                                        ObjectIdGetDatum(InvalidOid),
                                        Int32GetDatum(-1));

    result = DatumGetNumeric(numeric_datum);
    PG_RETURN_NUMERIC(result);
}


Datum
min_agg_init_4(PG_FUNCTION_ARGS)
{
    PG_RETURN_NULL();
}


Datum
min_agg_trans_4(PG_FUNCTION_ARGS)
{
    TimingArray *t_call = new TimingArray(call, row);
    float8 state = PG_ARGISNULL(0) ? NULL :PG_GETARG_FLOAT8(0);
    Numeric value = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERIC(1);
    Datum float8_datum = DirectFunctionCall1(numeric_float8, NumericGetDatum(value));
    double new_value = DatumGetFloat8(float8_datum);
    
    TimingArray *t_exec = new TimingArray(execution, row);
    if(new_value<state)state=new_value;
    delete t_exec;
    delete t_call;
    ++row;
    //if(row%1000==0)elog(NOTICE,"row is %d\n",row);
    if(row==801000)print_time();
    PG_RETURN_FLOAT8(state);
}


Datum
min_agg_combine_4(PG_FUNCTION_ARGS)
{
    float8 state1 = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    float8 state2 = PG_ARGISNULL(1) ? NULL : PG_GETARG_FLOAT8(1);

    if (state1 == NULL)
        PG_RETURN_NUMERIC(state2);
    if (state2 == NULL)
        PG_RETURN_NUMERIC(state1);
    if(state1<state2)PG_RETURN_FLOAT8(state1);
    else PG_RETURN_FLOAT8(state2);
}

Datum
min_agg_final_4(PG_FUNCTION_ARGS)
{
    float8 final_min = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    if (final_min == NULL)
        PG_RETURN_NULL();
    //elog(NOTICE,"min is %f\n",final_min);
    char buf[64];
    Numeric result;
    Datum numeric_datum;
    snprintf(buf, sizeof(buf), "%.2f", final_min);
    numeric_datum = DirectFunctionCall3(numeric_in,
                                        CStringGetDatum(buf),
                                        ObjectIdGetDatum(InvalidOid),
                                        Int32GetDatum(-1));

    result = DatumGetNumeric(numeric_datum);
    PG_RETURN_NUMERIC(result);
}



Datum
product_agg_init_1(PG_FUNCTION_ARGS)
{
    PG_RETURN_NULL();
}


Datum
product_agg_trans_1(PG_FUNCTION_ARGS)
{
    //int segment_id = GpIdentity.segindex;
    //elog(NOTICE,"segment is %d\n",segment_id);
    std::string funcName("product");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "product";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    float8 state = PG_ARGISNULL(0) ? NULL :PG_GETARG_FLOAT8(0);
    Numeric value = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERIC(1);
    Datum float8_datum = DirectFunctionCall1(numeric_float8, NumericGetDatum(value));
    double new_value = DatumGetFloat8(float8_datum);
    Numeric discount = PG_ARGISNULL(2) ? NULL : PG_GETARG_NUMERIC(2);
    Datum float8_datum1 = DirectFunctionCall1(numeric_float8, NumericGetDatum(discount));
    double new_value1 = DatumGetFloat8(float8_datum1);
    int arrayLength=2;
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 5;
    uint32 argv[5]; 
    
    wasm_wrapper_t *wasm = func_map.at(funcName);
    double *min=(double *)(wasm->native2);
    min[3]=state;
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+3);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, &new_value, 8);
    memcpy(wasm->native+8, &new_value1, 8);
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;
    argv[2] = arrayLength;  
    //PG_RETURN_VOID();
    if(!runtime_call_wasm_func(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_exec[row]=time[0];
    copy1[row]=time[1];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[2]);
    delete t_exec;
    delete t_call;
    ++row;
    //if(row%10==0)elog(NOTICE,"row is %d\n",row);
    if(row==550)print_time();
    PG_RETURN_FLOAT8(min[3]);
}


Datum
product_agg_combine_1(PG_FUNCTION_ARGS)
{
    float8 state1 = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    float8 state2 = PG_ARGISNULL(1) ? NULL : PG_GETARG_FLOAT8(1);

    if (state1 == NULL)PG_RETURN_FLOAT8(state2);
    if (state2 == NULL)PG_RETURN_FLOAT8(state1);
    PG_RETURN_FLOAT8(state1+state2);
}

Datum
product_agg_final_1(PG_FUNCTION_ARGS)
{
    float8 final_min = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    if (final_min == NULL)
        PG_RETURN_NULL();
    PG_RETURN_FLOAT8(final_min);
}




Datum
product_agg_init_2(PG_FUNCTION_ARGS)
{
    PG_RETURN_NULL();
}


Datum
product_agg_trans_2(PG_FUNCTION_ARGS)
{
    //int segment_id = GpIdentity.segindex;
    //elog(NOTICE,"segment is %d\n",segment_id);
    std::string funcName("product");
    if(func_map.find(funcName)==func_map.end())
    {
        RuntimeInitArgs init_args;
        memset(&init_args, 0, sizeof(RuntimeInitArgs));
        static char global_heap_buf[1024*1024];  //[4096*1024*2]  
        init_args.mem_alloc_type = Alloc_With_Pool;
        init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
        init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
        if(runtime_full_init(&init_args)){
        //elog(INFO, "wamr runtime init successfully!");
        }    
        else elog(ERROR, "wamr runtime init failed!");
        wasm_wrapper_t* wasm = new wasm_wrapper_t;
        std::string file_name = "";
        std::string base = "/home/xiaqian/wasm-udf/performance/udf/cpp/wasm/";
        //std::string base = "/var/lib/postgresql/wasm/";
        std::string func_name = "product";
        if(file_name == ""){
            file_name = base + func_name + ".aot";
        }   
        std::string func_name_with_ser = func_name + "_with_ser";
        elog(NOTICE, "wasm file: %s func: %s func_with_ser: %s", file_name.c_str(), func_name.c_str(), func_name_with_ser.c_str());
        if(!runtime_load_mod(wasm, file_name.c_str()))
            elog(ERROR, "wasm runtime load module failed!");
        else if(!runtime_instantiate_mod(wasm))
            elog(ERROR, "wasm runtime instantiate module failed!");
        else if(!runtime_create_mod_exec_env(wasm))
            elog(ERROR, "wasm runtime create exec env failed!");
        else if(!runtime_lookup_wasm_func(wasm, func_name.c_str(), func_name_with_ser.c_str())){
            elog(ERROR, "wasm runtime lookup wasm func failed!");
        } 
        else if(!runtime_mod_malloc(wasm)){
            elog(ERROR, "wasm runtime malloc failed");
        }
        else {
            func_map.insert(std::make_pair(std::string(func_name), wasm));
            // elog(INFO, "wasm instantiate successfully");
            // for(auto p : func_map){
            //     elog(INFO, "func name: %s instance addr: %p\n", p.first.c_str(), p.second);
            // }
        }
    }
    TimingArray *t_call = new TimingArray(call, row);
    float8 state = PG_ARGISNULL(0) ? NULL :PG_GETARG_FLOAT8(0);
    Numeric value = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERIC(1);
    Datum float8_datum = DirectFunctionCall1(numeric_float8, NumericGetDatum(value));
    double new_value[2];
    new_value[0] = DatumGetFloat8(float8_datum);
    Numeric discount = PG_ARGISNULL(2) ? NULL : PG_GETARG_NUMERIC(2);
    Datum float8_datum1 = DirectFunctionCall1(numeric_float8, NumericGetDatum(discount));
    new_value[1] = DatumGetFloat8(float8_datum1);
    int arrayLength=2;
    TimingArray *t_exec = new TimingArray(execution, row);
    int argc = 4;
    uint32 argv[4]; 
    
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder1(1024*1024);
    auto vec1 = builder1.CreateVector(new_value, arrayLength);
    auto root1 = CreateDArray(builder1, vec1);
    FinishDArrayBuffer(builder1, root1);
    // 获取缓冲区地址和长度
    uint8_t *buf1 = builder1.GetBufferPointer();
    int size1 = builder1.GetSize();

    delete t_trans;

    wasm_wrapper_t *wasm = func_map.at(funcName);
    double *min=(double *)(wasm->native2);
    min[4]=state;
    clock_t ida = CLOCK_MONOTONIC;
    struct timespec tb;
    clock_gettime(ida, &tb);
    uint64_t* time_begin=(uint64_t*)(argv+2);
    time_begin[0]=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    
    memcpy(wasm->native, buf1, size1);
    argv[0] = wasm->buffer;
    argv[1] = wasm->buffer2;
    //PG_RETURN_VOID();
    if(!runtime_call_wasm_func_with_ser(wasm, argc, argv))
        elog(ERROR, "call wasm function falied");
    
    int64_t *time=(int64_t *)(wasm->native2);
    //int8_t *time=(int8_t *)(result_buf);
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    clock_gettime(ida, &tb);
    uint64_t receive=(uint64_t)tb.tv_sec * 1000000000LL + tb.tv_nsec;
    copy2[row]=(int64_t)(receive-time[3]);
    delete t_exec;
    delete t_call;
    ++row;
    //if(row%10==0)elog(NOTICE,"row is %d\n",row);
    if(row==550)print_time();
    PG_RETURN_FLOAT8(min[4]);
}


Datum
product_agg_combine_2(PG_FUNCTION_ARGS)
{
    float8 state1 = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    float8 state2 = PG_ARGISNULL(1) ? NULL : PG_GETARG_FLOAT8(1);

    if (state1 == NULL)PG_RETURN_FLOAT8(state2);
    if (state2 == NULL)PG_RETURN_FLOAT8(state1);
    PG_RETURN_FLOAT8(state1+state2);
}

Datum
product_agg_final_2(PG_FUNCTION_ARGS)
{
    float8 final_min = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    if (final_min == NULL)
        PG_RETURN_NULL();
    PG_RETURN_FLOAT8(final_min);
}




Datum
product_agg_init_3(PG_FUNCTION_ARGS)
{
    PG_RETURN_NULL();
}


Datum
product_agg_trans_3(PG_FUNCTION_ARGS)
{
    TimingArray *t_call = new TimingArray(call, row);  
    float8 state = PG_ARGISNULL(0) ? NULL :PG_GETARG_FLOAT8(0);
    Numeric value = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERIC(1);
    Datum float8_datum = DirectFunctionCall1(numeric_float8, NumericGetDatum(value));
    double new_value[2];
    new_value[0] = DatumGetFloat8(float8_datum);
    Numeric discount = PG_ARGISNULL(2) ? NULL : PG_GETARG_NUMERIC(2);
    Datum float8_datum1 = DirectFunctionCall1(numeric_float8, NumericGetDatum(discount));
    new_value[1] = DatumGetFloat8(float8_datum1);
    int arrayLength=2;
    TimingArray *t_exec = new TimingArray(execution, row);
    TimingArray *t_trans = new TimingArray(data_trans, row);
    flatbuffers::FlatBufferBuilder builder1(1024*1024);
    auto vec1 = builder1.CreateVector(new_value, arrayLength);
    auto root1 = CreateDArray(builder1, vec1);
    FinishDArrayBuffer(builder1, root1);
    // 获取缓冲区地址和长度
    uint8_t *buf1 = builder1.GetBufferPointer();
    int size1 = builder1.GetSize();

    delete t_trans;
    
    char send_buf[1000000];//assume array length less than 50000
    clock_t id = CLOCK_MONOTONIC;
     
    clock_gettime(id, (struct timespec *)send_buf);
    double* arraylen=(double*)(send_buf+16);
    arraylen[0]=state;
    memcpy(send_buf+24,buf1,size1);
   
    //TimingPrint *send_time = new TimingPrint("send");
 // 动态生成路径
    int segment_id = GpIdentity.segindex;
    char host_to_docker_path[128];
    char docker_to_host_path[128];
    char host_to_docker_signal[64];
    char docker_to_host_signal[64];
    snprintf(host_to_docker_path, sizeof(host_to_docker_path), "/mnt/ramdisk_segment_%d/host_to_docker", segment_id);
    snprintf(docker_to_host_path, sizeof(docker_to_host_path), "/mnt/ramdisk_segment_%d/docker_to_host", segment_id);
    snprintf(host_to_docker_signal, sizeof(host_to_docker_signal), "/mnt/ramdisk_segment_%d/host_to_docker_signal", segment_id);
    snprintf(docker_to_host_signal, sizeof(docker_to_host_signal), "/mnt/ramdisk_segment_%d/docker_to_host_signal", segment_id);
    // 写入buffer到host_to_docker
    //elog(NOTICE, "Host to Docker path: %s", host_to_docker_path);
    //elog(NOTICE, "Docker to Host path: %s", docker_to_host_path);
    write_to_file(host_to_docker_path, host_to_docker_signal, (char *)send_buf, 24+size1);
    //elog(ERROR, "can run here\n");
    //elog(NOTICE,"Buffer sent to Docker.\n");
    //delete send_time;
    char recv_buf[4096];
    //TimingPrint *recv_time = new TimingPrint("recv");
    // 从docker_to_host读取buffer
    read_from_file(docker_to_host_path,docker_to_host_signal, recv_buf, sizeof(recv_buf));
    //elog(NOTICE,"Buffer received from Docker.\n");
    struct timespec *tc=(struct timespec *)(recv_buf+24);
    struct timespec td;
    clock_gettime(id, &td);
    copy2[row]=(td.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + td.tv_nsec - tc->tv_nsec;//第二次copy时间+
    //delete recv_time;
    
    int64_t *time=(int64_t *)recv_buf;
    inner_diser[row]=time[0];
    inner_exec[row]=time[1];
    copy1[row]=time[2];
    delete t_exec;
    delete t_call;
    ++row;  
    double* temp=(double*)(recv_buf+40);
    if(row==550)print_time();
    PG_RETURN_FLOAT8(temp[0]);
}


Datum
product_agg_combine_3(PG_FUNCTION_ARGS)
{
    float8 state1 = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    float8 state2 = PG_ARGISNULL(1) ? NULL : PG_GETARG_FLOAT8(1);

    if (state1 == NULL)PG_RETURN_FLOAT8(state2);
    if (state2 == NULL)PG_RETURN_FLOAT8(state1);
    PG_RETURN_FLOAT8(state1+state2);
}

Datum
product_agg_final_3(PG_FUNCTION_ARGS)
{
    float8 final_min = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    if (final_min == NULL)
        PG_RETURN_NULL();
    PG_RETURN_FLOAT8(final_min);
}

Datum
product_agg_init_4(PG_FUNCTION_ARGS)
{
    PG_RETURN_NULL();
}


Datum
product_agg_trans_4(PG_FUNCTION_ARGS)
{
    TimingArray *t_call = new TimingArray(call, row);
    float8 state = PG_ARGISNULL(0) ? NULL :PG_GETARG_FLOAT8(0);
    Numeric value = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERIC(1);
    Datum float8_datum = DirectFunctionCall1(numeric_float8, NumericGetDatum(value));
    double new_value = DatumGetFloat8(float8_datum);
    Numeric discount = PG_ARGISNULL(2) ? NULL : PG_GETARG_NUMERIC(2);
    Datum float8_datum1 = DirectFunctionCall1(numeric_float8, NumericGetDatum(discount));
    double new_value1 = DatumGetFloat8(float8_datum1);
    TimingArray *t_exec = new TimingArray(execution, row);
    state+=new_value*(1-new_value1);
    delete t_exec;
    delete t_call;
    ++row;
    //if(row%10==0)elog(NOTICE,"row is %d\n",row);
    if(row==550)print_time();
    PG_RETURN_FLOAT8(state);
}


Datum
product_agg_combine_4(PG_FUNCTION_ARGS)
{
    float8 state1 = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    float8 state2 = PG_ARGISNULL(1) ? NULL : PG_GETARG_FLOAT8(1);

    if (state1 == NULL)PG_RETURN_FLOAT8(state2);
    if (state2 == NULL)PG_RETURN_FLOAT8(state1);
    PG_RETURN_FLOAT8(state1+state2);
}

Datum
product_agg_final_4(PG_FUNCTION_ARGS)
{
    float8 final_min = PG_ARGISNULL(0) ? NULL : PG_GETARG_FLOAT8(0);
    if (final_min == NULL)
        PG_RETURN_NULL();
    PG_RETURN_FLOAT8(final_min);
}