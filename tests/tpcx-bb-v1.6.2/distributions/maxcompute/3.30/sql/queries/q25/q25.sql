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

-- TASK:
-- Customer segmentation analysis: Customers are separated along the
-- following key shopping dimensions: recency of last visit, frequency of visits and
-- monetary amount. Use the store and online purchase data during a given year
-- to compute. After model of separation is build, 
-- report for the analysed customers to which "group" they where assigned

-- IMPLEMENTATION NOTICE:
-- hive provides the input for the clustering program
-- The input format for the clustering is:
--   customer ID, 
--   flag if customer bought something within the last 60 days (integer 0 or 1), 
--   number of orders, 
--   total amount spent



-- ss_sold_date_sk > 2002-01-02
DROP TABLE IF EXISTS ${TEMP_TABLE};
CREATE TABLE ${TEMP_TABLE} (
  cid               BIGINT,
  frequency         BIGINT,
  most_recent_date  BIGINT,
  amount            decimal(15,2)
);

-- Add store sales data
INSERT INTO TABLE ${TEMP_TABLE}
SELECT
  ss_customer_sk                     AS cid,
  count(distinct ss_ticket_number)   AS frequency,
  max(ss_sold_date_sk)               AS most_recent_date,
  SUM(ss_net_paid)                   AS amount
FROM store_sales ss
JOIN date_dim d ON ss.ss_sold_date_sk = d.d_date_sk
WHERE d.d_date > '${q25_date}'
AND ss_customer_sk IS NOT NULL
GROUP BY ss_customer_sk
;

-- Add web sales data
INSERT INTO TABLE ${TEMP_TABLE}
SELECT
  ws_bill_customer_sk             AS cid,
  count(distinct ws_order_number) AS frequency,
  max(ws_sold_date_sk)            AS most_recent_date,
  SUM(ws_net_paid)                AS amount
FROM web_sales ws
JOIN date_dim d ON ws.ws_sold_date_sk = d.d_date_sk
WHERE d.d_date > '${q25_date}'
AND ws_bill_customer_sk IS NOT NULL
GROUP BY ws_bill_customer_sk
;

-- This query requires parallel order by for fast and deterministic global ordering of final result


--ML-algorithms expect double values as input for their Vectors. 
DROP TABLE IF EXISTS ${TEMP_RESULT_TABLE};
CREATE TABLE ${TEMP_RESULT_TABLE} (
  cid        BIGINT,--used as "label", all following values are used as Vector for ML-algorithm
  recency    double,
  frequency  double,
  totalspend double
)
;


INSERT OVERWRITE TABLE ${TEMP_RESULT_TABLE}
SELECT
  -- rounding of values not necessary
  cid            AS cid,
  CASE WHEN 37621 - max(most_recent_date) < 60 THEN 1.0 ELSE 0.0 END 
                 AS recency, -- 37621 == 2003-01-02
  SUM(frequency) AS frequency, --total frequency
  SUM(amount)    AS totalspend --total amount
FROM ${TEMP_TABLE}
GROUP BY cid 
ORDER BY cid
;


--- CLEANUP--------------------------------------------
DROP TABLE ${TEMP_TABLE};
