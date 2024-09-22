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


-- Find the categories with flat or declining sales for in store purchases
-- during a given year for a given store.

-- Resources

--Result  --------------------------------------------------------------------
--CREATE RESULT TABLE.
DROP TABLE IF EXISTS ${RESULT_TABLE};
CREATE TABLE ${RESULT_TABLE} (
  cat       INT,
  slope     decimal(15,7),
  intercept decimal(15,7)
);

INSERT INTO TABLE ${RESULT_TABLE}
SELECT *
FROM (
  SELECT
    cat,
    ((count(x) * SUM(xy) - SUM(x) * SUM(y)) / (count(x) * SUM(xx) - SUM(x) * SUM(x)) ) AS slope,
    (SUM(y) - ((count(x) * SUM(xy) - SUM(x) * SUM(y)) / (count(x) * SUM(xx) - SUM(x)*SUM(x)) ) * SUM(x)) / count(x) AS intercept
  FROM (
    SELECT
      i.i_category_id AS cat, -- ranges from 1 to 10
      s.ss_sold_date_sk AS x,
      SUM(s.ss_net_paid) AS y,
      s.ss_sold_date_sk * SUM(s.ss_net_paid) AS xy,
      s.ss_sold_date_sk * s.ss_sold_date_sk AS xx
    FROM store_sales s
    -- select date range
    LEFT SEMI JOIN (
      SELECT d_date_sk
      FROM date_dim d
      WHERE d.d_date >= '${q15_startDate}'
      AND   d.d_date <= '${q15_endDate}'
    ) dd ON ( s.ss_sold_date_sk=dd.d_date_sk )
    INNER JOIN item i ON s.ss_item_sk = i.i_item_sk
    WHERE i.i_category_id IS NOT NULL
    AND s.ss_store_sk = ${q15_store_sk} -- for a given store ranges from 1 to 12
    GROUP BY i.i_category_id, s.ss_sold_date_sk
  ) temp
  GROUP BY cat
) regression
WHERE slope <= 0
ORDER BY cat
-- limit not required, number of categories is known to be small and of fixed size across scalefactors
;
