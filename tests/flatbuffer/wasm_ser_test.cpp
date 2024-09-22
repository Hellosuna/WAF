#include "module_wrapper.h"
#include <string>
#include <sys/shm.h>
#include "include/int_array_generated.h"
#include "include/point_generated.h"
#include "include/flat_str_generated.h"
#include "include/flat_str_arr_generated.h"



#define SHM_KEY 2345                    // 共享内存键值
#define SHM_SIZE (1024 * 1024 * 1024UL) // 共享内存大小：1MB
#define SHM_ADDR (void *)0x80000000     // 2G地址
int main(int argc, char* argv[]){

    if(argc < 3){
        printf("please specify <wasm_file_name> [serialization] and <loop_times>\n");
        exit(0);
    }
    int shmid;
    void *shm_addr;
    // 获取共享内存标识符
    shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        printf("shmget error\n");
        return 0;
    }

    // 连接共享内存到指定地址
    shm_addr = shmat(shmid, SHM_ADDR, 0);
    if (shm_addr == (void *)-1 || shm_addr != SHM_ADDR) {
        printf("shmat error\n");
        return 0;
    }

    // std::string wasmBasePath = "/home/xq/Desktop/wasm-udf/performance/udf/wasm/";
    std::string wasmBasePath = "/home/xq/Desktop/wasm-udf/performance/udf/cpp/wasm/";
    std::string fileName = wasmBasePath + argv[1] + ".aot";
    std::string funcName;
    int times;
    if(argc == 3){
        funcName = std::string(argv[1]) + "_with_data_prepared";
        times = atoi(argv[2]);
    }else if(argc == 4){
        funcName = std::string(argv[1]) + "_serialize_with_" + argv[2];
        times = atoi(argv[3]);
    }
    printf("fileName: %s funcName: %s loopTimes: %d\n", fileName.c_str(), funcName.c_str(), times);
    
    
    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));
    static char global_heap_buf[4096 * 1024];
    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    ModuleWrap moduleWrap(fileName.c_str(), 8192*10, 0x40000000, shmid);

    moduleWrap.runtime_full_init(&init_args);
    moduleWrap.init();
    moduleWrap.lookup_function(funcName.c_str());
    int BUF_MAX_LEN = 8192;
    void *native1;
    void *native2;
    uint32_t buffer1 = wasm_runtime_module_malloc(moduleWrap.module_inst, BUF_MAX_LEN, &native1);
    uint32_t buffer2 = wasm_runtime_module_malloc(moduleWrap.module_inst, BUF_MAX_LEN, &native2);

    uint32 wasm_argc;
    uint32 wasm_argv[] = {1,2,3};
    flatbuffers::FlatBufferBuilder builder(1024);
    flatbuffers::FlatBufferBuilder builder2(1024);
    std::string tmp(argv[1]);
    if(tmp == "sum_array"){
        wasm_argc = 2;
        wasm_argv[0] = buffer1;
        wasm_argv[1] = times;
        int arr[] = {0,1,2,3,4,5,6,7,8,9};
        int len = sizeof(arr)/sizeof(int);
            
        // 开始构建IntArray表
        auto vec = builder.CreateVector(arr, len);
        auto root = CreateIntArray(builder, vec);
        FinishIntArrayBuffer(builder, root);
        // 获取缓冲区地址和长度
        uint8_t *buf = builder.GetBufferPointer();
        int size = builder.GetSize();
        memcpy(native1, buf, size);
        moduleWrap.call_wasm(wasm_argc, wasm_argv, 1);
        // moduleWrap.call_wasm(wasm_argc, wasm_argv, times);
        moduleWrap.free();
    }else if(tmp == "split_str"){
        wasm_argc = 2;
        wasm_argv[0] = buffer1;
        wasm_argv[1] = times;
        const char* str = "Hello\x01World\x01\x01Example";

        {

                // auto str1 = builder.CreateString(str);
                // auto flat_str = CreateFlatStr(builder, str1);
                // builder.Finish(flat_str);
                // uint8_t *buf = builder.GetBufferPointer();
                // size_t size = builder.GetSize();
                // memcpy(native1, buf, size);
                // // moduleWrap.call_wasm(wasm_argc, wasm_argv, times);
                // moduleWrap.call_wasm(wasm_argc, wasm_argv, 1);


                // builder.Reset();
                int count =3;
                char*res[] = {"Hello","World", "Example"};
                flatbuffers::FlatBufferBuilder builder;
                    // 创建一个包含多个字符串的向量
                std::vector<flatbuffers::Offset<flatbuffers::String>> message_vector;
                for(int i=0; i<count; ++i){
                    message_vector.push_back(builder.CreateString(res[i]));
                }
                auto messages = builder.CreateVector(message_vector);
                auto r = CreateFlatStrArr(builder, messages);
                builder.Finish(r);
                uint8_t *buf = builder.GetBufferPointer();
                size_t size = builder.GetSize();
                Timing t;
                for(int i=0; i<times; ++i){
                    auto root = GetFlatStrArr(buf);
                    auto str = root->str_arr();
                }

        }

    }else if(tmp == "cal_two_dis"){
        wasm_argc = 3;
        wasm_argv[0] = buffer1;
        wasm_argv[1] = buffer2;
        wasm_argv[2] = times;

        {
            // Timing t;
            // for(int i=0; i<times; ++i){
                auto point1 = CreatePoint(builder, 1.0, 2.0);
                builder.Finish(point1);
                uint8_t *buf1 = builder.GetBufferPointer();
                size_t size1 = builder.GetSize();
                // printf("point size: %d \n", size);
                auto point2 = CreatePoint(builder2, 4.0, 6.0);
                builder2.Finish(point2);
                uint8_t *buf2 = builder2.GetBufferPointer();
                size_t size2 = builder2.GetSize();
                memcpy(native1, buf1, size1);
                memcpy(native2, buf2, size2);
                // auto r1 = GetPoint(native1);
                // printf("r1 :%f\n", r1->x());
                moduleWrap.call_wasm(wasm_argc, wasm_argv, 1);

                // moduleWrap.call_wasm(wasm_argc, wasm_argv, times);
                // builder.Reset();
            // }
            // moduleWrap.call_wasm(wasm_argc, wasm_argv, 1);
            
            // moduleWrap.free();
        }
    }



    // wrap
}