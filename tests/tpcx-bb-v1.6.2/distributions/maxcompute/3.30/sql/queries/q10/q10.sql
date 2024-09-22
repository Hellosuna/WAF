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


-- For all products, extract sentences from its product reviews that contain positive or negative sentiment
-- and display for each item the sentiment polarity of the extracted sentences (POS OR NEG)
-- and the sentence and word in sentence leading to this classification

-- Resources
DROP FUNCTION IF EXISTS ${PREFIX}_extract_sentiment;
ADD JAR ${BIG_BENCH_QUERY_RESOURCES}/opennlp-maxent-3.0.3.jar as ${JAR_ALIAS_OPENNLP_MAXENT} -f;
ADD JAR ${BIG_BENCH_QUERY_RESOURCES}/opennlp-tools-1.9.3.jar as ${JAR_ALIAS_OPENNLP_TOOLS} -f;
ADD JAR ${BIG_BENCH_QUERY_RESOURCES}/bigbenchqueriesmr.jar as ${JAR_ALIAS_UDTF} -f ;
CREATE FUNCTION ${PREFIX}_extract_sentiment AS 'io.bigdatabenchmark.v1.queries.q10.SentimentUDF' using '${JAR_ALIAS_OPENNLP_MAXENT},${JAR_ALIAS_OPENNLP_TOOLS},${JAR_ALIAS_UDTF}';

-- Query parameters

--Result  --------------------------------------------------------------------
--CREATE RESULT TABLE. 
DROP TABLE IF EXISTS ${RESULT_TABLE};
CREATE TABLE ${RESULT_TABLE} (
  item_sk         BIGINT,
  review_sentence STRING,
  sentiment       STRING,
  sentiment_word  STRING
)
;

-- the real query part
INSERT OVERWRITE TABLE ${RESULT_TABLE}
SELECT item_sk, review_sentence, sentiment, sentiment_word
FROM (--wrap in additional FROM(), because Sorting/distribute by with UDTF in select clause is not allowed
  SELECT ${PREFIX}_extract_sentiment(pr_item_sk, pr_review_content) AS (item_sk, review_sentence, sentiment, sentiment_word)
  FROM product_reviews
) extracted
ORDER BY item_sk,review_sentence,sentiment,sentiment_word
;
