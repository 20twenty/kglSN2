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

struct Voter {
   int id;
   double overlap;
   set<int> recommendations;
};

struct EdgeRec {
   double Fs;
   double FsLs;
   double FsFs;
   double LsLs;
   double LsFs;
   double LsFsLs;
   EdgeRec():Fs(0),LsLs(0),LsFs(0),LsFsLs(0){}
};

class xData {
   public:
      bool isgood() { return good_;}
      vector<Node> graph_;
      vector<int> test_;
      map<int,set<int> > validate_;
      vector<pair<int,int> > globalLeaders_, globalFollowers_;
      void getMissing(int id, map<int,EdgeRec>& myMissing, ofstream& attrFile, ofstream& edgeFile, bool isTrain);
      xData(char* trainFile, char* testFile, int seed, int limit_train, int limit_test);
      ~xData();

   private:
      bool good_; //false if reading file fails
      bool recommend_Fs_;
      bool recommend_FsLs_;
      bool recommend_FsFs_;
      bool recommend_LsLs_;
      bool recommend_LsFs_;
      bool recommend_LsFsLs_;

};
#endif

