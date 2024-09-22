--\timing
\o /dev/null
DO $$ 
DECLARE
    start_time TIMESTAMP;
    end_time TIMESTAMP;
    execution_time INTERVAL;
    execution_time_seconds DOUBLE PRECISION;
BEGIN
    -- 记录开始时间
    SELECT clock_timestamp() INTO start_time;

    EXECUTE '
    SELECT
        s_acctbal,
        s_name,
        n_name,
        p_partkey,
        p_mfgr,
        s_address,
        s_phone,
        s_comment
    FROM
        part,
        supplier,
        partsupp,
        nation,
        region
    WHERE
        p_partkey = ps_partkey
        AND s_suppkey = ps_suppkey
        AND p_size = 15
        AND p_type LIKE ''%BRASS''
        AND s_nationkey = n_nationkey
        AND n_regionkey = r_regionkey
        AND r_name = ''EUROPE''
        AND ps_supplycost = (
            SELECT
                min_agg_ser(ps_supplycost)
            FROM
                partsupp,
                supplier,
                nation,
                region
            WHERE
                p_partkey = ps_partkey
                AND s_suppkey = ps_suppkey
                AND s_nationkey = n_nationkey
                AND n_regionkey = r_regionkey
                AND r_name = ''EUROPE''
        )
    ORDER BY
        s_acctbal DESC,
        n_name,
        s_name,
        p_partkey';

    -- 记录结束时间
    SELECT clock_timestamp() INTO end_time;

    -- 计算时间差
    execution_time := end_time - start_time;
    
    execution_time_seconds := EXTRACT(EPOCH FROM execution_time);
    
    -- 输出执行时间
    RAISE NOTICE 'Execution time: % seconds', execution_time_seconds;
END $$;
/*约398000行*/
/*约79000行*/
/*约801000行*/
/*
SELECT
        s_acctbal,
        s_name,
        n_name,
        p_partkey,
        p_mfgr,
        s_address,
        s_phone,
        s_comment
    FROM
        part,
        supplier,
        partsupp,
        nation,
        region
    WHERE
        p_partkey = ps_partkey
        AND s_suppkey = ps_suppkey
        AND p_size = 15
        AND p_type LIKE '%BRASS'
        AND s_nationkey = n_nationkey
        AND n_regionkey = r_regionkey
        AND r_name = 'EUROPE'
        AND CAST(ps_supplycost AS bigint) = (
            SELECT
                min_with_ser(array_agg(CAST(ps_supplycost AS bigint)))
            FROM
                partsupp,
                supplier,
                nation,
                region
            WHERE
                p_partkey = ps_partkey
                AND s_suppkey = ps_suppkey
                AND s_nationkey = n_nationkey
                AND n_regionkey = r_regionkey
                AND r_name = 'EUROPE'
        )
    ORDER BY
        s_acctbal DESC,
        n_name,
        s_name,
        p_partkey;
*/