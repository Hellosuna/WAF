#
#Copyright (C) 2019-2022 Alibaba Group Holding Limited.
#
import json
import sys
try:
	json_result = open(sys.argv[1]).read().replace("-nan", "\"-NAN\"").replace("nan", "\"NAN\"")
	result = json.loads(json_result)
	print "Precision: %f" %(round(float(result["OverallMeasures"]["Accuracy"]), 9))
	print "AUC: %f" %(round(float(result["LabelMeasureList"][0]["Auc"]), 9))
	print "Confusion Matrix:\n%s\t%s\n%s\t%s" %(result["ConfusionMatrix"][0][0], result["ConfusionMatrix"][0][1], result["ConfusionMatrix"][1][0], result["ConfusionMatrix"][1][1])
except BaseException, e:
	print str(e)
finally:
	pass
