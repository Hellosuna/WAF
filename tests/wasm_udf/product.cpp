#include "../../flatbuffer/include/long_array_generated.h"
#include "../../flatbuffer/include/double_array_generated.h"
#include <stdio.h>
#include<wasi/api.h>
extern "C" {
double product(void* buf1,void* buf2,int len,uint64_t time_begin){     // used by optimized wasm which can directly pass data
    __wasi_timestamp_t tc,ts,te;
    tc=( __wasi_timestamp_t)time_begin;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    double* sum=(double*)buf2;
    //sum+=arr[100];
    double *arr=(double *)buf1;
    sum[3]+=arr[0]*(1-arr[1]);
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    int64_t *time=(int64_t *)buf2;
    time[0]=(int64_t)(te-ts);
    time[1]=(int64_t)(ts-tc);//第一次拷贝时间
    time[2]=(int64_t)te;
    //printf("sum :%f\n", sum);
    return 0;
}


double product_with_ser(void* buf,void* buf1,uint64_t time_begin){
    __wasi_timestamp_t tc,ts,te,ts1,te1;
    tc=( __wasi_timestamp_t)time_begin;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    auto doubleArr = GetDArray(buf);
    auto value2 = doubleArr->values();
    auto size2 = value2->size();
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    
    
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts1);
    double* sum=(double*)buf1;
    sum[4]+=(value2->Get(0))*(1-value2->Get(1));
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te1);
    int64_t *time=(int64_t *)buf1;
    time[0]=(int64_t)(te-ts);
    time[1]=(int64_t)(te1-ts1);
    time[2]=(int64_t)(ts-tc);
    time[3]=(int64_t)te1;
    return 0;
}

}