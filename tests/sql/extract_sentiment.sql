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

    EXECUTE 
    'SELECT id, sentence, sentiment, word
    FROM (
    SELECT (unnest_result).id, 
           (unnest_result).sentence, 
           (unnest_result).sentiment, 
           (unnest_result).word
    FROM (
        SELECT unnest(extract_sentiment_share(pr_item_sk, pr_review_content)) AS unnest_result
        FROM product_reviews
    ) extracted
) final_result
ORDER BY id, sentence, sentiment, word';
    -- 记录结束时间
    SELECT clock_timestamp() INTO end_time;

    -- 计算时间差
    execution_time := end_time - start_time;
    
    execution_time_seconds := EXTRACT(EPOCH FROM execution_time);
    
    -- 输出执行时间
    RAISE NOTICE 'Execution time: % seconds', execution_time_seconds;
END $$;
/*约40000行*/
/*约140000行*/
/*约500000行*/