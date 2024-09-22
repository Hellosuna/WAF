#include "year_month_day_generated.h"
#include <iostream>
#include "util/timing.hpp"

int main(){
    int num = 10;
    flatbuffers::FlatBufferBuilder builder(1024*10);
    timing t;
    t.start();
    for(int i=0; i<num; ++i){
        auto root = CreateYearMonthDay(builder, 1, 1, 1990);
        builder.Finish(root);
    }
    t.end();
}