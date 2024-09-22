#include "../../flatbuffer/include/pair_array_generated.h"
#include "../../flatbuffer/include/long_array_generated.h"
#include <stdio.h>
#include<vector>
#include<wasi/api.h>
extern "C" {
typedef struct {
    int64_t first;
    int64_t second;
} couple;
void* make_pairs_with_ser(void* buf,void* buf1,uint64_t time_begin){
    __wasi_timestamp_t tc,ts,te;
    tc=( __wasi_timestamp_t)time_begin;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    auto longArr = GetLongArray(buf);
    auto value = longArr->values();
    auto size = value->size();
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    int64_t *time=(int64_t *)buf1;
    time[0]=(int64_t)(te-ts);
    time[3]=(int64_t)(ts-tc);//第一次拷贝时间
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    int len=size*(size-1)/2;
    couple *twins=(couple *)malloc(len*sizeof(couple));
    int k=0;
    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            twins[k].first=value->Get(i);
            twins[k].second=value->Get(j);
            k++;
        }
    }
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    time[1]=(int64_t)(te-ts);
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    flatbuffers::FlatBufferBuilder builder(1024);
    std::vector<flatbuffers::Offset<Pair>> pair_vector;
    for (int i = 0; i < len; ++i) {
        auto pair = CreatePair(builder, twins[i].first, twins[i].second);
        pair_vector.push_back(pair);
    }
      
    //flatbuffers::Offset<Pair> *pair_vector=(flatbuffers::Offset<Pair> *)malloc(len*sizeof(flatbuffers::Offset<Pair>));
    //flatbuffers::Offset<Pair> pair_vector[1000];
    /*for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            auto pair = CreatePair(builder, value->Get(i), value->Get(j));
            pair_vector.push_back(pair);
            //pair_vector[k]=pair;
            //k++;
        }
    }*/
    auto pairs_vec = builder.CreateVector(pair_vector);
    auto pair_array = CreatePairArray(builder, pairs_vec);
    builder.Finish(pair_array);
    uint8_t *return_buf = builder.GetBufferPointer();
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    time[2]=(int64_t)(te-ts);
    time[4]=(int64_t)te;
    return (void*)return_buf;
    //return (void*)time;
}
couple* make_pairs(const int64_t* arr, int n,void* buf1,uint64_t time_begin) {
    __wasi_timestamp_t tc,ts,te;
    tc=( __wasi_timestamp_t)time_begin;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    int pairCount = n * (n - 1) / 2;
    couple* result = (couple*)malloc(pairCount * sizeof(couple));
    
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    int index = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            result[index].first = arr[i];
            result[index].second = arr[j];
            ++index;
        }
    }
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    int64_t *time=(int64_t *)buf1;
    time[0]=(int64_t)(te-ts);
    time[1]=(int64_t)(ts-tc);//第一次拷贝时间
    time[2]=(int64_t)te;
    return result;
}
}