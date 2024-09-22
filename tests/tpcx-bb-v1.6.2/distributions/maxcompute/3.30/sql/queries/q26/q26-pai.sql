--
--Copyright (C) 2019-2022 Alibaba Group Holding Limited.
--
drop table if exists ${KMEANS_CENTER};
drop table if exists ${KMEANS_IDX};

pai -name kmeans
-project algo_public
-DinputTableName=${TEMP_TABLE}
-DselectedColNames=id1,id2,id3,id4,id5,id6,id7,id8,id9,id10,id11,id12,id13,id14,id15
-DcenterCount=8
-Dloop=20
-Daccuracy=0.00001
-DdistanceType=euclidean
-DinitCenterMethod=random
-Dseed=45738
-DcenterTableName=${KMEANS_CENTER}
-DidxTableName=${KMEANS_IDX}
-DappendColNames=cid
;
