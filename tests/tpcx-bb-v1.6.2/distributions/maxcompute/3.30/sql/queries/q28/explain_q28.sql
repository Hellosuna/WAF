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


-- TASK
-- Build text classifier for online review sentiment classification (Positive,
-- Negative, Neutral), using 90% of available reviews for training and the remaining
-- 40% for testing. Display classifier accuracy on testing data 
-- and classification result for the 10% testing data: <reviewSK>,<originalRating>,<classificationResult>





--Result 1 Training table for classification
DROP TABLE IF EXISTS ${TEMP_TABLE1};
CREATE TABLE ${TEMP_TABLE1} (
  pr_review_sk      BIGINT,
  pr_rating         BIGINT,
  pr_rating_str     STRING,
  pr_review_content STRING
)
;

--Result 2 Testing table for classification
DROP TABLE IF EXISTS ${TEMP_TABLE2};
CREATE TABLE ${TEMP_TABLE2} (
  pr_review_sk      BIGINT,
  pr_rating         BIGINT,
  pr_rating_str	    STRING,
  pr_review_content STRING
)
;

--Split reviews table into training and testing
EXPLAIN
FROM (
  SELECT
    pr_review_sk,
    pr_review_rating,
    case when pr_review_rating in (1, 2) then "NEG"
    when pr_review_rating in (4,5) then "POS"
    else 'NEUT' end,
    pr_review_content
  FROM product_reviews
  where pr_review_content is not null
  ORDER BY pr_review_sk
)p
INSERT OVERWRITE TABLE ${TEMP_TABLE1}
  SELECT *
  WHERE pmod(pr_review_sk, 10) IN (1,2,3,4,5,6,7,8,9) -- 90% are training
INSERT OVERWRITE TABLE ${TEMP_TABLE2}
  SELECT *
  WHERE pmod(pr_review_sk, 10) IN (0) -- 10% are testing
;
