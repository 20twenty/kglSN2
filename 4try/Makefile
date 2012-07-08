CC=g++
CFLAGS=-c
LDFLAGS=
TREES=1000

all: bin/xform bin/rf workspace/xform workspace/rf1
all_old: bin/xform bin/rf workspace/xform workspace/rf1 workspace/rf2

rf_only: workspace/rf1 workspace/rf2

bin/xform: src/xform.cpp src/xData.cpp src/xData.h
	$(CC) -I./src src/xform.cpp src/xData.cpp -o bin/xform

bin/rf: rf_src/rf.cpp rf_src/xData.cpp rf_src/xData.h rf_src/randomforest.cpp rf_src/randomforest.h
	$(CC) -I./rf_src rf_src/rf.cpp rf_src/xData.cpp rf_src/randomforest.cpp -o bin/rf

workspace/xform: bin/xform
	bin/xform ../data/train.csv ../data/test.csv 31120011 0 0
	mv workspace/step1_test.csv .
	mv workspace/step1_test_edges.csv .
	bin/xform ../data/train.csv ../data/test.csv 31120011 20000 5
	mv step1_test.csv workspace/
	mv step1_test_edges.csv workspace/
	#bin/xform ../data/train.csv ../data/test.csv 31120011 10000 5
	touch workspace/xform

workspace/rf1: bin/rf
	bin/rf 2 1 1 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 1 >& log_2_1_1_$(TREES) &
	bin/rf 2 1 2 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 0 >& log_2_1_2_$(TREES) &
	bin/rf 2 1 3 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 0 >& log_2_1_3_$(TREES) 
	#bin/rf 2 1 4 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 0 >& log_2_1_4_$(TREES) &
	#bin/rf 2 1 5 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 0 >& log_2_1_5_$(TREES) &
	#bin/rf 2 1 6 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 0 >& log_2_1_6_$(TREES) 
	./score.pl workspace/oob_predict_2_1_1_$(TREES).csv workspace/tst_predict_2_1_1_$(TREES).csv
	#mv workspace/submit.csv workspace/submit1.csv
	touch workspace/rf1
	#rm -f workspace/rf2

workspace/rf1_old: bin/rf
	bin/rf 2 1 1 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 0 >& log_2_1_1_$(TREES) 
	#bin/rf 2 1 2 $(TREES) workspace/step1_train.csv workspace/step1_test.csv 0 >& log_2_1_2_$(TREES) 
	./score.pl workspace/oob_predict_2_1_1_$(TREES).csv workspace/tst_predict_2_1_1_$(TREES).csv
	./score.pl workspace/oob_predict_2_1_2_$(TREES).csv workspace/tst_predict_2_1_2_$(TREES).csv
	touch workspace/rf1
	rm -f workspace/rf2

workspace/rf2: bin/rf
	./merge.pl workspace _2_1_ _$(TREES)
	bin/rf 3 1 1 $(TREES) workspace/step2_train.csv workspace/step2_test.csv 0
	#bin/rf 3 1 1 $(TREES) workspace/step2_train.csv workspace/step2_test.csv 0
	#./score.pl workspace/oob_predict_2_1_2_$(TREES).csv workspace/tst_predict_2_1_2_$(TREES).csv
	#./score.pl workspace/oob_predict_2_1_3_$(TREES).csv workspace/tst_predict_2_1_3_$(TREES).csv
	#./score.pl workspace/oob_predict_2_1_4_$(TREES).csv workspace/tst_predict_2_1_4_$(TREES).csv
	./score.pl workspace/oob_predict_3_1_1_$(TREES).csv workspace/tst_predict_3_1_1_$(TREES).csv
	#./score.pl workspace/oob_predict_3_1_2_$(TREES).csv workspace/tst_predict_3_1_2_$(TREES).csv
	touch workspace/rf2
