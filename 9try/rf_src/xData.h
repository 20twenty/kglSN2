#ifndef XDATA_H
#define XDATA_H

#include <vector>

using namespace std;

class xData {
   public:
      vector<double> trn_labl;
      vector< vector<double> > trn_attr;
      vector< vector<double> > trn_attr_cf;
      vector< vector<double> > tst_attr;
      vector< vector<double> > tst_attr_cf;
      vector<int> attr_class;
      vector<double> attr_avg;
      int trn_rows, trn_cols, tst_rows, tst_cols;

      bool isgood() { return good_;}
      xData(char* trainFile, char* testFile);
      ~xData();

   private:
      bool good_; //false if reading file fails

};
#endif
