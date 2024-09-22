# FROM frolvlad/alpine-gcc
#FROM ubuntu:20.04
FROM gcc:latest
LABEL desc="sum_of_array"

RUN mkdir /udf
 
# COPY sum_of_array.c /udf
 
WORKDIR /udf
 
# RUN gcc sum_of_array.c
 
# CMD ["./a.out"]

COPY sum_of_array /udf

CMD ["./sum_of_array"]