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


-- Find all customers who viewed items of a given category on the web
-- in a given month and year that was followed by an in-store purchase of an item from the same category in the three
-- consecutive months.

-- Resources
--Result  --------------------------------------------------------------------

DROP TABLE IF EXISTS ${RESULT_TABLE};
CREATE TABLE ${RESULT_TABLE} (
  u_id BIGINT
)
;

EXPLAIN
SELECT DISTINCT wcs_user_sk -- Find all customers
-- TODO check if 37134 is first day of the month
FROM
( -- web_clicks viewed items in date range with items from specified categories
  SELECT
    wcs_user_sk,
    wcs_click_date_sk
  FROM web_clickstreams, item
  WHERE wcs_click_date_sk BETWEEN 37134 AND (37134 + 30) -- in a given month and year
  AND i_category IN (${q12_i_category_IN}) -- filter given category
  AND wcs_item_sk = i_item_sk
  AND wcs_user_sk IS NOT NULL
  AND wcs_sales_sk IS NULL --only views, not purchases
) webInRange,
(  -- store sales in date range with items from specified categories
  SELECT
    ss_customer_sk,
    ss_sold_date_sk
  FROM store_sales, item
  WHERE ss_sold_date_sk BETWEEN 37134 AND (37134 + 90) -- in the three consecutive months.
  AND i_category IN (${q12_i_category_IN}) -- filter given category 
  AND ss_item_sk = i_item_sk
  AND ss_customer_sk IS NOT NULL
) storeInRange
-- join web and store
WHERE wcs_user_sk = ss_customer_sk
AND wcs_click_date_sk < ss_sold_date_sk -- buy AFTER viewed on website
ORDER BY wcs_user_sk
;
