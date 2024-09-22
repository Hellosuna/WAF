<<<<<<< HEAD
# tests
## 一、 数据生成
本次测试6个UDF，相关查询的表数据来自TPCx-BB和TPC-H两个基准，TPC测试包可在其官网(https://www.tpc.org/)获取。
### 1. TPC-H
下载代码后，需要先编译出可执行文件dbgen，然后生成表数据，可参考文档(https://blog.csdn.net/qq_38688267/article/details/122535189)  
`dbgen -f -s 1`   //表示生成1G数据  
### 2. TPCx-BB
TPCx-BB的生成数据需要在hadoop+hive/spark环境下，部署流程不再赘述，此外还要设置一些环境变量以和hadoop,hive绑定  
`bigBench runBenchmark -f 1 -i CLEAN_DATA,DATA_GENERATION`   //表示生成1G数据，只运行数据清理和数据生成阶段
## 二、相关环境
### 1. 数据库
搭建GreenPlum环境(https://github.com/greenplum-db/gpdb-archive)，完成后就可以将上述生成的表数据导入数据库了(我们的greenplum是在本地搭建的，1个协调器，2个段)
### 2. wasm环境
使用的wasm编译器是我们定制的clang，wasm运行时是加入了共享内存的wamr
### 3. docker安装
### 4. 序列化工具
使用谷歌的flatbuffer完成序列化和反序列化工作(https://github.com/google/flatbuffers)
## 三、UDF函数编写
GreenPlum允许使用多种语言编写扩展，我们使用C/C++，我们定义了wasm_engine扩展，处理与wasm或容器的交互  
`cd tests/wasm_engine`  
`make`  
`make install` //安装扩展  
这里要注意些一些库的链接，见makefile  
在tests/wasm_engine/wasm_engine.cpp中，我们可以看到每个函数逻辑都有四种实现接口，分别是native、docker序列化、wasm序列化和wasm共享内存  
### 1. native
原生代码，函数逻辑直接在wasm_engine.cpp中定义，见_native后缀函数
### 2. docker udf
在tests/docker_udf中，将6个UDF打包成6个镜像  
如构建sum镜像  
`cd sum`  
`docker build -t sum ./`  
我们使greenplum服务器和容器通过内存文件通信，因此要创建一个内存文件系统，并在容器启动时挂载上去  
`sudo mount -t tmpfs -o size=64M tmpfs /mnt/ramdisk_segment_0`  
`docker run -it  --hostname vkernel --network host -v /mnt/ramdisk_segment_0:/mnt/ramdisk sum`  
greenplum查询分布在多个段节点上，为避免段节点争抢内存文件，只好为每个段节点都创建和挂载了一个tmpfs(段后缀-1~1)
与这些容器通信的是wasm_engine.cpp中的_with_docker后缀函数
### 3. wasm udf
在tests/wasm_udf中，有6个函数(.cpp)，使用clang将其编译为wasm  
`cmake ./`  
`make`  
使用wamrc将其编译为aot  
`cd wasm`  
`make`  
解析这些aot文件，并进行wasm模块验证、实例化、调用这些wasm函数的是wasm_engine.cpp中的_share和_with_ser后缀函数
可以看到，在tests/wasm_udf下的每个cpp文件中都有两个函数，一个是共享内存方式传输数据，另一个使用flatbuffer序列化/反序列化传输数据
## 四、查询语句执行
在tests/sql中有6条sql查询语句(从TPC-H和TPCx-BB中摘抄出来)  
`psql -p 15432 -d postgres -f tests/sql/sum.sql`   
查询执行完成后会将UDF执行过程中每个部分的时间消耗以日志形式打印出来