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
    select
	c_name,
	c_custkey,
	o_orderkey,
	o_orderdate,
	o_totalprice,
	sum_native(array_agg(cast(l_quantity as bigint)))
from
	customer,
	orders,
	lineitem
where
	o_orderkey in (
		select
			l_orderkey
		from
			lineitem
		group by
			l_orderkey having
				sum_native(array_agg(cast(l_quantity as bigint))) > 300
	)
	and c_custkey = o_custkey
	and o_orderkey = l_orderkey
group by
	c_name,
	c_custkey,
	o_orderkey,
	o_orderdate,
	o_totalprice
order by
	o_totalprice desc,
	o_orderdate';

    -- 记录结束时间
    SELECT clock_timestamp() INTO end_time;

    -- 计算时间差
    execution_time := end_time - start_time;
    
    execution_time_seconds := EXTRACT(EPOCH FROM execution_time);
    
    -- 输出执行时间
    RAISE NOTICE 'Execution time: % seconds', execution_time_seconds;
END $$;
/*约700000行*/
/*约7000000行*/
/*约3500000行*/