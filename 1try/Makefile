CC=g++
CFLAGS=-c
LDFLAGS=
TREES=100

all: bin/xform bin/rf workspace/xform workspace/rf1 workspace/rf2

rf_only: workspace/rf1 workspace/rf2

bin/xform: src/xform.cpp src/xData.cpp src/xData.h
	$(CC) -I./src src/xform.cpp src/xData.cpp -L./src -lgzstream -lz -o bin/xform

bin/rf: rf_src/rf.cpp rf_src/xData.cpp rf_src/xData.h rf_src/randomforest.cpp rf_src/randomforest.h
	$(CC) -I./rf_src rf_src/rf.cpp rf_src/xData.cpp rf_src/randomforest.cpp -o bin/rf

workspace/xform: bin/xform
	bin/xform ../data/train.csv ../data/test.csv 3112001 1000
	touch workspace/xform

workspace/rf1: bin/rf
	bin/rf 2 1 1 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 0 >& log_2_1_1_$(TREES) &
	bin/rf 2 1 2 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 0 >& log_2_1_2_$(TREES) &
	bin/rf 2 1 3 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 0 >& log_2_1_3_$(TREES) 
	touch workspace/rf1
	rm -f workspace/rf2

workspace/rf2: bin/rf
	./merge.pl workspace _2_1_ _$(TREES)
	bin/rf 3 1 1 $(TREES) workspace/step2_train.csv workspace/step2_test.csv 0
	#./score.pl workspace/oob_predict_2_1_1_$(TREES).csv workspace/tst_predict_2_1_1_$(TREES).csv
	#./score.pl workspace/oob_predict_2_1_2_$(TREES).csv workspace/tst_predict_2_1_2_$(TREES).csv
	#./score.pl workspace/oob_predict_2_1_3_$(TREES).csv workspace/tst_predict_2_1_3_$(TREES).csv
	./score.pl workspace/oob_predict_3_1_1_$(TREES).csv workspace/tst_predict_3_1_1_$(TREES).csv
	touch workspace/rf2
