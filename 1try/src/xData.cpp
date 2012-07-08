#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//#include "gzstream.h"
#include "xData.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <algorithm>
#include <math.h>

using namespace std;

void venn(set<int>& a, set<int>& b, int& ao_cnt, int& bo_cnt, int& ab_cnt, set<int>* a_only, set<int>* b_only, set<int>* a_b) {
   set<int>::iterator it_a = a.begin(); // my leaders
   set<int>::iterator it_a_end = a.end(); 
   set<int>::iterator it_b = b.begin(); // my leaders followers leaders
   set<int>::iterator it_b_end = b.end(); 
   set<int>::iterator it_a_only, it_b_only, it_a_b;
   if(a_only != NULL) it_a_only = a_only->begin(); 
   if(b_only != NULL) it_b_only = b_only->begin();
   if(a_b != NULL) it_a_b = a_b->begin();
   while(it_a != it_a_end || it_b != it_b_end) {
      //cerr << *it_a << "," << *it_b << endl;
      if(it_a == it_a_end) {
         if(b_only != NULL) {it_b_only = b_only->insert(it_b_only,*it_b);}
         bo_cnt++;
         it_b++;
         continue;
      }
      if(it_b == it_b_end) {
         if(a_only != NULL) {it_a_only = a_only->insert(it_a_only,*it_a);}
         ao_cnt++;
         it_a++;
         continue;
      }
      if (*it_a == *it_b) {
         if(a_b != NULL) {it_a_b = a_b->insert(it_a_b,*it_a);}
         ab_cnt++;
         it_a++;
         it_b++;
         continue;
      }
      if (*it_a < *it_b) {
         if(a_only != NULL) {it_a_only = a_only->insert(it_a_only,*it_a);}
         ao_cnt++;
         it_a++;
      } else {
         if(b_only != NULL) {it_b_only = b_only->insert(it_b_only,*it_b);}
         bo_cnt++;
         it_b++;
      }
   }
}

int int_min(int a, int b) {
   if (a < b) return a;
   else return b;
}

bool sort_v_p_i_i_d(pair<int,int> a, pair<int,int> b) {
   return a.second > b.second;
}

bool sort_v_p_i_d_d(pair<int,double> a, pair<int,double> b) {
   if(a.second == b.second) return a.first < b.first;
   else return a.second > b.second;
}

bool sort_v_n_d(Neighbor a, Neighbor b) {
   return a.overlap > b.overlap;
}

void xData::getMissing (int id, map<int,EdgeRec>& myMissing, ofstream& attrFile, ofstream& edgeFile, bool isTrain) {
   bool recommend_LsFsLs = true;
   bool recommend_FRsFRs = true;
   bool recommend_recip = true;
   if(recommend_LsFsLs) {
      set<int> myLsFs; // my leaders followers
      //cerr << "find my leaders followers" << endl;
      for(set<int>::iterator it_leaders = graph_[id].leaders.begin(); it_leaders != graph_[id].leaders.end(); it_leaders++) {
         int leader = *it_leaders;
         myLsFs.insert(graph_[leader].followers.begin(),graph_[leader].followers.end());
      }
      myLsFs.erase(id); // need to remove myself from my leaders followers
      vector<Neighbor> neighbors;
      //cerr << "calculate distance and find recs" << endl;
      for(set<int>::iterator it_myLsFs = myLsFs.begin(); it_myLsFs != myLsFs.end(); it_myLsFs++) {
         Neighbor n;
         set<int> a, a_b;
         int ao_cnt=0, bo_cnt=0, ab_cnt=0;
         venn(graph_[id].leaders, graph_[*it_myLsFs].leaders, ao_cnt, bo_cnt, ab_cnt, NULL, &(n.recommendations) /*b*/, NULL);
         n.id=*it_myLsFs;
         n.overlap=(double)ab_cnt/(double)(ao_cnt+bo_cnt+ab_cnt);
         neighbors.push_back(n);
      }
      sort(neighbors.begin(),neighbors.end(),sort_v_n_d);
      int n_count=0;
      int nL_count=0;
      for(vector<Neighbor>::iterator it_n=neighbors.begin(); it_n != neighbors.end(); it_n++) {
         if(n_count < 10) {
            if(graph_[id].leaders.find(it_n->id) == graph_[id].leaders.end()) {
               myMissing[it_n->id].LsFs+=it_n->overlap;
               n_count++;
            }
         }
         if(nL_count < 10) {
            for(set<int>::iterator it_r=it_n->recommendations.begin(); it_r!=it_n->recommendations.end(); it_r++) {
               myMissing[*it_r].LsFsLs+=it_n->overlap / ((double)int_min(10,neighbors.size())*(double)it_n->recommendations.size());
            }
            nL_count++;
         }
         if(n_count == 10 && nL_count == 10) break;
      }
   }
   if(recommend_recip) {
      for(set<int>::iterator it_fo = graph_[id].followers_only.begin(); it_fo != graph_[id].followers_only.end(); it_fo++) {
         myMissing[*it_fo].Recip=graph_[*it_fo].myLsConnectBack_rate;
         myMissing[*it_fo].Recip2=1;
      }
   }
   for(map<int,EdgeRec>::iterator it_missing = myMissing.begin(); it_missing != myMissing.end(); it_missing++) {
      if(isTrain) {
         if(validate_[id].find(it_missing->first) != validate_[id].end()) attrFile << "1,";
         else attrFile << "0,";
      }
      attrFile << it_missing->second.LsFs << "," << it_missing->second.LsFsLs << "," << it_missing->second.Recip << endl << flush;
      edgeFile << id << "," << it_missing->first << endl << flush;
   }

}

xData::xData(char* trainFile, char* testFile, int seed, int limit) {
   good_ = true;
   //igzstream fTrain(trainFile);
   ifstream fTrain(trainFile);
   if(!fTrain.good()) {
      cerr << "Could not open " << trainFile << endl;
      good_ = false;
   }

   //igzstream fTest(testFile);
   ifstream fTest(testFile);
   if(!fTest.good()) {
      cerr << "Could not open " << testFile << endl;
      good_ = false;
   }

   string line;
   string token;
   char delim=',';
   int last_id = 1862220;
   
   //srand(3112001);
   srand(seed);

   {
      cerr << "reading trainFile: " << trainFile << " ..." << flush;
      int row = 0;
      bool header_row = true;  //header_row is first row in csv
      graph_.assign(last_id+1,Node());
      globalLeaders_.assign(last_id+1,pair<int,int>(0,0));
      globalFollowers_.assign(last_id+1,pair<int,int>(0,0));
      while(fTrain.good() /*&& row < 10*/) {
         getline(fTrain,line);
         if(line.length() == 0) continue;
         stringstream ss(line);
         int col = 0;
         int source = -1;
         int dest = -1;
         while( getline(ss, token, delim)) {
            if (header_row) {
               header_row = false;
               break;
            }
            if (col > 1) {
               good_ = false;
               cerr << "Too many columns!" << endl;
               return;
            }
            if (col == 0) {
               source = atoi(token.c_str());
               if(source > last_id) {
                  cerr << "source: " << source << " is greater that last_id specified: " << last_id << endl;
                  good_=false;
                  return;
               }
            } else {
               dest = atoi(token.c_str());
               if(dest > last_id) {
                  cerr << "dest: " << dest << " is greater that last_id specified: " << last_id << endl;
                  good_=false;
                  return;
               }
               //create edge
               //cerr << "source: " << source << "; dest: " << dest << endl;
               graph_[source].leaders.insert(dest);
               graph_[dest].followers.insert(source);
            }
            col++;
         }
      }
      fTrain.close();
      cerr << "done" << endl;
   }

   int vcnt=0;
   set<int>::iterator it_dest;
   while (validate_.size() < 175000) {
      int source = rand() % graph_.size();
      int lsize = graph_[source].leaders.size();
      int fsize = graph_[source].followers.size();
      if((lsize+fsize) <= 1 || lsize == 0 || validate_[source].size() == 10) continue;
      it_dest = graph_[source].leaders.begin();
      for(int i=0; i < (rand() % lsize); i++) {it_dest++;}
      if((graph_[*it_dest].followers.size() + graph_[*it_dest].leaders.size()) <= 1) continue;
      // remove the edge from the graph and add it to the validate set
      validate_[source].insert(*it_dest);
      graph_[source].leaders.erase(*it_dest);
      graph_[*it_dest].followers.erase(source);
      vcnt++;
   }

   int tn = 0, tnnl = 0, tnnf = 0, tnnlf;
   {
      cerr << "reading testFile: " << testFile << " ..." << flush;
      bool header_row = true;  //header_row is first row in csv
      while(fTest.good() /*&& row < 10*/) {
         getline(fTest,line);
         if(header_row) {
            header_row = false;
            continue;
         }
         if(line.length() == 0) continue;
         stringstream ss(line);
         int col = 0;
         int source = atoi(line.c_str());
         if(source < 1 || source > last_id) {
            good_ = false;
            cerr << "follower id " << source << " is outside range in training data" << endl;
            return;
         }
         test_.push_back(source);
         if(graph_[source].leaders.size() < 1) {
            if(graph_[source].followers.size() < 1) tnnlf++;
            else tnnl++;
         } else if(graph_[source].followers.size() < 1) tnnf++;
         tn++;
      }
      fTest.close();
      cerr << "done" << endl;
   }
   int vnnl=0;
   int vnnf=0;
   int vnnlf=0;
   for(map<int,set<int> >::iterator it = validate_.begin(); it != validate_.end(); it++) {
      if(graph_[it->first].leaders.size() < 1) {
         if(graph_[it->first].followers.size() < 1) vnnlf++;
         else vnnl++;
      } else if(graph_[it->first].followers.size() < 1) vnnf++;
   }

   cerr << "nodes in test set: " << tn << endl;
   cerr << "nodes in test set w/ no leaders: " << tnnl << endl;
   cerr << "nodes in test set w/ no followers: " << tnnf << endl;
   cerr << "nodes in test set w/ no connections: " << tnnlf << endl;
   cerr << "nodes in validate set: " << validate_.size() << endl;
   cerr << "nodes in validate set w/ no leaders: " << vnnl << endl;
   cerr << "nodes in validate set w/ no followers: " << vnnf << endl;
   cerr << "nodes in validate set w/ no connections: " << vnnlf << endl;
   
   if(0) {
      for(int id = 1; id < graph_.size(); id++) {
         globalFollowers_[id].first = id;
         globalFollowers_[id].second = graph_[id].leaders.size();
         globalLeaders_[id].first = id;
         globalLeaders_[id].second = graph_[id].followers.size();
      }
      sort(globalFollowers_.begin(), globalFollowers_.end(), sort_v_p_i_i_d);
      sort(globalLeaders_.begin(), globalLeaders_.end(), sort_v_p_i_i_d);
      cerr << "Top followers..." << endl;
      for(int n=0; n<10; n++) {
         cerr << "   id: " << globalFollowers_[n].first << "; followingCnt: " << globalFollowers_[n].second << endl;
      }
      cerr << "Top leaders..." << endl;
      for(int n=0; n<10; n++) {
         cerr << "   id: " << globalLeaders_[n].first << "; leadingCnt: " << globalLeaders_[n].second << endl;
      }
   }
   cerr << "Predicting missing edges..." << endl;
   cerr << "   determining friends and connect back rates..." << endl;
   for(int i=0; i<graph_.size(); i++) {
      if((graph_[i].leaders.size() == 0 || graph_[i].followers.size() == 0) && 0) {
         graph_[i].myLsConnectBack_rate = 0.5;
         graph_[i].myFsConnectBack_rate = 0.5;
      } else {
         int ao_cnt=0, bo_cnt=0, ab_cnt=0;
         set<int> ab;
         venn(graph_[i].leaders, graph_[i].followers, ao_cnt, bo_cnt, ab_cnt, &(graph_[i].leaders_only), &(graph_[i].followers_only), &(graph_[i].friends));
         graph_[i].pLO = (double)(ao_cnt+1)/(double)(ao_cnt+bo_cnt+ab_cnt+2);
         graph_[i].pFO = (double)(bo_cnt+1)/(double)(ao_cnt+bo_cnt+ab_cnt+2);
         graph_[i].pFR = (double)(ab_cnt+1)/(double)(ao_cnt+bo_cnt+ab_cnt+2);

         graph_[i].myLsConnectBack_rate = (double)(ab_cnt+1)/(double)(ao_cnt+ab_cnt+2);
         graph_[i].myFsConnectBack_rate = (double)(ab_cnt+1)/(double)(bo_cnt+ab_cnt+2);

      }
      //cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" \
           << i+1 << "/" << graph_.size() << "; fbr: " << graph_[i].myLsConnectBack_rate << endl;
   }
   cerr << endl;
   
   ofstream rfTrain("workspace/step1_train.csv");  // contains feature data for each possible missing edge. first col specifies actual edge.
   ofstream rfTrainEdges("workspace/step1_train_edges.csv");   // contains source,dest for all possible missing connections
   
   ofstream rfTest("workspace/step1_test.csv");   // contains feature data for each possible missing edge
   ofstream rfTestEdges("workspace/step1_test_edges.csv");   // contains source,dest for all possible missing connections
   
   ofstream rfValidate("workspace/step1_validate.csv"); //contains source,dest,dest,dest...

   if(!rfTrain.is_open()) {
      good_=false;
      cerr << "Cannot open workspace/step1_train.csv for writing" << endl;
      return;
   }
   if(!rfTrainEdges.is_open()) {
      good_=false;
      cerr << "Cannot open workspace/step1_train_edges.csv for writing" << endl;
      return;
   }
   if(!rfTest.is_open()) {
      good_=false;
      cerr << "Cannot open workspace/step1_test.csv for writing" << endl;
      return;
   }
   if(!rfTestEdges.is_open()) {
      good_=false;
      cerr << "Cannot open workspace/step1_test_edges.csv for writing" << endl;
      return;
   }
   if(!rfValidate.is_open()) {
      good_=false;
      cerr << "Cannot open workspace/step1_validate.csv for writing" << endl;
      return;
   }
   //string header1("leaders,followers,L/F,lo,fo,friends,pLO,pFO,pFR,myLsCB,myFsCB,LsFs,LsFsLs,Recip,Recip2,TotRec");
   string header1("LsFs,LsFsLs,Recip");
   string header2("source,dest");
   rfTrain << "RealEdge,";
   rfTrain << header1 << endl;
   rfTrainEdges << header2 << endl;
   rfTest << header1 << endl;
   rfTestEdges << header2 << endl;

   cerr << "   identifying recommendations..." << endl;
   int total=validate_.size();
   int counter=0;
   double tot_map10=0;
   bool recommend_LsFsLs = true;
   bool recommend_FRsFRs = true;
   bool recommend_recip = true;

   counter=0;
   for(map<int,set<int> >::iterator it = validate_.begin(); it != validate_.end(); it++) {
      int id = it->first;
      rfValidate << id;
      for(set<int>::iterator it_val=validate_[id].begin(); it_val != validate_[id].end(); it_val++) {
         rfValidate << "," << *it_val;
      }
      rfValidate << endl;
      counter++;
      if(counter == limit) break;
   }

   counter=0;
   for(map<int,set<int> >::iterator it = validate_.begin(); it != validate_.end(); it++) {
      int id = it->first;
      map<int,EdgeRec> myMissing;
      getMissing(id,myMissing,rfTrain,rfTrainEdges, true /*isTrain*/);
      counter++;
      cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
           << counter << "/" << total;
      if(counter == limit) break;
   }

   counter = 0;
   for(vector<int>::iterator it = test_.begin(); it != test_.end(); it++) {
      int id = *it;
      map<int,EdgeRec> myMissing;
      getMissing(id,myMissing,rfTest,rfTestEdges, false /*isTrain*/);
      counter++;
      cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
           << counter << "/" << total;
      //if(counter == limit) break;
   }

   rfTrain.close();
   rfTrainEdges.close();
   rfTest.close();
   rfTestEdges.close();
   rfValidate.close();
   cerr << endl;
}

xData::~xData() {}
