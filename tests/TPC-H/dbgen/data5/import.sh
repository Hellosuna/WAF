#!/bin/bash
psql -p 15432 -d postgres -f ../delete.sql
# 设置数据库连接信息
DB_NAME="postgres"
DB_PORT="15432"

# 为每个.tbl文件导入数据
for tbl_file in *.tbl; do
    # 根据文件名确定表名（假设文件名格式为 table_name.tbl）
    table_name=$(basename "$tbl_file" .tbl)
    
    # 使用COPY命令导入数据
    psql -d $DB_NAME  -p $DB_PORT -c "\COPY $table_name FROM '$tbl_file' DELIMITER '|' CSV"
done
