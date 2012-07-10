CC=g++
CFLAGS=-c
LDFLAGS=
MTRY=1
TREES=200
SUBF = 0
NUMBERS = 3 4 5 6 7 8 9
NUMBERSA = 13 14 15 16 17 18 19
NUMBERSB = 23 24 25 26 27 28 29
NUMBERSC = 33 34 35 36 37 38 39
NUMBERSD = 43 44 45 46 47 48 49

GROUPS = 2 3 4 5 6 7 8
LAST_GROUP=8

all: exe rf sleep_predict score

exe: bin/rf
rf: workspace/rf
sleep_predict:
	sleep 500
score: workspace/score

bin/rf: rf_src/rf.cpp rf_src/xData.cpp rf_src/xData.h rf_src/randomforest.cpp rf_src/randomforest.h
	$(CC) -I./rf_src rf_src/rf.cpp rf_src/xData.cpp rf_src/randomforest.cpp -o bin/rf

workspace/rf: bin/rf
	rm -f workspace/oob_predict_*_1_$(MTRY)_$(TREES)
	rm -f workspace/tst_predict_*_1_$(MTRY)_$(TREES)
	$(foreach var,$(NUMBERS),(./bin/rf $(var) 1 $(MTRY) $(TREES) workspace/step2_train.csv workspace/step2_test.csv $(SUBF) > log_$(var)_1_$(MTRY)_$(TREES) &);)
	./bin/rf 2 1 $(MTRY) $(TREES) workspace/step2_train.csv workspace/step2_test.csv $(SUBF) > log_2_1_$(MTRY)_$(TREES)
	#sleep 600
	#$(foreach var,$(NUMBERSA),(./bin/rf $(var) 1 $(MTRY) $(TREES) workspace/step2_train.csv workspace/step2_test.csv $(SUBF) > log_$(var)_1_$(MTRY)_$(TREES) &);)
	#./bin/rf 12 1 $(MTRY) $(TREES) workspace/step2_train.csv workspace/step2_test.csv $(SUBF) > log_12_1_$(MTRY)_$(TREES)
	#sleep 600
	#$(foreach var,$(NUMBERSB),(./bin/rf $(var) 1 $(MTRY) $(TREES) workspace/step2_train.csv workspace/step2_test.csv $(SUBF) > log_$(var)_1_$(MTRY)_$(TREES) &);)
	#./bin/rf 22 1 $(MTRY) $(TREES) workspace/step2_train.csv workspace/step2_test.csv $(SUBF) > log_22_1_$(MTRY)_$(TREES)
	#sleep 600
	#$(foreach var,$(NUMBERSC),(./bin/rf $(var) 1 $(MTRY) $(TREES) workspace/step2_train.csv workspace/step2_test.csv $(SUBF) > log_$(var)_1_$(MTRY)_$(TREES) &);)
	#./bin/rf 32 1 $(MTRY) $(TREES) workspace/step2_train.csv workspace/step2_test.csv $(SUBF) > log_32_1_$(MTRY)_$(TREES)
	#sleep 600
	#$(foreach var,$(NUMBERSD),(./bin/rf $(var) 1 $(MTRY) $(TREES) workspace/step2_train.csv workspace/step2_test.csv $(SUBF) > log_$(var)_1_$(MTRY)_$(TREES) &);)
	#./bin/rf 42 1 $(MTRY) $(TREES) workspace/step2_train.csv workspace/step2_test.csv $(SUBF) > log_42_1_$(MTRY)_$(TREES)
	#sleep 600
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
