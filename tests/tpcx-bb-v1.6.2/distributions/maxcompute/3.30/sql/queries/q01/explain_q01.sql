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


--TASK
--Find top 100 products that are sold together frequently in given
--stores. Only products in certain categories sold in specific stores are considered,
--and "sold together frequently" means at least 50 customers bought these products
--together in a transaction.

--IMPLEMENTATION NOTICE:
-- "Market basket analysis"
-- create pairs of "viewed together" items within one sale (one sale == one ss_sales_sk)
-- There are are several ways to to "basketing". Implemented is way A)
-- A) collect distinct viewed items per session (same sales_sk) in list and employ a UDTF to produce pairwise combinations of all list elements
-- B) distribute by sales_sk end employ reducer streaming script to aggregate all items per session and produce the pairs
-- C) pure SQL: produce pairings by self joining on sales_sk and filtering out left.item_sk < right.item_sk (elimiates dupplicates and switched posistions)


-- Resources
DROP FUNCTION IF EXISTS ${PREFIX}_makePairs;
ADD JAR ${BIG_BENCH_QUERY_RESOURCES}/bigbenchqueriesmr.jar as ${JAR_ALIAS_UDTF} -f;
CREATE FUNCTION ${PREFIX}_makePairs AS 'io.bigdatabenchmark.v1.queries.udf.PairwiseUDTF' using ${JAR_ALIAS_UDTF};

--Result -------------------------------------------------------------------------
--keep result human readable
--CREATE RESULT TABLE. 
DROP TABLE IF EXISTS ${RESULT_TABLE};
CREATE TABLE ${RESULT_TABLE} (
  pid1 BIGINT,
  pid2 BIGINT,
  cnt  BIGINT
);

-- the real query part
-- Find the most frequent ones

EXPLAIN INSERT INTO TABLE ${RESULT_TABLE}
SELECT item_sk_1, item_sk_2, COUNT(*) AS cnt
FROM
(
  -- Make item "sold together" pairs
  -- combining collect_set + sorting + makePairs(array, selfParing=false)
  -- ensures we get no pairs with swapped places like: (12,24),(24,12).
  -- We only produce tuples like: (12,24) ensuring that the smaller number is always on the left side
  SELECT ${PREFIX}_makePairs(sort_array(itemArray), false) AS (item_sk_1, item_sk_2)
  FROM
  (
    SELECT collect_set(ss_item_sk) AS itemArray --(_list = with duplicates, _set = distinct)
    FROM store_sales s, item i
    -- Only products in certain categories sold in specific stores are considered,
    WHERE s.ss_item_sk = i.i_item_sk
    AND i.i_category_id IN (${q01_i_category_id_IN})
    AND s.ss_store_sk IN (${q01_ss_store_sk_IN})
    GROUP BY ss_ticket_number
  ) soldItemsPerTicket
) soldTogetherPairs
GROUP BY item_sk_1, item_sk_2
-- 'frequently'
HAVING cnt > ${q01_viewed_together_count}
ORDER BY cnt DESC, item_sk_1, item_sk_2
LIMIT ${q01_limit};
;
