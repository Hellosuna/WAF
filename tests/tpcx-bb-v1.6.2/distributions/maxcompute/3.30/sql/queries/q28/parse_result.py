#
#Copyright (C) 2019-2022 Alibaba Group Holding Limited.
#
import json
import sys
#result = json.load(open(sys.argv[1]).read())
#result = open(sys.argv[1]).read()
result = json.load(open(sys.argv[1]))
print "Precision: %s" %(result["model"]["ValidPrecision"])
confusionMatrix = result["model"]["ValidConfusionMatrix"]
print "Confusion Matrix:\n%s\t%s\t%s\n%s\t%s\t%s\n%s\t%s\t%s" \
	%(confusionMatrix[0][0], confusionMatrix[0][1],confusionMatrix[0][2],confusionMatrix[1][0],confusionMatrix[1][1],confusionMatrix[1][2],confusionMatrix[2][0],confusionMatrix[2][1],confusionMatrix[2][2])

label_precision = []
label_recall =[]
label_f1score = []

for i in range(0,3):
	precision = 1.0 * confusionMatrix[i][i] / (confusionMatrix[0][i] + confusionMatrix[1][i] + confusionMatrix[2][i])
	recall = 1.0 * confusionMatrix[i][i] / (confusionMatrix[i][0] + confusionMatrix[i][1] + confusionMatrix[i][2])
	f1score = 2 * precision * recall / (precision + recall)
	label_precision.append(precision)
	label_recall.append(recall)
	label_f1score.append(f1score)

print "Label POS: Precision=%f  Recall=%f  F1-score=%f" %(label_precision[2], label_recall[2], label_f1score[2])
print "Label NEG: Precision=%f  Recall=%f  F1-score=%f" %(label_precision[0], label_recall[0], label_f1score[0])
print "Label NEU: Precision=%f  Recall=%f  F1-score=%f" %(label_precision[1], label_recall[1], label_f1score[1])

#
