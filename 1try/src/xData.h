#ifndef XDATA_H
#define XDATA_H

#include <vector>
#include <set>
#include <map>
#include <iostream>

using namespace std;

struct Node {
   set<int> leaders;
   set<int> followers;
   set<int> leaders_only;
   set<int> friends;
   set<int> followers_only;
   double pFO, pLO, pFR;
   double myLsConnectBack_rate;
   double myFsConnectBack_rate;
};

struct Neighbor {
   int id;
   double overlap;
   set<int> recommendations;
};

struct EdgeRec {
   double LsFs;
   double LsFsLs;
   double Recip;
   double Recip2;
   EdgeRec():LsFs(0),LsFsLs(0),Recip(0),Recip2(0){}
};

class xData {
   public:
      bool isgood() { return good_;}
      vector<Node> graph_;
      vector<int> test_;
      map<int,set<int> > validate_;
      vector<pair<int,int> > globalLeaders_, globalFollowers_;
      void getMissing(int id, map<int,EdgeRec>& myMissing, ofstream& attrFile, ofstream& edgeFile, bool isTrain);
      xData(char* trainFile, char* testFile, int fold, int folds);
      ~xData();

   private:
      bool good_; //false if reading file fails

};
#endif

