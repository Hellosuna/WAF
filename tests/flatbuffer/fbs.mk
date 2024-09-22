
all: date int_array point pair_array

.PHONY: date
date: date.fbs
	bin/flatc --cpp -o include/ date.fbs

.PHONY: int_array
int_array: int_array.fbs
	bin/flatc --cpp -o include/ int_array.fbs


.PHONY: point
point: point.fbs
	bin/flatc --cpp -o include/ point.fbs

.PHONY: pair_array
point: pair_array.fbs
	bin/flatc --cpp -o include/ pair_array.fbs

.PHONY: long_array
point: long_array.fbs
	bin/flatc --cpp -o include/ long_array.fbs