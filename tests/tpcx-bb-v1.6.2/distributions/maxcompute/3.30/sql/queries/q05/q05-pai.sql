--
--Copyright (C) 2019-2022 Alibaba Group Holding Limited.
--
drop offlinemodel if exists ${LR_MODEL};
drop table if exists ${TEMP_TABLE1};
drop table if exists ${TEMP_TABLE2};
drop table if exists ${TEMP_TABLE3};
drop table if exists ${LR_PREDICTION};
drop table if exists ${LR_METRIC};

pai -name split
    -project algo_public
    -DinputTableName=${TEMP_TABLE}
    -Doutput1TableName=${TEMP_TABLE1}
    -Doutput2TableName=${TEMP_TABLE2}
    -DrandomSeed=5
    -Dfraction=0.9;

pai -name logisticregression_binary
-project algo_public
-DmodelName=${LR_MODEL}
-DregularizedLevel=0.0
-DmaxIter=20
-DregularizedType=l2
-Depsilon=1e-5
-DlabelColName=clicks_in_category
-DfeatureColNames=college_education,male,clicks_in_1,clicks_in_2,clicks_in_3,clicks_in_4,clicks_in_5,clicks_in_6,clicks_in_7
-DgoodValue=1
-DinputTableName=${TEMP_TABLE1}
-DverboseTableName=${TEMP_TABLE3}
;

pai -name prediction
    -DmodelName=${LR_MODEL}
    -DinputTableName=${TEMP_TABLE2}
    -DoutputTableName=${LR_PREDICTION}
    -DappendColNames=clicks_in_category
;

pai -name=MultiClassEvaluation -project=algo_public
    -DoutputTableName=${LR_METRIC}
    -DinputTableName=${LR_PREDICTION}
    -DlabelColName=clicks_in_category
    -DpredictionColName=prediction_result
    -DpredictionDetailColName=prediction_detail
;
drop offlinemodel if exists ${LR_MODEL};
