--
--Copyright (C) 2019-2022 Alibaba Group Holding Limited.
--
-- drop tables
DROP TABLE IF EXISTS ${TEMP_TABLE}_model;
DROP TABLE IF EXISTS ${TEMP_TABLE}_predict;

-- run naive bayes
pai -name TextClassificationTrain
-DinputTableName=${TEMP_TABLE1}
-DinputValidTableName=${TEMP_TABLE2}
-DmodelTableName=${TEMP_TABLE}_model
-DoutputPredValidTableName=${TEMP_TABLE}_predict
-DlabelColName=pr_rating_str
-DtextColName=pr_review_content
-DappendColNames='pr_review_sk,pr_rating,pr_rating_str'
-Dmethod=MultiNomialNaiveBayes
;
