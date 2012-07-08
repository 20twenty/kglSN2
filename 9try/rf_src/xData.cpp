#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <limits>
#include "xData.h"

using namespace std;

xData::xData(char* trainFile, char* testFile) {
   good_ = true;
   //const char* trainFile = "../data/train.csv";
   ifstream fTrain(trainFile);
   if(!fTrain.is_open()) {
      cerr << "Could not open " << trainFile << endl;
      good_ = false;
   }

   //const char* testFile = "../data/test.csv";
   ifstream fTest(testFile);
   if(!fTest.is_open()) {
      cerr << "Could not open " << testFile << endl;
      good_ = false;
   }

   string line;
   string token;
   char delim=',';
   double rcval;

   vector<double> trn_minV, trn_maxV, trn_sumV;
   vector<double> tst_minV, tst_maxV, tst_sumV;

   trn_rows=0;
   trn_cols=0;
   tst_rows=0;
   tst_cols=0;

   {
      cerr << "reading train.csv..." << flush;
      int row = 0;
      bool header_row = true;  //header_row is first row in csv
      while(fTrain.good() /*&& row < 10*/) {
         getline(fTrain,line);
         if(line.length() == 0) continue;
         stringstream ss(line);
         int col = 0;
         bool label_col = true;  //label column is first column in csv
         vector<double> row_data(trn_cols,0.0);
         while( getline(ss, token, delim)) {
            if(header_row) {
               if(label_col) {
                  label_col = false;
               } else {
                  col++;
               }
            } else {
               rcval = atof(token.c_str());
               if(label_col) {
                  label_col = false;
                  trn_labl.push_back(rcval);
               } else {
                  if(token.size() == 0) {
                     cerr << endl << "Missing data found at row: " << row << ", col: " << col << endl;
                     good_ = false;
                  }
                  if(rcval>trn_maxV[col]) trn_maxV[col]=rcval;
                  if(rcval<trn_minV[col]) trn_minV[col]=rcval;
                  trn_sumV[col]+=(double)rcval;
                  row_data[col] = rcval;
                  //trn_attr_cf[col].push_back(rcval);
                  col++;
               }
            }
         }
         if(header_row) {
            trn_cols = col;
            header_row = false;
            trn_sumV.assign(trn_cols,0);
            trn_minV.assign(trn_cols,numeric_limits<double>::max());
            trn_maxV.assign(trn_cols,-numeric_limits<double>::max());
            //trn_attr_cf.assign(trn_cols,vector<double>());
         } else {
            if(col != trn_cols) {
               cerr << endl << "training data has inconsistent column count" << endl;
               good_ = false;
               return;
            }
            trn_attr.push_back(row_data);
            row++;
         }
      }
      trn_rows = row;
      fTrain.close();
      cerr << "done" << endl;
      cerr << "   trn_labl_rows: " << trn_labl.size() << endl;
      cerr << "   trn_rows: " << trn_rows << "; trn_cols: " << trn_cols << endl;
   }

   {
      cerr << "reading test.csv..." << flush;
      int row = 0;
      bool header_row = true;  //header_row is first row in csv
      while(fTest.good() /*&& row < 10*/) {
         int col = 0;
         getline(fTest,line);
         if(line.length() == 0) continue;
         stringstream ss(line);
         vector<double> row_data(tst_cols,0.0);
         while( getline(ss, token, delim)) {
            if(header_row) {
               col++;
            } else {
               if(token.size() == 0) {
                  cerr << endl << "Missing data found at row: " << row << ", col: " << col << endl;
                  good_ = false;
               }
               rcval = atof(token.c_str());
               if(rcval>tst_maxV[col]) tst_maxV[col]=rcval;
               if(rcval<tst_minV[col]) tst_minV[col]=rcval;
               tst_sumV[col]+=(double)rcval;
               row_data[col] = rcval;
               //tst_attr_cf[col].push_back(rcval);
               col++;
            }
         }
         if(header_row) {
            tst_cols = col;
            if(tst_cols != trn_cols) {
               cerr << endl << "ERROR: tst_cols does not equal trn_cols" << endl;
               cerr << "   trn_cols: " << trn_cols << "; tst_cols: " << tst_cols << endl;
               good_ = false;
            }
            header_row = false;
            tst_sumV.assign(trn_cols,0);
            tst_minV.assign(trn_cols,numeric_limits<double>::max());
            tst_maxV.assign(trn_cols,-numeric_limits<double>::max());
            //tst_attr_cf.assign(tst_cols,vector<double>());
         } else {
            if(col != tst_cols) {
               cerr << endl << "test data has inconsistent column count" << endl;
               good_ = false;
               return;
            }
            tst_attr.push_back(row_data);
            row++;
         }
      }
      tst_rows = row;
      fTrain.close();
      cerr << "done" << endl;
      cerr << "   tst_rows: " << tst_rows << "; tst_cols: " << tst_cols << endl;
   }

   attr_class.assign(trn_cols,0);
   attr_avg.assign(trn_cols,0.0);
   vector<double> trn_attr_avg(trn_cols,0);
   vector<double> tst_attr_avg(tst_cols,0);

   for(int col=0; col<trn_cols; col++) {
      for(int row=0; row<trn_rows; row++) {
         attr_avg[col]+=trn_attr[row][col];
         trn_attr_avg[col]+=trn_attr[row][col];
         if(trn_attr[row][col] != 0 and trn_attr[row][col] != 1) attr_class[col] = 1;
      }
      for(int row=0; row<tst_rows; row++) {
         attr_avg[col]+=tst_attr[row][col];
         tst_attr_avg[col]+=tst_attr[row][col];
         if(tst_attr[row][col] != 0 and tst_attr[row][col] != 1) attr_class[col] = 1;
      }
      attr_avg[col] /= (double)(trn_rows+tst_rows);
      trn_attr_avg[col] /= (double)trn_rows;
      tst_attr_avg[col] /= (double)tst_rows;
      double frac = tst_attr_avg[col]/trn_attr_avg[col];
      if(frac < 1) frac = 1/frac;
      if(frac > 3 && 0) {
         cerr << "attr: " << col << "; frac: " << frac << "; trn: " << trn_attr_avg[col] \
              << "; tst: " << tst_attr_avg[col] << endl;
      }
   }
   vector<int> class_cnt(2,0);
   for(int col=0; col<trn_cols; col++) {
      class_cnt[attr_class[col]]++;
   }
   cerr << "binary: " << class_cnt[0] << "; continuous: " << class_cnt[1] << endl;

   //int myints[] = {142,504,181,92,218,52,62,57,150,74,53,91,208,49,54,193,130,177,125,43,141,67,204,32,75,129,149,206,69,138,910,64,950,179,145,47,144,10,79,66,68,100,174,197,172,217,151,201,198,60,86,200,87,38,176,94,99,44,203,31,180,195,18,207,30,1,73,20,37,101,199,85,102,13,98,25,89,19,83,46,15,88,24,16,90,4,106,104,45,5,103,17,6,7,9,29,8,105,14};
   //int myints[] = {180,195,18,207,30,1,73,20,37,101,199,85,102,13,98,25,89,19,83,46,15,88,24,16,90,4,106,104,45,5,103,17,6,7,9,29,8,105,14};
   //int myints[] = {15,88,24,16,90,4,106,104,45,5,103,17,6,7,9,29,8,105,14};
   //int myints[] = {29,8,105,14};
   int myints[]={};
   set<int> bad_features(myints,myints+sizeof(myints)/sizeof(int));
   for(set<int>::iterator it = bad_features.begin(); it != bad_features.end(); it++) {
      for(int row=0; row<trn_rows; row++) {
         trn_attr[row][*it] = 0;
      }
      for(int row=0; row<tst_rows; row++) {
         tst_attr[row][*it] = 0;
      }
   }
}

xData::~xData() {}
