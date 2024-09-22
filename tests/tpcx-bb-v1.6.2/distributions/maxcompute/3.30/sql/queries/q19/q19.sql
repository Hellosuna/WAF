--
-- Copyright (C) 2019 Transaction Processing Performance Council (TPC) and/or its contributors.
-- This file is part of a software package distributed by the TPC
-- The contents of this file have been developed by the TPC, and/or have been licensed to the TPC under one or more contributor
-- license agreements.
-- This file is subject to the terms and conditions outlined in the End-User
-- License Agreement (EULA) which can be found in this distribution (EULA.txt) and is available at the following URL:
-- http://www.tpc.org/TPC_Documents_Current_Versions/txt/EULA.txt
-- Unless required by applicable law or agreed to in writing, this software is distributed on an "AS IS" BASIS, WITHOUT
-- WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied, and the user bears the entire risk as to quality
-- and performance as well as the entire cost of service or repair in case of defect. See the EULA for more details.
-- 
--


--
--Copyright (C) 2019-2022 Alibaba Group Holding Limited.
--


-- Retrieve the items with the highest number of returns where the number
-- of returns was approximately equivalent across all store and web channels
-- (within a tolerance of +/- 10%), within the week ending given dates. Analyse
-- the online reviews for these items to see if there are any major negative reviews.

-- Resources
DROP FUNCTION IF EXISTS ${PREFIX}_extract_sentiment;
ADD JAR ${BIG_BENCH_QUERY_RESOURCES}/opennlp-maxent-3.0.3.jar as ${JAR_ALIAS_OPENNLP_MAXENT} -f ;
ADD JAR ${BIG_BENCH_QUERY_RESOURCES}/opennlp-tools-1.9.3.jar as ${JAR_ALIAS_OPENNLP_TOOLS} -f;
ADD JAR ${BIG_BENCH_QUERY_RESOURCES}/bigbenchqueriesmr.jar as ${JAR_ALIAS_UDTF} -f;
CREATE FUNCTION ${PREFIX}_extract_sentiment AS 'io.bigdatabenchmark.v1.queries.q10.SentimentUDF' using '${JAR_ALIAS_OPENNLP_MAXENT},${JAR_ALIAS_OPENNLP_TOOLS},${JAR_ALIAS_UDTF}';

--Result  returned items with negative sentiment --------------------------------------------

--CREATE RESULT TABLE. 
DROP TABLE IF EXISTS ${RESULT_TABLE};
CREATE TABLE ${RESULT_TABLE} (
  item_sk         BIGINT,
  review_sentence STRING,
  sentiment       STRING,
  sentiment_word  STRING
)
;

---- the real query --------------
INSERT OVERWRITE TABLE ${RESULT_TABLE}
SELECT *
FROM
( --wrap in additional FROM(), because Sorting/distribute by with UDTF in select clause is not allowed
  SELECT ${PREFIX}_extract_sentiment(pr.pr_item_sk, pr.pr_review_content) AS
  (
    item_sk,
    review_sentence,
    sentiment,
    sentiment_word
  )
  FROM product_reviews pr,
  (
    --store returns in week ending given date
    SELECT sr_item_sk, SUM(sr_return_quantity) sr_item_qty
    FROM store_returns sr,
    (
      -- within the week ending a given date
      SELECT d1.d_date_sk
      FROM date_dim d1, date_dim d2
      WHERE d1.d_week_seq = d2.d_week_seq
      AND d2.d_date IN ( ${q19_storeReturns_date_IN} )
    ) sr_dateFilter
    WHERE sr.sr_returned_date_sk = d_date_sk
    GROUP BY sr_item_sk --across all store and web channels
    HAVING sr_item_qty > 0
  ) fsr,
  (
    --web returns in week ending given date
    SELECT wr_item_sk, SUM(wr_return_quantity) wr_item_qty
    FROM web_returns wr,
    (
      -- within the week ending a given date
      SELECT d1.d_date_sk
      FROM date_dim d1, date_dim d2
      WHERE d1.d_week_seq = d2.d_week_seq
      AND d2.d_date IN ( ${q19_webReturns_date_IN} )
    ) wr_dateFilter
    WHERE wr.wr_returned_date_sk = d_date_sk
    GROUP BY wr_item_sk  --across all store and web channels
    HAVING wr_item_qty > 0
  ) fwr
  WHERE fsr.sr_item_sk = fwr.wr_item_sk
  AND pr.pr_item_sk = fsr.sr_item_sk --extract product_reviews for found items
  -- equivalent across all store and web channels (within a tolerance of +/- 10%)
  AND abs( (sr_item_qty-wr_item_qty)/ ((sr_item_qty+wr_item_qty)/2)) <= 0.1
)extractedSentiments
WHERE sentiment= 'NEG' --if there are any major negative reviews.
--item_sk is skewed, but we need to sort by it. Technically we just expect a deterministic global sorting and not clustering by item_sk...so we could distribute by pr_review_sk
ORDER BY item_sk,review_sentence,sentiment,sentiment_word
;
