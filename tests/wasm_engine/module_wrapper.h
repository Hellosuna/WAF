// #define KERNEL_VERSION_NUMBER 0x051500 
#include "wasm_export.h"
#include "bh_read_file.h"
#include <errno.h>
#include <sys/ipc.h>
#include </usr/local/gpdb/include/postgresql/server/utils/elog.h>
#include <string>
#include <sys/shm.h>
#define SHM_SIZE (1024 * 1024 * 1024UL) // 共享内存大小：1GB
#define SHM_ADDR (void *)0x80000000     //共享内存范围2-3G  
class Timing {
    struct timespec ts,te;
    clock_t id = CLOCK_MONOTONIC;
public:
    Timing(){
        clock_gettime(id, &ts);
    }
    Timing(const Timing&) = delete;
    Timing& operator=(const Timing&) = delete;
    long long getTimeValue(){
        clock_gettime(id, &te);
        long long val = (te.tv_sec - ts.tv_sec) * 1000 * 1000 * 1000 + te.tv_nsec - ts.tv_nsec;
	return val;
    }
    virtual ~Timing(){}

};
class TimingArray : public Timing {
private:
	long long *arr;
	int index;
public:	
    TimingArray(long long times[], int n):arr(times), index(n){
    }

    ~TimingArray(){
	arr[index] = getTimeValue();
    }
};

class TimingValue : public Timing {
private:
	long long &value;
public:
	TimingValue(long long &var):value(var){}
	~TimingValue(){
		value = getTimeValue();
	}
};

class TimingPrint : public Timing {
private:
     	std::string message;
public:
	TimingPrint(std::string from):message(from){}
	~TimingPrint(){	
	    elog(NOTICE,"%s: %ld ns", message.c_str(), getTimeValue());
	}
};

char *
bh_read_file_to_buffer_debug(const char *filename, uint32 *ret_size)
{
    char *buffer;
    int file;
    uint32 file_size, buf_size, read_size;
    struct stat stat_buf;

    if (!filename || !ret_size) {
        elog(NOTICE,"Read file to buffer failed: invalid filename or ret size.\n");
        return NULL;
    }
    /*filename = "/home/xiaqian/testfile";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        elog(ERROR, "could not open file \"%s\"", filename);
        return NULL;
    }*/
    if ((file = open(filename, O_RDONLY, 0666)) == -1) {
        elog(NOTICE,"Read file to buffer failed: open file %s failed.\n", filename);
	int err = errno;
	elog(NOTICE,"Error no: %d", err);
        return NULL;
    }

    if (fstat(file, &stat_buf) != 0) {
        elog(NOTICE,"Read file to buffer failed: fstat file %s failed.\n", filename);
        close(file);
        return NULL;
    }

    file_size = (uint32)stat_buf.st_size;

    /* At lease alloc 1 byte to avoid malloc failed */
    buf_size = file_size > 0 ? file_size : 1;

    if (!(buffer = (char*)os_malloc(buf_size))) {
        elog(NOTICE,"Read file to buffer failed: alloc memory failed.\n");
        close(file);
        return NULL;
    }
#if WASM_ENABLE_MEMORY_TRACING != 0
    elog(NOTICE,"Read file, total size: %u\n", file_size);
#endif

    read_size = (uint32)read(file, buffer, file_size);
    close(file);

    if (read_size < file_size) {
        elog(NOTICE,"Read file to buffer failed: read file content failed.\n");
        BH_FREE(buffer);
        return NULL;
    }

    *ret_size = file_size;
    return buffer;
} 

static bool run = false;

struct wasm_wrapper_t{
    char* buf = nullptr;
    uint32 buf_size;
    char error_buf[128];
    wasm_module_t module;
    uint32 wasm_stack_size = 8192*100;//8192*100
    uint32 wasm_heap_size = 8192*1024;//8192*1024
    wasm_module_inst_t module_inst;
    wasm_exec_env_t exec_env;
    wasm_function_inst_t func;
    wasm_function_inst_t func_with_ser;
    void* native;
    void* native2;
    uint32 buffer = 0;
    uint32 buffer2 = 0;
    int shm_key = 0;
};

bool runtime_full_init(RuntimeInitArgs *init_args){
    if (!wasm_runtime_full_init(init_args)) 
        return false;
    return true;
}

bool runtime_connect_shared_memory(wasm_wrapper_t* wasm_wrap,int key)
{
    TimingPrint("connect shared memory");
    int shmid;
    void *shm_addr;

    // 获取共享内存标识符
    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        int err = errno;
        ereport(ERROR, (errmsg("shmget error: %s", strerror(err))));
        return false;
    }

    // 连接共享内存到指定地址
    shm_addr = shmat(shmid, SHM_ADDR, 0);
    if (shm_addr == (void *)-1 || shm_addr != SHM_ADDR) {
        int err = errno;
        ereport(ERROR, (errmsg("shmat error: %s", strerror(err))));
        return false;
    }
    wasm_wrap->shm_key=shmid;
    //elog(NOTICE,"key is %d\n",wasm_wrap->shm_key);
    return true;
}

void runtime_free_mod(wasm_wrapper_t* wasm_wrap){
    if (wasm_wrap->exec_env)
        wasm_runtime_destroy_exec_env(wasm_wrap->exec_env);
    if (wasm_wrap->module_inst) {
        // if (wasm_buffer)
        //     wasm_runtime_module_free(module_inst, wasm_buffer);
        wasm_runtime_deinstantiate(wasm_wrap->module_inst);
    }
    if (wasm_wrap->module)
        wasm_runtime_unload(wasm_wrap->module);
    if (wasm_wrap->buf)
        BH_FREE(wasm_wrap->buf);
    wasm_runtime_destroy();
}

bool runtime_load_mod(wasm_wrapper_t *wasm_wrap, const char* file_name){
    //elog(NOTICE, "can run here\n");
    /*if(!run)
        return false;
    */
    TimingPrint("load module");
    
    wasm_wrap->buf = bh_read_file_to_buffer_debug(file_name, &wasm_wrap->buf_size);
    if(!wasm_wrap->buf){
        elog(ERROR, "read wasm file failed: %s", file_name);
        return false;
    }
    
    wasm_wrap->module = wasm_runtime_load(reinterpret_cast<uint8 *>(wasm_wrap->buf), wasm_wrap->buf_size,
        wasm_wrap->error_buf, sizeof(wasm_wrap->error_buf));
    
    if(!wasm_wrap->module){
        elog(NOTICE,"key is %d\n",wasm_wrap->shm_key);
        elog(ERROR, "load aot module failed\n");
        runtime_free_mod(wasm_wrap);
        return false;
    }
    
    return true;
}

bool runtime_instantiate_mod(wasm_wrapper_t* wasm_wrap){
    TimingPrint("instantiate module");
    //elog(NOTICE,"key is %d\n",wasm_wrap->shm_key);
    wasm_wrap->module_inst = wasm_runtime_instantiate(wasm_wrap->module,
        wasm_wrap->wasm_stack_size, wasm_wrap->wasm_heap_size, 
        wasm_wrap->error_buf, sizeof(wasm_wrap->error_buf),wasm_wrap->shm_key);
    if(!wasm_wrap->module_inst){
        runtime_free_mod(wasm_wrap);
        return false;
    }
    return true;
}

bool runtime_create_mod_exec_env(wasm_wrapper_t* wasm_wrap){
    TimingPrint("initialize exec env");
    wasm_wrap->exec_env = wasm_runtime_create_exec_env(wasm_wrap->module_inst, wasm_wrap->wasm_stack_size);
    if (!wasm_wrap->exec_env){
        runtime_free_mod(wasm_wrap);
        return false;
    }
    return true;
}

bool runtime_lookup_wasm_func(wasm_wrapper_t* wasm_wrap, const char* func_name, const char* func_name_with_ser){
    TimingPrint("lookup function");
    wasm_wrap->func = wasm_runtime_lookup_function(wasm_wrap->module_inst, func_name);
    if(!wasm_wrap->func){
        elog(ERROR, "lookup function %s falied", func_name);
        runtime_free_mod(wasm_wrap);
        return false;
    }
    wasm_wrap->func_with_ser = wasm_runtime_lookup_function(wasm_wrap->module_inst, func_name_with_ser);
    if(!wasm_wrap->func_with_ser){
        elog(ERROR, "lookup function %s falied", func_name_with_ser);
        runtime_free_mod(wasm_wrap);
        return false;
    }
    return true;
}

bool runtime_mod_malloc(wasm_wrapper_t* wasm_wrap){
    TimingPrint("malloc buffer");
    wasm_wrap->buffer = wasm_runtime_module_malloc(wasm_wrap->module_inst, 1024*2560, &wasm_wrap->native);//1024*512
    if(wasm_wrap->buffer == 0)
        return false;
    wasm_wrap->buffer2 = wasm_runtime_module_malloc(wasm_wrap->module_inst, 1024*2560, &wasm_wrap->native2);
    if(wasm_wrap->buffer2 == 0)
        return false;
    return true;
}

bool runtime_call_wasm_func(wasm_wrapper_t* wasm_wrap, uint32 argc, uint32 argv[]){
    if(!wasm_runtime_call_wasm(wasm_wrap->exec_env, wasm_wrap->func, argc, argv)){
        runtime_free_mod(wasm_wrap);
        elog(ERROR, "call wasm error: %s", wasm_runtime_get_exception(wasm_wrap->module_inst));
        return false;
    }
    return true;
}

bool runtime_call_wasm_func_with_ser(wasm_wrapper_t* wasm_wrap, uint32 argc, uint32 argv[]){
    if(!wasm_runtime_call_wasm(wasm_wrap->exec_env, wasm_wrap->func_with_ser, argc, argv)){
        runtime_free_mod(wasm_wrap);
        elog(ERROR, "call wasm error: %s", wasm_runtime_get_exception(wasm_wrap->module_inst));
        return false;
    }
    return true;
}
