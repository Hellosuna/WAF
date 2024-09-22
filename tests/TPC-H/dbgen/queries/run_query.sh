#!/bin/bash

# Define the database and port
DB="postgres"
PORT="15432"
QUERY_PATH="/home/xiaqian/TPC-H/dbgen/queries"

# Loop through the files d1.sql to d22.sql and execute each one
for i in {1..22}
do
  FILENAME="d${i}.sql"
  psql -p $PORT -d $DB -f "${QUERY_PATH}/${FILENAME}"
done
