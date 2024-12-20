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

-- based on tpc-ds q21

-- For all items whose price was changed on a given date,
-- compute the percentage change in inventory between the 30-day period BEFORE
-- the price change and the 30-day period AFTER the change. Group this
-- information by warehouse.

-- Resources

--Result --------------------------------------------------------------------
--CREATE RESULT TABLE. 
DROP TABLE IF EXISTS ${RESULT_TABLE};
CREATE TABLE ${RESULT_TABLE} (
  w_warehouse_name STRING,
  i_item_id        STRING,
  inv_before       BIGINT,
  inv_after        BIGINT
);

-- the real query part
INSERT INTO TABLE ${RESULT_TABLE}
SELECT
  w_warehouse_name,
  i_item_id,
  SUM( CASE WHEN datediff(d_date, '${q22_date}') < 0
    THEN inv_quantity_on_hand
    ELSE 0 END
  ) AS inv_before,
  SUM( CASE WHEN datediff(d_date, '${q22_date}') >= 0
    THEN inv_quantity_on_hand
    ELSE 0 END
  ) AS inv_after
FROM inventory inv,
  item i,
  warehouse w,
  date_dim d
WHERE i_current_price BETWEEN ${q22_i_current_price_min} AND ${q22_i_current_price_max}
AND i_item_sk        = inv_item_sk
AND inv_warehouse_sk = w_warehouse_sk
AND inv_date_sk      = d_date_sk
AND datediff(d_date, '${q22_date}') >= -30
AND datediff(d_date, '${q22_date}') <= 30

GROUP BY w_warehouse_name, i_item_id
HAVING inv_before > 0
AND inv_after / inv_before >= 2.0 / 3.0
AND inv_after / inv_before <= 3.0 / 2.0
ORDER BY w_warehouse_name, i_item_id
LIMIT 100
;


---- cleanup ----------------
