#include "../../flatbuffer/include/long_array_generated.h"
#include "../../flatbuffer/include/double_array_generated.h"
#include <stdio.h>
#include<wasi/api.h>
extern "C" {
int min(void* start, int len,void* buf1,uint64_t time_begin){     // used by optimized wasm which can directly pass data
    __wasi_timestamp_t tc,ts,te;
    tc=( __wasi_timestamp_t)time_begin;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    double* min=(double*)buf1;
    //sum+=arr[100];
    double *arr=(double *)start;
    for(int i=0; i<len; ++i){
        if(arr[i]<min[3])min[3]=arr[i];
    }
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    int64_t *time=(int64_t *)buf1;
    time[0]=(int64_t)(te-ts);
    time[1]=(int64_t)(ts-tc);//第一次拷贝时间
    time[2]=(int64_t)te;
    // printf("sum :%d\n", sum);
    return 0;
}


double min_with_ser(void* buf,void* buf1,uint64_t time_begin){
    __wasi_timestamp_t tc,ts,te;
    tc=( __wasi_timestamp_t)time_begin;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    auto doubleArr = GetDArray(buf);
    auto value = doubleArr->values();
    auto size = value->size(); 
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    int64_t *time=(int64_t *)buf1;
    time[0]=(int64_t)(te-ts);
    time[2]=(int64_t)(ts-tc);
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    double* min=(double*)buf1;
    for(int i=0; i<size; ++i){
        if(min[4]>(value->Get(i)))min[4]=value->Get(i);
    }
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    time[1]=(int64_t)(te-ts);
    time[3]=(int64_t)te;
    return min[4];
}
}