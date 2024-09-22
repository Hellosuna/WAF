# FROM frolvlad/alpine-gcc
FROM ubuntu:22.04
#FROM gcc:latest
LABEL desc="count"

# Expose the port the application is listening on
EXPOSE 4888

RUN mkdir /udf
 
# COPY sum_of_array.c /udf
 
WORKDIR /udf
 
# RUN gcc sum_of_array.c
 
# CMD ["./a.out"]
# Set environment variables for logging
ENV LOG_LEVEL=INFO

COPY count /udf

CMD ["./count"]