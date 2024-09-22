#!/usr/bin/env bash

#
# Copyright (C) 2019 Transaction Processing Performance Council (TPC) and/or
# its contributors.
#
# This file is part of a software package distributed by the TPC.
#
# The contents of this file have been developed by the TPC, and/or have been
# licensed to the TPC under one or more contributor license agreements.
#
#  This file is subject to the terms and conditions outlined in the End-User
#  License Agreement (EULA) which can be found in this distribution (EULA.txt)
#  and is available at the following URL:
#  http://www.tpc.org/TPC_Documents_Current_Versions/txt/EULA.txt
#
# Unless required by applicable law or agreed to in writing, this software
# is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
# ANY KIND, either express or implied, and the user bears the entire risk as
# to quality and performance as well as the entire cost of service or repair
# in case of defect.  See the EULA for more details.
#

#
#Copyright (C) 2019-2022 Alibaba Group Holding Limited.
#

# define used temp tables
TEMP_TABLE1="${TEMP_TABLE}_click"
TEMP_TABLE2="${TEMP_TABLE}_sale"

query_run_main_method () {
	QUERY_SCRIPT="$QUERY_SQL_DIR/$QUERY_NAME.sql"
        sed -i s/'${TEMP_TABLE1}'/$TEMP_TABLE1/g $QUERY_SQL_DIR/${QUERY_NAME}.sql
        sed -i s/'${TEMP_TABLE2}'/$TEMP_TABLE2/g $QUERY_SQL_DIR/${QUERY_NAME}.sql
	if [ ! -r "$QUERY_SCRIPT" ]
	then
		echo "SQL file $QUERY_SCRIPT can not be read."
		exit 1
	fi

        runCmdWithErrorCheck runEngineCmd -f "$QUERY_SCRIPT"
	return $?
}

query_run_clean_method () {
	runCmdWithErrorCheck runEngineCmd --project=$BIG_BENCH_DATABASE -e "DROP VIEW IF EXISTS $TEMP_TABLE1; DROP VIEW IF EXISTS $TEMP_TABLE2; DROP TABLE IF EXISTS $RESULT_TABLE;"
	return $?
}

query_run_validate_method () {
	# perform exact result validation if using SF 1, else perform general sanity check
        local RESULT_FILE=$BIG_BENCH_ENGINE_TEMP_DIR/"$RESULT_TABLE"_result
	if [ "$BIG_BENCH_SCALE_FACTOR" -eq 1 ]
	then
                runCmdWithErrorCheck runEngineCmd --project=$BIG_BENCH_DATABASE -e "tunnel download $RESULT_TABLE $RESULT_FILE"
		local VALIDATION_PASSED="1"

		if [ ! -f "$VALIDATION_RESULTS_FILENAME" ]
		then
			echo "Golden result set file $VALIDATION_RESULTS_FILENAME not found"
			VALIDATION_PASSED="0"
		fi

                if diff -q "$VALIDATION_RESULTS_FILENAME" <(cat $RESULT_FILE)
		then
			echo "Validation of $VALIDATION_RESULTS_FILENAME passed: Query returned correct results"
		else
			echo "Validation of $VALIDATION_RESULTS_FILENAME failed: Query returned incorrect results"
			VALIDATION_PASSED="0"
		fi
		if [ "$VALIDATION_PASSED" -eq 1 ]
		then
			echo "Validation passed: Query results are OK"
		else
			echo "Validation failed: Query results are not OK"
			return 1
		fi
	else
                runCmdWithErrorCheck runEngineCmd --project=$BIG_BENCH_DATABASE -e "tunnel download $RESULT_TABLE $RESULT_FILE -limit 10"
		if [ `cat $RESULT_FILE | head -n 10 | wc -l` -ge 1 ]
		then
			echo "Validation passed: Query returned results"
		else
			echo "Validation failed: Query did not return results"
			return 1
		fi
	fi
}
