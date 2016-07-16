#!/bin/bash

TA_FILE="$1"
echo "$TA_FILE"

cat "$TA_FILE" | awk '{if (NR % 2 == 1) print $0}' > "input/$TA_FILE"
cat "$TA_FILE" | awk '{if (NR % 2 == 0) print $0}' > "expected_output/$TA_FILE"
