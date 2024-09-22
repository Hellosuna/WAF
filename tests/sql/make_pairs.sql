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
    SELECT (item_sk).first, (item_sk).second,COUNT(*) AS cnt from (
    SELECT unnest(make_pairs_with_ser(itemArray)) AS item_sk
        FROM (
          SELECT array_agg(ss_item_sk ORDER BY ss_item_sk) AS itemArray
          FROM store_sales s
          JOIN item i ON s.ss_item_sk = i.i_item_sk
          WHERE i.i_category_id IN (1, 2 ,3)
          AND s.ss_store_sk IN (10, 20, 33, 40, 50)
          GROUP BY ss_ticket_number
        ) AS soldItemsPerTicket
    )soldTogetherPairs
    GROUP BY (item_sk).first, (item_sk).second
    having count(*) > 50
    ORDER BY cnt DESC,(item_sk).first, (item_sk).second
    LIMIT 100';
    -- 记录结束时间
    SELECT clock_timestamp() INTO end_time;

    -- 计算时间差
    execution_time := end_time - start_time;
    
    execution_time_seconds := EXTRACT(EPOCH FROM execution_time);
    
    -- 输出执行时间
    RAISE NOTICE 'Execution time: % seconds', execution_time_seconds;
END $$;
/*约3000行*/
/*约40000行*/
/*约290000行*/
