#!/bin/bash

# 检查 qgen 是否存在
if ! command -v ./qgen &> /dev/null; then
    echo "qgen 命令未找到，请确保 qgen 可执行文件在当前目录。"
    exit 1
fi

# 生成 1 到 22 的 SQL 查询文件
for i in {1..22}; do
    ./qgen -d $i > "d${i}.sql"
    echo "生成 d${i}.sql 完成。"
    
done

echo "所有 SQL 查询文件生成完毕。"


