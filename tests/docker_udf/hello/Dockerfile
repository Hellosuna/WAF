FROM frolvlad/alpine-gcc

LABEL desc="c/c++ udf image test"

RUN mkdir /udf
 
COPY hello.c /udf
 
WORKDIR /udf
 
RUN gcc hello.c
 
CMD ["./a.out"]