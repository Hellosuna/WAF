set hive.groupby.aggregateratio=0.5;

set inceptor.udafdistinct.groupby.conversion=true;

set inceptor.mapjoin.filter=true;
set ngmr.stargate.filterjoin.pushdown.filter.enabled=true;
set ngmr.stargate.filterjoin.infilter.size=100000;
set ngmr.stargate.filterjoin.minmaxfilter.max.num=2;

set ngmr.windrunner.enabled=true;
set ngmr.windrunner.nonquery.enabled=true;
set hive.conf.validation=false;
set inceptor.conf.validation=false;
set ngmr.windrunner.session.orc=true;
set orc.columnreader=true;
