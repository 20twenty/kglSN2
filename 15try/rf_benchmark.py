#!/usr/bin/env python

from sklearn.ensemble import RandomForestClassifier
import csv_io
import math

def isort(data):
     return sorted(range(len(data)), key = data.__getitem__, reverse=False)

def isortr(data):
     return sorted(range(len(data)), key = data.__getitem__, reverse=True)

def main():
    print "Reading training data..."
    train = csv_io.read_data("step1_train.csv")

    print "Reading testing data..."
    test = csv_io.read_data("step1_test.csv")

    target = [x[0] for x in train]
    train = [x[1:] for x in train]
    if 0:
       #indx = {0,1,4,6,7,3,5,9,11,10,2,12}  # .985
       #indx = {0,1,2,3,4,5,6,7,8,9,10,11,12,13}  # .9846
       #indx = {0,1,2,3,4,5,6,7,9,10,11,12,13}  # .9848
       #indx = {0,1,2,3,4,5,6,7,9,10,11,13}  #  .9846
       #indx = {0,1,2,3,4,5,6,7,9,10,11}  # .9847
       #indx = {0,1,3,4,5,6,7,9,10,11}  # .9848
       #indx = {0,1,3,4,5,6,7,10,11}  # 0.9847
       #indx = {0,1,3,4,5,6,7,10}  # 0.9848
       indx = {0,1,3,4,6,7,10}  # 0.9852
       #indx = {0,1,4,6,7,3,5,9}
       train = [[x[i] for i in indx] for x in train]
       test = [[x[i] for i in indx] for x in test]

    print "RF training..."
    rf = RandomForestClassifier(n_estimators=100, min_samples_split=2, n_jobs=-1, compute_importances=True, oob_score=True, max_features=2, verbose=2)
    rf.fit(train, target)
    indices = isort(rf.feature_importances_)
    for i in indices:
      print "%d - %f" % (i, rf.feature_importances_[i])

    print "oob_score_: %f" % rf.oob_score_

    exit()

    step = 890
    maxF = int(100.0/1775.0*step)
    maxF = 2
    indices = isortr(rf.feature_importances_)
    train = [[x[i] for i in indices[:step]] for x in train]
    test = [[x[i] for i in indices[:step]] for x in test]

    rf = RandomForestClassifier(n_estimators=200, min_samples_split=2, n_jobs=-1, compute_importances=True, oob_score=True, max_features=maxF, verbose=2)
    rf.fit(train, target)
    indices = isort(rf.feature_importances_)
    for i in indices:
      print "%d - %f" % (i, rf.feature_importances_[i])

    print "oob_score_: %f" % rf.oob_score_

    step = 443
    maxF = int(100.0/1775.0*step)
    indices = isortr(rf.feature_importances_)
    train = [[x[i] for i in indices[:step]] for x in train]
    test = [[x[i] for i in indices[:step]] for x in test]

    rf = RandomForestClassifier(n_estimators=400, min_samples_split=2, n_jobs=-1, compute_importances=True, oob_score=True, max_features=maxF, verbose=2)
    rf.fit(train, target)
    indices = isort(rf.feature_importances_)
    for i in indices:
      print "%d - %f" % (i, rf.feature_importances_[i])

    print "oob_score_: %f" % rf.oob_score_

    step = 220 
    maxF = int(100.0/1775.0*step)
    indices = isortr(rf.feature_importances_)
    train = [[x[i] for i in indices[:step]] for x in train]
    test = [[x[i] for i in indices[:step]] for x in test]

    rf = RandomForestClassifier(n_estimators=800, min_samples_split=2, n_jobs=-1, compute_importances=True, oob_score=True, max_features=maxF, verbose=2)
    rf.fit(train, target)
    indices = isort(rf.feature_importances_)
    for i in indices:
      print "%d - %f" % (i, rf.feature_importances_[i])

    print "oob_score_: %f" % rf.oob_score_

    print "predicting test..."
    test_probs = rf.predict_proba(test)
    test_probs = ["%f" % x[1] for x in test_probs]

    print "writing preditions..."
    csv_io.write_delimited_file("../Submissions/rf_benchmark.csv", test_probs)

if __name__=="__main__":
    main()
