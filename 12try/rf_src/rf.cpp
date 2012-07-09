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
#include "randomforest.h"

using namespace std;

int main(int argc, char** argv) {
   if(argc != 8) {
      cerr << "args: <fold> <num_folds> <Mtry> <trees> <train_file> <test_file> <subset_on>" << endl;
      return 1;
   }
   bool subset_on = (atoi(argv[7]) == 1) ? true : false;
   //xData* pXD = new xData("../data/train.csv","../data/test.csv");
   xData* pXD = new xData(argv[5],argv[6]);
   if (!pXD->isgood()) return 1;
   RandomForest* pRF = new RandomForest(pXD,atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),subset_on);
   if (!pRF->isgood()) return 1;
   return 0;
}
