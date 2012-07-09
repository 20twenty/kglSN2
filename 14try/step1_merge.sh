#!/bin/bash

# wait for all files to close
set -x  # turn on debugging mode
dir="workspace/"

[ ! -d "$dir" ] && {  echo "Directory $dir does not exists."; exit 1; }

ok=0
while [ $ok -ne 1 ]
do
   lsof +D $dir | grep -q COMMAND &>/dev/null
   if [ $? -ne 0 ]
   then
      ok=1
   else
      echo "Warning: Open files found in $dir"
      sleep 5
   fi
done

cat workspace/step1_validate_?_8.csv > workspace/step1_validate.csv

head -n 1 workspace/step1_train_1_8.csv > workspace/header1
head -n 1 workspace/step1_train_edges_1_8.csv > workspace/header2
head -n 1 workspace/step1_test_1_8.csv > workspace/header3
head -n 1 workspace/step1_test_edges_1_8.csv > workspace/header4

sed -i '1d' workspace/step1_t*.csv

cat workspace/header1 workspace/step1_train_?_8.csv > workspace/step1_train.csv
cat workspace/header2 workspace/step1_train_edges_?_8.csv > workspace/step1_train_edges.csv
cat workspace/header3 workspace/step1_test_?_8.csv > workspace/step1_test.csv
cat workspace/header4 workspace/step1_test_edges_?_8.csv > workspace/step1_test_edges.csv
