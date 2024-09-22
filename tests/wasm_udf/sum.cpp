#include "../../flatbuffer/include/long_array_generated.h"
#include <stdio.h>
#include<wasi/api.h>
//#define SHM_ADDR (void *)0x40000000
extern "C" {
int sum(void* start, int len,void* buf1,uint64_t time_begin){     // used by optimized wasm which can directly pass data
    __wasi_timestamp_t tc,ts,te;
    tc=( __wasi_timestamp_t)time_begin;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    int64_t sum1 = 0;
    //sum+=arr[100];
    int64_t *arr=(int64_t *)start;
    for(int i=0; i<len; ++i){
        sum1 += arr[i];
    }
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    int64_t *time=(int64_t *)buf1;
    time[0]=(int64_t)(te-ts);
    time[1]=(int64_t)(ts-tc);//第一次拷贝时间
    time[2]=(int64_t)te;
    // printf("sum :%d\n", sum);
    return sum1;
    //int *write_buffer=(int*)SHM_ADDR;
    //return write_buffer[0];
}


int sum_with_ser(void* buf,void* buf1,uint64_t time_begin){
    __wasi_timestamp_t tc,ts,te;
    tc=(__wasi_timestamp_t)time_begin;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    auto longArr = GetLongArray(buf);
    auto value = longArr->values();
    auto size = value->size();
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    int64_t *time=(int64_t *)buf1;
    time[0]=(int64_t)(te-ts);
    time[2]=(int64_t)(ts-tc);
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    int64_t sum = 0;
    for(int i=0; i<size; ++i){
        sum += value->Get(i);
    }
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    time[1]=(int64_t)(te-ts);
    time[3]=(int64_t)te;
    return sum;
}
}