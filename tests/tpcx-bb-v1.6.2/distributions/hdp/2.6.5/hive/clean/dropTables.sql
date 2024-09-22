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


set customerTableName=customer;
set customerAddressTableName=customer_address;
set customerDemographicsTableName=customer_demographics;
set dateTableName=date_dim;
set householdDemographicsTableName=household_demographics;
set incomeTableName=income_band;
set itemTableName=item;
set promotionTableName=promotion;
set reasonTableName=reason;
set shipModeTableName=ship_mode;
set storeTableName=store;
set timeTableName=time_dim;
set warehouseTableName=warehouse;
set webSiteTableName=web_site;
set webPageTableName=web_page;
set inventoryTableName=inventory;
set storeSalesTableName=store_sales;
set storeReturnsTableName=store_returns;
set webSalesTableName=web_sales;
set webReturnsTableName=web_returns;
set marketPricesTableName=item_marketprices;
set clickstreamsTableName=web_clickstreams;
set reviewsTableName=product_reviews;

DROP TABLE IF EXISTS ${hiveconf:customerTableName};
DROP TABLE IF EXISTS ${hiveconf:customerAddressTableName};
DROP TABLE IF EXISTS ${hiveconf:customerDemographicsTableName};
DROP TABLE IF EXISTS ${hiveconf:dateTableName};
DROP TABLE IF EXISTS ${hiveconf:householdDemographicsTableName};
DROP TABLE IF EXISTS ${hiveconf:incomeTableName};
DROP TABLE IF EXISTS ${hiveconf:itemTableName};
DROP TABLE IF EXISTS ${hiveconf:promotionTableName};
DROP TABLE IF EXISTS ${hiveconf:reasonTableName};
DROP TABLE IF EXISTS ${hiveconf:shipModeTableName};
DROP TABLE IF EXISTS ${hiveconf:storeTableName};
DROP TABLE IF EXISTS ${hiveconf:timeTableName};
DROP TABLE IF EXISTS ${hiveconf:warehouseTableName};
DROP TABLE IF EXISTS ${hiveconf:webSiteTableName};
DROP TABLE IF EXISTS ${hiveconf:webPageTableName};
DROP TABLE IF EXISTS ${hiveconf:inventoryTableName};
DROP TABLE IF EXISTS ${hiveconf:storeSalesTableName};
DROP TABLE IF EXISTS ${hiveconf:storeReturnsTableName};
DROP TABLE IF EXISTS ${hiveconf:webSalesTableName};
DROP TABLE IF EXISTS ${hiveconf:webReturnsTableName};
DROP TABLE IF EXISTS ${hiveconf:marketPricesTableName};
DROP TABLE IF EXISTS ${hiveconf:clickstreamsTableName};
DROP TABLE IF EXISTS ${hiveconf:reviewsTableName};
