CC=g++
CFLAGS=-c
LDFLAGS=
MTRY=2
#TREES=63
#TREES=125
TREES=200
#TREES=333
SEED=31120011
SUBF = 1
#NUMBERS = 3 4
NUMBERS = 3 4 5 6 7 8 9
NUMBERSA = 13 14 15 16 17 18 19
NUMBERSB = 23 24 25 26 27 28 29
NUMBERSC = 33 34 35 36 37 38 39
NUMBERSD = 43 44 45 46 47 48 49
#NUMBERS = 3 4 5 6 7 8 9 10 11
#NUMBERS = 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17

GROUPS = 2 3 4 5 6 7 8
LAST_GROUP=8
#TEST_SAMPLE=1000
TEST_SAMPLE=20000

test: exe xform_test rf sleep_test score
predict: exe xform_predict rf sleep_predict score

exe: bin/xform bin/rf
xform_test: workspace/xform_test
xform_predict: workspace/xform_predict
rf: workspace/rf
sleep_test:
	sleep 5
sleep_predict:
	sleep 300
score: workspace/score

bin/xform: src/xform.cpp src/xData.cpp src/xData.h
	$(CC) -I./src src/xform.cpp src/xData.cpp -o bin/xform

bin/rf: rf_src/rf.cpp rf_src/xData.cpp rf_src/xData.h rf_src/randomforest.cpp rf_src/randomforest.h
	$(CC) -I./rf_src rf_src/rf.cpp rf_src/xData.cpp rf_src/randomforest.cpp -o bin/rf

workspace/xform_predict: bin/xform
	rm -f workspace/*
	$(foreach var,$(GROUPS),(bin/xform ../../kglSN_data/train.csv ../../kglSN_data/test.csv $(SEED) 0 0 $(var) $(LAST_GROUP) > log_s1_$(var)_$(LAST_GROUP) &);)
	bin/xform ../../kglSN_data/train.csv ../../kglSN_data/test.csv $(SEED) 0 0 1 $(LAST_GROUP)
	./step1_merge.sh
	mv workspace/step1_test.csv .
	mv workspace/step1_test_edges.csv .
	rm -f workspace/*
	$(foreach var,$(GROUPS),(bin/xform ../../kglSN_data/train.csv ../../kglSN_data/test.csv $(SEED) 20000 5 $(var) $(LAST_GROUP) > log_s1_$(var)_$(LAST_GROUP) &);)
	bin/xform ../../kglSN_data/train.csv ../../kglSN_data/test.csv $(SEED) 20000 5 1 $(LAST_GROUP)
	./step1_merge.sh
	rm -f workspace/step1_*_?_$(LAST_GROUP).csv
	rm -f workspace/header*
	mv step1_test.csv workspace/
	mv step1_test_edges.csv workspace/
	rm -f workspace/rf
	touch workspace/xform_predict

workspace/xform_test: bin/xform
	rm -f workspace/*
	bin/xform ../../kglSN_data/train.csv ../../kglSN_data/test.csv $(SEED) $(TEST_SAMPLE) 10 1 $(LAST_GROUP) 1  #generate a cache if none found
	$(foreach var,$(GROUPS),(bin/xform ../../kglSN_data/train.csv ../../kglSN_data/test.csv $(SEED) $(TEST_SAMPLE) 10 $(var) $(LAST_GROUP) > log_s1_$(var)_$(LAST_GROUP) &);)
	bin/xform ../../kglSN_data/train.csv ../../kglSN_data/test.csv $(SEED) $(TEST_SAMPLE) 10 1 $(LAST_GROUP)
	./step1_merge.sh
	rm -f workspace/step1_*_?_$(LAST_GROUP).csv
	rm -f workspace/header*
	rm -f workspace/rf
	touch workspace/xform_test

workspace/rf: bin/rf
	rm -f workspace/oob_predict_*_1_$(MTRY)_$(TREES)
	rm -f workspace/tst_predict_*_1_$(MTRY)_$(TREES)
	$(foreach var,$(NUMBERS),(./bin/rf $(var) 1 $(MTRY) $(TREES) workspace/step1_train.csv workspace/step1_test.csv $(SUBF) > log_$(var)_1_$(MTRY)_$(TREES) &);)
	./bin/rf 2 1 $(MTRY) $(TREES) workspace/step1_train.csv workspace/step1_test.csv $(SUBF) > log_2_1_$(MTRY)_$(TREES)
	sleep 600
	$(foreach var,$(NUMBERSA),(./bin/rf $(var) 1 $(MTRY) $(TREES) workspace/step1_train.csv workspace/step1_test.csv $(SUBF) > log_$(var)_1_$(MTRY)_$(TREES) &);)
	./bin/rf 12 1 $(MTRY) $(TREES) workspace/step1_train.csv workspace/step1_test.csv $(SUBF) > log_2_1_$(MTRY)_$(TREES)
	sleep 600
	$(foreach var,$(NUMBERSB),(./bin/rf $(var) 1 $(MTRY) $(TREES) workspace/step1_train.csv workspace/step1_test.csv $(SUBF) > log_$(var)_1_$(MTRY)_$(TREES) &);)
	./bin/rf 22 1 $(MTRY) $(TREES) workspace/step1_train.csv workspace/step1_test.csv $(SUBF) > log_2_1_$(MTRY)_$(TREES)
	sleep 600
	#$(foreach var,$(NUMBERSC),(./bin/rf $(var) 1 $(MTRY) $(TREES) workspace/step1_train.csv workspace/step1_test.csv $(SUBF) > log_$(var)_1_$(MTRY)_$(TREES) &);)
	#./bin/rf 32 1 $(MTRY) $(TREES) workspace/step1_train.csv workspace/step1_test.csv $(SUBF) > log_2_1_$(MTRY)_$(TREES)
	#sleep 300
	#$(foreach var,$(NUMBERSD),(./bin/rf $(var) 1 $(MTRY) $(TREES) workspace/step1_train.csv workspace/step1_test.csv $(SUBF) > log_$(var)_1_$(MTRY)_$(TREES) &);)
	#./bin/rf 42 1 $(MTRY) $(TREES) workspace/step1_train.csv workspace/step1_test.csv $(SUBF) > log_2_1_$(MTRY)_$(TREES)
	#sleep 300
	rm -f workspace/score
	touch workspace/rf

workspace/score:
	./merge.pl workspace/ _ _1_$(MTRY)_$(TREES)
	./score.pl workspace/oob_predict_merge.csv workspace/tst_predict_merge.csv
	touch workspace/score

clean:
	rm -f workspace/*
	#rm -f workspace/oob_predict*
	#rm -f workspace/tst_predict*
	#rm -f workspace/step1*
	#rm -f workspace/*.csv
	#rm -f workspace/rf workspace/score workspace/xform*
	rm -f log*
