

/*
CREATE FUNCTION print_time() RETURNS void
AS 'MODULE_PATHNAME', 'print_time'
LANGUAGE C STRICT;
*/

CREATE FUNCTION sum_native(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'sum_native'
LANGUAGE C STRICT;

CREATE FUNCTION sum_share(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'sum_share'
LANGUAGE C STRICT;

CREATE FUNCTION sum_with_ser(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'sum_with_ser'
LANGUAGE C STRICT;

CREATE FUNCTION sum_with_docker(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'sum_with_docker'
LANGUAGE C STRICT;

CREATE FUNCTION min_share(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'min_share'
LANGUAGE C STRICT;

CREATE FUNCTION min_with_ser(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'min_with_ser'
LANGUAGE C STRICT;

CREATE FUNCTION min_with_docker(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'min_with_docker'
LANGUAGE C STRICT;

CREATE FUNCTION count_native(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'count_native'
LANGUAGE C STRICT;

CREATE FUNCTION count_share(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'count_share'
LANGUAGE C STRICT;

CREATE FUNCTION count_with_ser(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'count_with_ser'
LANGUAGE C STRICT;

CREATE FUNCTION count_with_docker(int8[]) RETURNS int8
AS 'MODULE_PATHNAME', 'count_with_docker'
LANGUAGE C STRICT;

CREATE FUNCTION product_share(int8[],float8[]) RETURNS float8
AS 'MODULE_PATHNAME', 'product_share'
LANGUAGE C STRICT;

CREATE FUNCTION product_with_ser(int8[],float8[]) RETURNS float8
AS 'MODULE_PATHNAME', 'product_with_ser'
LANGUAGE C STRICT;

CREATE FUNCTION product_with_docker(int8[],float8[]) RETURNS float8
AS 'MODULE_PATHNAME', 'product_with_docker'
LANGUAGE C STRICT;

CREATE FUNCTION extract_sentiment_share(int8,text) RETURNS string_array[]
AS 'MODULE_PATHNAME', 'extract_sentiment_share'
LANGUAGE C STRICT;

CREATE FUNCTION extract_sentiment_native(int8,text) RETURNS string_array[]
AS 'MODULE_PATHNAME', 'extract_sentiment_native'
LANGUAGE C STRICT;

CREATE FUNCTION extract_sentiment_with_ser(int8,text) RETURNS string_array[]
AS 'MODULE_PATHNAME', 'extract_sentiment_with_ser'
LANGUAGE C STRICT;

CREATE FUNCTION extract_sentiment_with_docker(int8,text) RETURNS string_array[]
AS 'MODULE_PATHNAME', 'extract_sentiment_with_docker'
LANGUAGE C STRICT;


CREATE FUNCTION make_pairs_native(int8[]) RETURNS pair[]
AS 'MODULE_PATHNAME', 'make_pairs_native'
LANGUAGE C STRICT;

CREATE FUNCTION make_pairs_share(int8[]) RETURNS pair[]
AS 'MODULE_PATHNAME', 'make_pairs_share'
LANGUAGE C STRICT;


CREATE FUNCTION make_pairs_with_ser(int8[]) RETURNS pair[]
AS 'MODULE_PATHNAME', 'make_pairs_with_ser'
LANGUAGE C STRICT;

CREATE FUNCTION make_pairs_with_docker(int8[]) RETURNS pair[]
AS 'MODULE_PATHNAME', 'make_pairs_with_docker'
LANGUAGE C STRICT;


CREATE FUNCTION min_agg_init_1() RETURNS numeric
AS 'MODULE_PATHNAME', 'min_agg_init_1'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_trans_1(float8, numeric) RETURNS float8
AS 'MODULE_PATHNAME', 'min_agg_trans_1'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_combine_1(float8, float8) RETURNS float8
AS 'MODULE_PATHNAME', 'min_agg_combine_1'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_final_1(float8) RETURNS numeric
AS 'MODULE_PATHNAME', 'min_agg_final_1'
LANGUAGE C STRICT;

CREATE AGGREGATE min_agg_share(numeric) (
    SFUNC = min_agg_trans_1,
    STYPE = float8,
    COMBINEFUNC = min_agg_combine_1,
    INITCOND = 1000000,
    FINALFUNC = min_agg_final_1
);


CREATE FUNCTION min_agg_init_2() RETURNS numeric
AS 'MODULE_PATHNAME', 'min_agg_init_2'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_trans_2(float8, numeric) RETURNS float8
AS 'MODULE_PATHNAME', 'min_agg_trans_2'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_combine_2(float8, float8) RETURNS float8
AS 'MODULE_PATHNAME', 'min_agg_combine_2'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_final_2(float8) RETURNS numeric
AS 'MODULE_PATHNAME', 'min_agg_final_2'
LANGUAGE C STRICT;

CREATE AGGREGATE min_agg_ser(numeric) (
    SFUNC = min_agg_trans_2,
    STYPE = float8,
    COMBINEFUNC = min_agg_combine_2,
    INITCOND = 1000000,
    FINALFUNC = min_agg_final_2
);

CREATE FUNCTION min_agg_init_3() RETURNS numeric
AS 'MODULE_PATHNAME', 'min_agg_init_3'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_trans_3(float8, numeric) RETURNS float8
AS 'MODULE_PATHNAME', 'min_agg_trans_3'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_combine_3(float8, float8) RETURNS float8
AS 'MODULE_PATHNAME', 'min_agg_combine_3'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_final_3(float8) RETURNS numeric
AS 'MODULE_PATHNAME', 'min_agg_final_3'
LANGUAGE C STRICT;

CREATE AGGREGATE min_agg_docker(numeric) (
    SFUNC = min_agg_trans_3,
    STYPE = float8,
    COMBINEFUNC = min_agg_combine_3,
    INITCOND = 1000000,
    FINALFUNC = min_agg_final_3
);


CREATE FUNCTION min_agg_init_4() RETURNS numeric
AS 'MODULE_PATHNAME', 'min_agg_init_4'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_trans_4(float8, numeric) RETURNS float8
AS 'MODULE_PATHNAME', 'min_agg_trans_4'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_combine_4(float8, float8) RETURNS float8
AS 'MODULE_PATHNAME', 'min_agg_combine_4'
LANGUAGE C STRICT;

CREATE FUNCTION min_agg_final_4(float8) RETURNS numeric
AS 'MODULE_PATHNAME', 'min_agg_final_4'
LANGUAGE C STRICT;

CREATE AGGREGATE min_agg_native(numeric) (
    SFUNC = min_agg_trans_4,
    STYPE = float8,
    COMBINEFUNC = min_agg_combine_4,
    INITCOND = 1000000,
    FINALFUNC = min_agg_final_4
);

CREATE FUNCTION product_agg_init_1() RETURNS numeric
AS 'MODULE_PATHNAME', 'product_agg_init_1'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_trans_1(float8, numeric,numeric) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_trans_1'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_combine_1(float8, float8) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_combine_1'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_final_1(float8) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_final_1'
LANGUAGE C STRICT;

CREATE AGGREGATE product_agg_share(numeric,numeric) (
    SFUNC = product_agg_trans_1,
    STYPE = float8,
    COMBINEFUNC = product_agg_combine_1,
    INITCOND = 0,
    FINALFUNC = product_agg_final_1
);


CREATE FUNCTION product_agg_init_2() RETURNS numeric
AS 'MODULE_PATHNAME', 'product_agg_init_2'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_trans_2(float8, numeric,numeric) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_trans_2'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_combine_2(float8, float8) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_combine_2'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_final_2(float8) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_final_2'
LANGUAGE C STRICT;

CREATE AGGREGATE product_agg_ser(numeric,numeric) (
    SFUNC = product_agg_trans_2,
    STYPE = float8,
    COMBINEFUNC = product_agg_combine_2,
    INITCOND = 0,
    FINALFUNC = product_agg_final_2
);


CREATE FUNCTION product_agg_init_3() RETURNS numeric
AS 'MODULE_PATHNAME', 'product_agg_init_3'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_trans_3(float8, numeric,numeric) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_trans_3'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_combine_3(float8, float8) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_combine_3'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_final_3(float8) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_final_3'
LANGUAGE C STRICT;

CREATE AGGREGATE product_agg_docker(numeric,numeric) (
    SFUNC = product_agg_trans_3,
    STYPE = float8,
    COMBINEFUNC = product_agg_combine_3,
    INITCOND = 0,
    FINALFUNC = product_agg_final_3
);



CREATE FUNCTION product_agg_init_4() RETURNS numeric
AS 'MODULE_PATHNAME', 'product_agg_init_4'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_trans_4(float8, numeric,numeric) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_trans_4'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_combine_4(float8, float8) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_combine_4'
LANGUAGE C STRICT;

CREATE FUNCTION product_agg_final_4(float8) RETURNS float8
AS 'MODULE_PATHNAME', 'product_agg_final_4'
LANGUAGE C STRICT;

CREATE AGGREGATE product_agg_native(numeric,numeric) (
    SFUNC = product_agg_trans_4,
    STYPE = float8,
    COMBINEFUNC = product_agg_combine_4,
    INITCOND = 0,
    FINALFUNC = product_agg_final_4
);