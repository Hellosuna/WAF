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
-- Copyright 2015-2019 Intel Corporation.
-- This software and the related documents are Intel copyrighted materials, and your use of them 
-- is governed by the express license under which they were provided to you ("License"). Unless the 
-- License provides otherwise, you may not use, modify, copy, publish, distribute, disclose or 
-- transmit this software or the related documents without Intel's prior written permission.
-- 
-- This software and the related documents are provided as is, with no express or implied warranties, 
-- other than those that are expressly stated in the License.
-- 
--


-- Identify the stores with flat or declining sales in 3 consecutive months,
-- check if there are any negative reviews regarding these stores available online.

-- Resources
ADD JAR ${env:BIG_BENCH_QUERY_RESOURCES}/opennlp-maxent-3.0.3.jar;
ADD JAR ${env:BIG_BENCH_QUERY_RESOURCES}/opennlp-tools-1.9.3.jar;
ADD JAR ${env:BIG_BENCH_QUERY_RESOURCES}/bigbenchqueriesmr.jar;
CREATE TEMPORARY FUNCTION extract_NegSentiment AS 'io.bigdatabenchmark.v1.queries.q18.NegativeSentimentUDF';

--STEP 1: calculate and filter stores with linear regression: stores with flat or declining sales in 3 consecutive months
DROP TABLE IF EXISTS ${hiveconf:TEMP_TABLE1};
CREATE TABLE IF NOT EXISTS ${hiveconf:TEMP_TABLE1} AS
-- join with "store" table to retrieve store name
  SELECT s.s_store_sk, s.s_store_name
  FROM store s,
  (
--select ss_store_sk's with flat or declining sales in 3 consecutive months.
-- linear regression part of stores by analysing store_sales
    SELECT
      temp.ss_store_sk,
      ((count(temp.x) * SUM(temp.xy) - SUM(temp.x) * SUM(temp.y)) / (count(temp.x) * SUM(temp.xx) - SUM(temp.x) * SUM(temp.x)) ) AS slope
    FROM
    (
      SELECT
        s.ss_store_sk,
        s.ss_sold_date_sk AS x,
        SUM(s.ss_net_paid) AS y,
        s.ss_sold_date_sk * SUM(s.ss_net_paid) AS xy,
        s.ss_sold_date_sk*s.ss_sold_date_sk AS xx
      FROM store_sales s
--select date range
      LEFT SEMI JOIN (
        SELECT d_date_sk
        FROM date_dim d
        WHERE d.d_date >= '${hiveconf:q18_startDate}'
        AND   d.d_date <= '${hiveconf:q18_endDate}'
      ) dd ON ( s.ss_sold_date_sk=dd.d_date_sk )
      GROUP BY s.ss_store_sk, s.ss_sold_date_sk
    ) temp
    GROUP BY temp.ss_store_sk
  ) regression_analysis
--flat or declining sales
  WHERE slope <= 0
  AND s.s_store_sk = regression_analysis.ss_store_sk
;

-- STEP 2: reviews regarding these stores
DROP TABLE IF EXISTS ${hiveconf:TEMP_TABLE2};
CREATE TABLE IF NOT EXISTS ${hiveconf:TEMP_TABLE2} AS
SELECT
--this could be any string we want to identify the store. but as store_sk is just a number and store_name is ambiguous we choose the concatenation of both
  CONCAT(s_store_sk,"_", s_store_name ) AS store_ID,
  pr_review_date,
  pr_review_content
-- THIS IS A CROSS JOIN! but fortunately the "stores_with_regression" table is very small
FROM product_reviews pr, ${hiveconf:TEMP_TABLE1} stores_with_regression
 --find store name in reviews
WHERE locate(lower(stores_with_regression.s_store_name), lower(pr.pr_review_content), 1) >= 1
;


--Result  --------------------------------------------------------------------
--keep result human readable
set hive.exec.compress.output=false;
set hive.exec.compress.output;

-- This query requires parallel orderby for fast and deterministic global ordering of final result
set hive.optimize.sampling.orderby=${hiveconf:bigbench.hive.optimize.sampling.orderby};
set hive.optimize.sampling.orderby.number=${hiveconf:bigbench.hive.optimize.sampling.orderby.number};
set hive.optimize.sampling.orderby.percent=${hiveconf:bigbench.hive.optimize.sampling.orderby.percent};
--debug print
set hive.optimize.sampling.orderby;
set hive.optimize.sampling.orderby.number;
set hive.optimize.sampling.orderby.percent;

--Prepare result storage
DROP TABLE IF EXISTS ${hiveconf:RESULT_TABLE};
CREATE TABLE ${hiveconf:RESULT_TABLE} (
  s_name         STRING,
  r_date         STRING,
  r_sentence     STRING,
  sentiment      STRING,
  sentiment_word STRING
)
ROW FORMAT DELIMITED FIELDS TERMINATED BY ',' LINES TERMINATED BY '\n'
STORED AS ${env:BIG_BENCH_hive_default_fileformat_result_table} LOCATION '${hiveconf:RESULT_DIR}';

-- the real query - filter
INSERT INTO TABLE ${hiveconf:RESULT_TABLE}
SELECT s_name, r_date, r_sentence, sentiment, sentiment_word
-- wrap in additional FROM(), because Sorting/distribute by with UDTF in select clause is not allowed
FROM (
--negative sentiment analysis of found reviews for stores with declining sales
  SELECT extract_NegSentiment(store_ID, pr_review_date, pr_review_content) AS ( s_name, r_date, r_sentence, sentiment, sentiment_word )
  FROM ${hiveconf:TEMP_TABLE2}
) extracted
ORDER BY s_name, r_date, r_sentence, sentiment,sentiment_word
--CLUSTER BY instead of ORDER BY does not work to achieve global ordering. e.g. 2 reducers: first reducer will write keys 0,2,4,6.. into file 000000_0 and reducer 2 will write keys 1,3,5,7,.. into file 000000_1.concatenating these files does not produces a deterministic result if number of reducer changes.
--Solution: parallel "order by" as non parallel version only uses a single reducer and we cant use "limit
;


-- Cleanup ----------------------------------------
DROP TABLE IF EXISTS ${hiveconf:TEMP_TABLE1};
DROP TABLE IF EXISTS ${hiveconf:TEMP_TABLE2};
