#ifndef XDATA_H
#define XDATA_H

#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <math.h>

using namespace std;

struct Node {
   set<int> leaders;
   set<int> followers;
   double pFO, pLO, pFR;
   double myLsConnectBack_rate;
   double myFsConnectBack_rate;
   double myLsLsConnectBack_rate;
   double myLsFsConnectBack_rate;
   double myFsLsConnectBack_rate;
   double myFsFsConnectBack_rate;
};

struct Voter {
   int id;
   double overlap;
   set<int> recommendations;
};

struct EdgeRec2 {
   map<string,pair<int,int> > prAfBtype;
   map<string,pair<int,int> > prBlAtype;
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
      int recGetDepth (int A, vector<int>& myDepth, int depth);
      void writeFiles(int source, map<int,map<string,pair<double,int> > >& prAfBrecSum, map<int,double>& bank, ofstream& attrFile, ofstream& edgeFile, bool isTrain);
      void getMissing3 (map<int,pair<double,double> >& rcvd_gift, map<int,double>& bank, int depth, int maxdepth);
      void getMissing2(int source, int dest, map<int,map<string,pair<double,int> > >& prAfBrecSum, string type, int depth, int maxdepth);
      void getMissing(int id, map<int,EdgeRec>& myMissing, ofstream& attrFile, ofstream& edgeFile, bool isTrain);

      xData(char* trainFile, char* testFile, int seed, int limit_train, int limit_test, int group, int groups, bool cache_only);
      ~xData();

   private:
      bool good_; //false if reading file fails
      bool recommend_Fs_;
      bool recommend_FsLs_;
      bool recommend_FsFs_;
      bool recommend_LsLs_;
      bool recommend_LsFs_;
      bool recommend_LsFsLs_;
      set<string> types_;
};
#endif

