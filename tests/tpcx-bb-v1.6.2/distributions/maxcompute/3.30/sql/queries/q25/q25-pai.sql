--
--Copyright (C) 2019-2022 Alibaba Group Holding Limited.
--
drop table if exists ${KMEANS_CENTER};
drop table if exists ${KMEANS_IDX};

pai -name kmeans
-project algo_public
-DinputTableName=${TEMP_RESULT_TABLE}
-DselectedColNames=recency,frequency,totalspend
-DcenterCount=8
-Dloop=20
-Daccuracy=0.00001
-DdistanceType=euclidean
-DinitCenterMethod=random
-DcenterTableName=${KMEANS_CENTER}
-DidxTableName=${KMEANS_IDX}
-Dseed=45738
-DappendColNames=cid
;
