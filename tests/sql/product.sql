/*q19*/
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
	product_agg_docker(l_extendedprice,l_discount) as revenue
from
	lineitem, 
	part
where
	(
		p_partkey = l_partkey
		and p_brand = ''Brand#12''
		and p_container in (''SM CASE'', ''SM BOX'', ''SM PACK'', ''SM PKG'')
		and l_quantity >= 1 and l_quantity <= 1 + 10
		and p_size between 1 and 5
		and l_shipmode in (''AIR'', ''AIR REG'')
		and l_shipinstruct = ''DELIVER IN PERSON''
	)
	or
	(
		p_partkey = l_partkey
		and p_brand = ''Brand#23''
		and p_container in (''MED BAG'', ''MED BOX'', ''MED PKG'', ''MED PACK'')
		and l_quantity >= 10 and l_quantity <= 10 + 10
		and p_size between 1 and 10
		and l_shipmode in (''AIR'', ''AIR REG'')
		and l_shipinstruct = ''DELIVER IN PERSON''
	)
	or
	(
		p_partkey = l_partkey
		and p_brand = ''Brand#34''
		and p_container in (''LG CASE'', ''LG BOX'', ''LG PACK'', ''LG PKG'')
		and l_quantity >= 20 and l_quantity <= 20 + 10
		and p_size between 1 and 15
		and l_shipmode in (''AIR'', ''AIR REG'')
		and l_shipinstruct = ''DELIVER IN PERSON''
	)';

    -- 记录结束时间
    SELECT clock_timestamp() INTO end_time;

    -- 计算时间差
    execution_time := end_time - start_time;
    
    execution_time_seconds := EXTRACT(EPOCH FROM execution_time);
    
    -- 输出执行时间
    RAISE NOTICE 'Execution time: % seconds', execution_time_seconds;
END $$;


/*约250行*/
/*约50行*/
/*约550行*/