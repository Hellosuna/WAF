

--\timing
\o /dev/null
/*SELECT item_sk, cnt
FROM (
SELECT item_sk,COUNT(*) AS cnt
FROM
(
  SELECT make_pairs(itemArray) AS item_sk
  FROM
  (
    SELECT array_agg(ss_item_sk order by ss_item_sk) AS itemArray --(_list = with duplicates, _set = distinct)
    FROM store_sales s, item i
    WHERE s.ss_item_sk = i.i_item_sk
    AND i.i_category_id IN (1, 2 ,3)
    AND s.ss_store_sk IN (10, 20, 33, 40, 50)
    GROUP BY ss_ticket_number
  ) soldItemsPerTicket
) soldTogetherPairs
)groupedItems
GROUP BY item_sk
HAVING cnt > 50
ORDER BY cnt DESC, item_sk
LIMIT 100;*/
/*SELECT make_pairs(itemArray) AS item_sk
FROM
(
    SELECT array_agg(ss_item_sk ORDER BY ss_item_sk) AS itemArray
    FROM store_sales s, item i
    WHERE s.ss_item_sk = i.i_item_sk
    AND i.i_category_id IN (1, 2 ,3)
    AND s.ss_store_sk IN (10, 20, 33, 40, 50)
    GROUP BY ss_ticket_number  
)AS soldItemsPerTicket ;*/
/*SELECT make_pairs(itemArray) AS item_sk
FROM
(
    SELECT array_agg(ss_item_sk ORDER BY ss_item_sk) AS itemArray
    FROM store_sales s
    JOIN item i ON s.ss_item_sk = i.i_item_sk
    WHERE i.i_category_id IN (1, 2 ,3)
    AND s.ss_store_sk IN (10, 20, 33, 40, 50)
    GROUP BY ss_ticket_number
) AS soldItemsPerTicket;*/
/*SELECT item_sk, cnt
FROM (
  SELECT item_sk_1, item_sk_2,COUNT(*) AS cnt
  FROM (
    SELECT unnest(make_pairs(itemArray)) AS (item_sk_1, item_sk_2)
    FROM (
      SELECT array_agg(ss_item_sk ORDER BY ss_item_sk) AS itemArray
      FROM store_sales s
      JOIN item i ON s.ss_item_sk = i.i_item_sk
      WHERE i.i_category_id IN (1, 2 ,3)
      AND s.ss_store_sk IN (10, 20, 33, 40, 50)
      GROUP BY ss_ticket_number
    ) AS soldItemsPerTicket
  ) AS soldTogetherPairs
  GROUP BY item_sk_1, item_sk_2
) AS groupedItems
WHERE cnt > 50
ORDER BY cnt DESC, item_sk_1, item_sk_2
LIMIT 100;

DO $$ 
DECLARE
    start_time TIMESTAMP;
    end_time TIMESTAMP;
    execution_time INTERVAL;
    execution_time_seconds DOUBLE PRECISION;
BEGIN
    -- 记录开始时间
    SELECT clock_timestamp() INTO start_time;

    EXECUTE 'SELECT (item_sk).first, (item_sk).second,COUNT(*) AS cnt from (
    SELECT unnest(make_pairs_all(itemArray)) AS item_sk
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
*/
/*
SELECT 
    C.C_CUSTKEY, 
    (
        SELECT dt4.retval 
        FROM (
            SELECT O_ORDERKEY 
            FROM orders 
            WHERE O_CUSTKEY = C.C_CUSTKEY
        ) dt2
        CROSS JOIN LATERAL (
            SELECT count_share(array_agg(dt2.O_ORDERKEY)) as udtVal
        ) dt3
        CROSS JOIN LATERAL (
            SELECT dt3.udtVal.val as retval
        ) dt4
    ) as retval
FROM customer C;*/
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
      c_count,
      count(*) as custdist
    from
      (
        select
          c_custkey,
          count_native(array_agg(o_orderkey))
        from
          customer left outer join orders on
            c_custkey = o_custkey
            and o_comment not like ''%special%requests%''
        group by
          c_custkey
      ) as c_orders (c_custkey, c_count)
    group by
      c_count
    order by
      custdist desc,
      c_count desc';
-- 记录结束时间
    SELECT clock_timestamp() INTO end_time;

    -- 计算时间差
    execution_time := end_time - start_time;
    
    execution_time_seconds := EXTRACT(EPOCH FROM execution_time);
    
    -- 输出执行时间
    RAISE NOTICE 'Execution time: % seconds', execution_time_seconds;
END $$;

/*约70000行*/
/*约700000行*/
/*约350000行*/