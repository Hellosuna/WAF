--
--Copyright (C) 2019-2022 Alibaba Group Holding Limited.
--
drop table if exists ${KMEANS_CENTER};
drop table if exists ${KMEANS_IDX};

pai -name kmeans
-project algo_public
-DinputTableName=${TEMP_TABLE}
-DselectedColNames=orderratio,itemsratio,monetaryratio,frequency
-DcenterCount=8
-Dloop=20
-Daccuracy=0.00001
-DdistanceType=euclidean
-DinitCenterMethod=random
-Dseed=45738
-DcenterTableName=${KMEANS_CENTER}
-DidxTableName=${KMEANS_IDX}
-DappendColNames=user_sk
;
