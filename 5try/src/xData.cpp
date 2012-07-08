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

bool sort_v_v_d(Voter a, Voter b) {
   return a.overlap > b.overlap;
}

const int DEPTH = 3;
const int LIMIT = 20000;
const int LS1 = 2;
const int LS2 = 4;

int xData::recGetDepth (int A, vector<int>& myDepth, int depth) {
   if(depth == DEPTH || myDepth[depth] > LIMIT) return 0;
   myDepth[depth]+=graph_[A].leaders.size() + graph_[A].followers.size();
   for(set<int>::iterator it_next = graph_[A].leaders.begin(); it_next != graph_[A].leaders.end(); it_next++) {
         recGetDepth(*it_next, myDepth, depth+1);
   }
   for(set<int>::iterator it_next = graph_[A].followers.begin(); it_next != graph_[A].followers.end(); it_next++) {
         recGetDepth(*it_next, myDepth, depth+1);
   }
   if(depth == 0) {
      int maxdepth = DEPTH;
      for(int i = 0; i < DEPTH; i++) {
         if (myDepth[i] > LIMIT) {
            maxdepth = i+1;
            break;
         }
      }
      return maxdepth;
   }
   return 0;
}

void xData::getMissing2 (int A, int B, map<int,map<string,pair<double,int> > >& prAfBrecSum, ofstream& attrFile, ofstream& edgeFile, bool isTrain, string type, int depth, int maxdepth) {
   if (depth == maxdepth || (depth > 0 && A==B)) return;
   
   // traverse leader edges
   {
      string newtype = type + "L";
      int ao=0, bo=0, ab=0;
      set<int> recommendations;
      venn(graph_[A].leaders,graph_[B].leaders,ao,bo,ab,NULL,&recommendations,NULL);
      if(recommendations.erase(A)>0) bo--;
      double prAfBrecs = 0;
      if(ab+bo+LS2 > 0) prAfBrecs = (double)(ab+LS1)/(double)(ab+bo+LS2);
      //cerr << A << "," << B << " - " << newtype << "(" << prAfBrecs << "):";
      prAfBrecSum[-1][newtype].first += prAfBrecs;
      prAfBrecSum[-1][newtype].second += 1;
      for(set<int>::iterator it_rec = recommendations.begin(); it_rec != recommendations.end(); it_rec++) {
         //cerr << " " << *it_rec;
         prAfBrecSum[*it_rec][newtype].first += prAfBrecs;
         prAfBrecSum[*it_rec][newtype].second += 1;
      }
      //cerr << endl;
      for(set<int>::iterator it_next = graph_[B].leaders.begin(); it_next != graph_[B].leaders.end(); it_next++) {
         getMissing2(A,*it_next,prAfBrecSum,attrFile,edgeFile,isTrain,newtype,depth+1,maxdepth);
      }
   }
   // traverse follower edges
   {
      string newtype = type + "F";
      int ao=0, bo=0, ab=0;
      set<int> recommendations;
      venn(graph_[A].leaders,graph_[B].followers,ao,bo,ab,NULL,&recommendations,NULL);
      if(recommendations.erase(A)>0) bo--;
      double prAfBrecs = 0;
      if(ab+bo+LS2 > 0) prAfBrecs = (double)(ab+LS1)/(double)(ab+bo+LS2);
      //cerr << A << "," << B << " - " << newtype << "(" << prAfBrecs << "):";
      prAfBrecSum[-1][newtype].first += prAfBrecs;
      prAfBrecSum[-1][newtype].second += 1;
      for(set<int>::iterator it_rec = recommendations.begin(); it_rec != recommendations.end(); it_rec++) {
         //cerr << " " << *it_rec;
         //prAfBrecSum[*it_rec][newtype].first += prAfBrecs/(double)recommendations.size();
         prAfBrecSum[*it_rec][newtype].first += prAfBrecs;
         prAfBrecSum[*it_rec][newtype].second += 1;
      }
      //cerr << endl;
      for(set<int>::iterator it_next = graph_[B].followers.begin(); it_next != graph_[B].followers.end(); it_next++) {
         getMissing2(A,*it_next,prAfBrecSum,attrFile,edgeFile,isTrain,newtype,depth+1,maxdepth);
      }
   }
   if(depth==0) {
      //cerr << "--" << A << "--" << endl;
      vector<pair<int,double> > tmp;
      for(map<int,map<string,pair<double,int> > >::iterator it_rec = prAfBrecSum.begin(); it_rec != prAfBrecSum.end(); it_rec++) {
         if(it_rec->first == -1) continue;
         double prAvgSum = 0;
         int rec = it_rec->first;
         for(map<string,pair<double,int> >::iterator it_type = (it_rec->second).begin(); it_type != (it_rec->second).end(); it_type++) {
            string type = it_type->first;
            //prAvgSum += (it_type->second).first / ((double)(it_type->second).second * pow(2,(double)type.size()));
            prAvgSum += (it_type->second).first / ((double)prAfBrecSum[-1][type].second * pow(2,(double)type.size()));
            //prAvgSum += (it_type->second).first / (double)prAfBrecSum[-1][type].second;
         }
         tmp.push_back(pair<int,double>(rec,prAvgSum));
      }
      sort(tmp.begin(),tmp.end(),sort_v_p_i_d_d);
      int counter=0;
      for(vector<pair<int,double> >::iterator it_tmp = tmp.begin(); it_tmp != tmp.end() && counter < 50; it_tmp++) {
         counter++;
         if(isTrain) {
            if(validate_[A].find((*it_tmp).first) != validate_[A].end()) attrFile << "1,";
            else attrFile << "0,";
         }
         //attrFile << (*it_tmp).second << ",";
         int types_count = 0;
         for(set<string>::iterator it_type = types_.begin(); it_type != types_.end(); it_type++) {
            types_count++;
            if(prAfBrecSum[(*it_tmp).first].find(*it_type) == prAfBrecSum[(*it_tmp).first].end()) {
               if(*it_type == "F") attrFile << "0,";
               else if(*it_type == "FF") attrFile << "0,";
               else if(*it_type == "LF") attrFile << "0,";
               else if(*it_type == "FL") attrFile << "0,";
               else if(*it_type == "LL") attrFile << "0,";
               attrFile << "0";
            } else {
               if(*it_type == "F") attrFile << graph_[(*it_tmp).first].myLsConnectBack_rate << ","; // << prAfBrecSum[(*it_tmp).first][*it_type].first;
               else if(*it_type == "FF") attrFile << graph_[(*it_tmp).first].myLsLsConnectBack_rate << ","; // << prAfBrecSum[(*it_tmp).first][*it_type].first;
               else if(*it_type == "LF") attrFile << graph_[(*it_tmp).first].myLsFsConnectBack_rate << ","; // << prAfBrecSum[(*it_tmp).first][*it_type].first;
               else if(*it_type == "FL") attrFile << graph_[(*it_tmp).first].myFsLsConnectBack_rate << ","; // << prAfBrecSum[(*it_tmp).first][*it_type].first;
               else if(*it_type == "LL") attrFile << graph_[(*it_tmp).first].myFsFsConnectBack_rate << ","; // << prAfBrecSum[(*it_tmp).first][*it_type].first;
               attrFile << prAfBrecSum[(*it_tmp).first][*it_type].first / (double)prAfBrecSum[-1][*it_type].second;
            }
            if(types_count < types_.size()) attrFile << ",";
         }
         attrFile << endl;
         edgeFile << A << "," << (*it_tmp).first << endl;
      }
      attrFile << flush;
      edgeFile << flush;
   }
}

void xData::getMissing (int id, map<int,EdgeRec>& myMissing, ofstream& attrFile, ofstream& edgeFile, bool isTrain) {
   int voter_limit = 10;
   int topN=9;
   double ls1_=0, ls2_=0;
   int ao_cnt=0, bo_cnt=0, ab_cnt=0;
   set<int> followers_only;
   venn(graph_[id].leaders, graph_[id].followers, ao_cnt, bo_cnt, ab_cnt, NULL, &(followers_only), NULL);
   if(recommend_Fs_) {
      for(set<int>::iterator it_fo = followers_only.begin(); it_fo != followers_only.end(); it_fo++) {
         myMissing[*it_fo].Fs=graph_[*it_fo].myLsConnectBack_rate;
      }
   }
   if(recommend_FsLs_) {
      map<int,double> myFsLs;
      int recommenders = 0;
      for(set<int>::iterator it_followers = graph_[id].followers.begin(); it_followers != graph_[id].followers.end(); it_followers++) {
         int follower = *it_followers;
         int ao_cnt=0, bo_cnt=0, ab_cnt=0;
         set<int> recommendations;
         venn(graph_[id].leaders,graph_[follower].leaders,ao_cnt,bo_cnt,ab_cnt,NULL,&recommendations,NULL);
         recommendations.erase(id);  // need to remove myself from recommendations
         if( bo_cnt > 0) {
            for(set<int>::iterator it_FsL = recommendations.begin(); it_FsL != recommendations.end(); it_FsL++) {
               int Fs_leader = *it_FsL;
               //myFsLs[Fs_leader] += 1.0 / (double)(recommendations.size());
               myFsLs[Fs_leader] += 1.0;
            }
            recommenders++;
         }
      }
      if(recommenders > 0) {
         vector<pair<int,double> > tmp;
         for(map<int,double>::iterator it_tmp=myFsLs.begin(); it_tmp!=myFsLs.end(); it_tmp++) {
            tmp.push_back(pair<int,double>(it_tmp->first,it_tmp->second));
         }
         sort(tmp.begin(),tmp.end(),sort_v_p_i_d_d);
         for(int i=0; i < tmp.size() && i < topN; i++) {
            myMissing[tmp[i].first].FsLs = (tmp[i].second+ls1_)/((double)recommenders+ls2_);
         }
      }
   }
   if(recommend_FsFs_) {
      map<int,double> myFsFs;
      int recommenders = 0;
      for(set<int>::iterator it_followers = graph_[id].followers.begin(); it_followers != graph_[id].followers.end(); it_followers++) {
         int follower = *it_followers;
         int ao_cnt=0, bo_cnt=0, ab_cnt=0;
         set<int> recommendations;
         venn(graph_[id].leaders,graph_[follower].followers,ao_cnt,bo_cnt,ab_cnt,NULL,&recommendations,NULL);
         recommendations.erase(id);  // need to remove myself from recommendations
         if( bo_cnt > 0) {
            for(set<int>::iterator it_FsF = recommendations.begin(); it_FsF != recommendations.end(); it_FsF++) {
               int Fs_follower = *it_FsF;
               //myFsFs[Fs_follower] += 1.0 / (double)(recommendations.size());
               myFsFs[Fs_follower] += 1.0;
            }
            recommenders++;
         }
      }
      if(recommenders > 0) {
         vector<pair<int,double> > tmp;
         for(map<int,double>::iterator it_tmp=myFsFs.begin(); it_tmp!=myFsFs.end(); it_tmp++) {
            tmp.push_back(pair<int,double>(it_tmp->first,it_tmp->second));
         }
         sort(tmp.begin(),tmp.end(),sort_v_p_i_d_d);
         for(int i=0; i < tmp.size() && i < topN; i++) {
            myMissing[tmp[i].first].FsFs = (tmp[i].second+ls1_)/((double)recommenders+ls2_);
         }
      }
   }
   if(recommend_LsLs_) {
      map<int,double> myLsLs;
      int recommenders = 0;
      for(set<int>::iterator it_leaders = graph_[id].leaders.begin(); it_leaders != graph_[id].leaders.end(); it_leaders++) {
         int leader = *it_leaders;
         int ao_cnt=0, bo_cnt=0, ab_cnt=0;
         set<int> recommendations;
         venn(graph_[id].leaders,graph_[leader].leaders,ao_cnt,bo_cnt,ab_cnt,NULL,&recommendations,NULL);
         recommendations.erase(id);  // need to remove myself from recommendations
         if( bo_cnt > 0) {
            for(set<int>::iterator it_LsL = recommendations.begin(); it_LsL != recommendations.end(); it_LsL++) {
               int Ls_leader = *it_LsL;
               //myLsLs[Ls_leader] += 1.0 / (double)(recommendations.size());
               myLsLs[Ls_leader] += 1.0;
            }
            recommenders++;
         }
      }
      if(recommenders > 0) {
         vector<pair<int,double> > tmp;
         for(map<int,double>::iterator it_tmp=myLsLs.begin(); it_tmp!=myLsLs.end(); it_tmp++) {
            tmp.push_back(pair<int,double>(it_tmp->first,it_tmp->second));
         }
         sort(tmp.begin(),tmp.end(),sort_v_p_i_d_d);
         for(int i=0; i < tmp.size() && i < topN; i++) {
            myMissing[tmp[i].first].LsLs = (tmp[i].second+ls1_)/((double)recommenders+ls2_);
         }
      }
   }
   if(recommend_LsFs_) {
      map<int,double> myLsFs;
      int recommenders = 0;
      for(set<int>::iterator it_leaders = graph_[id].leaders.begin(); it_leaders != graph_[id].leaders.end(); it_leaders++) {
         int leader = *it_leaders;
         int ao_cnt=0, bo_cnt=0, ab_cnt=0;
         set<int> recommendations;
         venn(graph_[id].leaders,graph_[leader].followers,ao_cnt,bo_cnt,ab_cnt,NULL,&recommendations,NULL);
         recommendations.erase(id);  // need to remove myself from recommendations
         if( bo_cnt > 0) {
            for(set<int>::iterator it_LsF = recommendations.begin(); it_LsF != recommendations.end(); it_LsF++) {
               int Ls_follower = *it_LsF;
               //myLsFs[Ls_follower] += 1.0 / (double)(recommendations.size());
               myLsFs[Ls_follower] += 1.0;
            }
            recommenders++;
         }
      }
      if(recommenders > 0) {
         vector<pair<int,double> > tmp;
         for(map<int,double>::iterator it_tmp=myLsFs.begin(); it_tmp!=myLsFs.end(); it_tmp++) {
            tmp.push_back(pair<int,double>(it_tmp->first,it_tmp->second));
         }
         sort(tmp.begin(),tmp.end(),sort_v_p_i_d_d);
         for(int i=0; i < tmp.size() && i < topN; i++) {
            myMissing[tmp[i].first].LsFs = (tmp[i].second+ls1_)/((double)recommenders+ls2_);
         }
      }
   }
   if(recommend_LsFsLs_) {
      set<int> myLsFs; // my leaders followers
      //cerr << "find my leaders followers" << endl;
      for(set<int>::iterator it_leaders = graph_[id].leaders.begin(); it_leaders != graph_[id].leaders.end(); it_leaders++) {
         int leader = *it_leaders;
         myLsFs.insert(graph_[leader].followers.begin(),graph_[leader].followers.end());
      }
      myLsFs.erase(id); // need to remove myself from my leaders followers
      vector<Voter> voters;
      //cerr << "calculate distance and find recs" << endl;
      for(set<int>::iterator it_myLsFs = myLsFs.begin(); it_myLsFs != myLsFs.end(); it_myLsFs++) {
         Voter v;
         int ao_cnt=0, bo_cnt=0, ab_cnt=0;
         venn(graph_[id].leaders, graph_[*it_myLsFs].leaders, ao_cnt, bo_cnt, ab_cnt, NULL, &(v.recommendations) /*b*/, NULL);
         v.recommendations.erase(id);  // need to remove myself
         bo_cnt--;
         v.id=*it_myLsFs;
         v.overlap=(double)ab_cnt/(double)(ao_cnt+bo_cnt+ab_cnt);
         voters.push_back(v);
      }
      sort(voters.begin(),voters.end(),sort_v_v_d);
      int v_count=0;
      int vR_count=0;
      for(vector<Voter>::iterator it_voter=voters.begin(); it_voter != voters.end(); it_voter++) {
         if(v_count < voter_limit) {
            if(graph_[id].leaders.find(it_voter->id) == graph_[id].leaders.end()) { // check to see if I am already following this person
               //myMissing[it_voter->id].LsFs+=it_voter->overlap;
               v_count++;
            }
         }
         if(vR_count < voter_limit) {
            if(it_voter->recommendations.size() == 0) continue; // need to allow voters with no recommendations for LsFs
            for(set<int>::iterator it_r=it_voter->recommendations.begin(); it_r!=it_voter->recommendations.end(); it_r++) {
               myMissing[*it_r].LsFsLs += it_voter->overlap / ((double)int_min(voter_limit,voters.size())*(double)it_voter->recommendations.size());
               //myMissing[*it_r].LsFsLs += it_voter->overlap / ((double)int_min(voter_limit,voters.size()));
               //myMissing[*it_r].LsFsLs += 1.0 / ((double)int_min(voter_limit,voters.size()));
            }
            vR_count++;
         }
         if(v_count == voter_limit && vR_count == voter_limit) break;
      }
   }
   for(map<int,EdgeRec>::iterator it_missing = myMissing.begin(); it_missing != myMissing.end(); it_missing++) {
      if(isTrain) {
         if(validate_[id].find(it_missing->first) != validate_[id].end()) attrFile << "1,";
         else attrFile << "0,";
      }
      // !!! DON'T FORGET TO CHANGE THE HEADER STRING header1 if you modify the attributes !!!
      attrFile << it_missing->second.Fs << "," \
               << it_missing->second.FsLs << "," \
               << it_missing->second.FsFs << "," \
               << it_missing->second.LsLs << "," \
               << it_missing->second.LsFs << "," \
               << it_missing->second.LsFsLs << endl << flush;
      edgeFile << id << "," << it_missing->first << endl << flush;
   }

}

void recSetType(set<string>& types, string type, int depth) {
   if(depth == DEPTH) return;
   string newtype = type + "L";
   types.insert(newtype);
   recSetType(types, newtype, depth+1);
   newtype = type + "F";
   types.insert(newtype);
   recSetType(types, newtype, depth+1);
}

xData::xData(char* trainFile, char* testFile, int seed, int limit_train, int limit_test) {
   recommend_Fs_ = true;
   recommend_FsLs_ = true;
   recommend_FsFs_ = true;
   recommend_LsLs_ = true;
   recommend_LsFs_ = true;
   recommend_LsFsLs_ = true;
   good_ = true;
   recSetType(types_,"",0);
   types_.erase("L");
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

   int leave_out_cnt = 262000;
   if(limit_test <= 0) leave_out_cnt = 0;

   {
      vector<pair<int,int> > all_edges;
      cerr << "reading trainFile: " << trainFile << " ..." << flush;
      int row = 0;
      bool header_row = true;  //header_row is first row in csv
      graph_.assign(last_id+1,Node());
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
               all_edges.push_back(pair<int,int>(source,dest));
            }
            col++;
         }
      }
      fTrain.close();
      cerr << "done" << endl;
#if(0)
      while(validate_.size() < leave_out_cnt) {
         int edge = rand() % all_edges.size();
         int source = all_edges[edge].first;
         int dest = all_edges[edge].second;
         if(graph_[source].leaders.size()+graph_[source].followers.size() > 1 \
              && graph_[dest].leaders.size()+graph_[dest].followers.size() > 1
              && validate_[source].size() < 10) {
            validate_[source].insert(dest);
            graph_[source].leaders.erase(dest);
            graph_[dest].followers.erase(source);
         }
      }
#endif
   }

   int vcnt=0;
   set<int>::iterator it_dest;
   while (validate_.size() < leave_out_cnt) {
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

   int tn = 0, tnnl = 0, tnnf = 0, tnnlf=0;
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
      vector<pair<int,int> > globalLeaders_(last_id+1,pair<int,int>(0,0));
      //vector<pair<int,int> > globalFollowers_(last_id+1,pair<int,int>(0,0));

      for(int id = 1; id < graph_.size(); id++) {
         //globalFollowers_[id].first = id;
         //globalFollowers_[id].second = graph_[id].leaders.size();
         globalLeaders_[id].first = id;
         globalLeaders_[id].second = graph_[id].followers.size();
      }
      //sort(globalFollowers_.begin(), globalFollowers_.end(), sort_v_p_i_i_d);
      sort(globalLeaders_.begin(), globalLeaders_.end(), sort_v_p_i_i_d);
      //cerr << "Top followers..." << endl;
      //for(int n=0; n<10; n++) {
      //   cerr << "   id: " << globalFollowers_[n].first << "; followingCnt: " << globalFollowers_[n].second << endl;
      //}
      cerr << "Top leaders..." << endl;
      for(int n=0; n<10; n++) {
         cerr << "   id: " << globalLeaders_[n].first << "; leadingCnt: " << globalLeaders_[n].second << endl;
      }
   }
   cerr << "Predicting missing edges..." << endl;
   if(1) {
   cerr << "   determining friends and connect back rates..." << endl;
   for(int i=0; i<graph_.size(); i++) {
      if((graph_[i].leaders.size() == 0 || graph_[i].followers.size() == 0) && 0) {
         graph_[i].myLsConnectBack_rate = 0.5;
         graph_[i].myFsConnectBack_rate = 0.5;
         graph_[i].myLsLsConnectBack_rate = 0.5;
         graph_[i].myLsFsConnectBack_rate = 0.5;
         graph_[i].myFsLsConnectBack_rate = 0.5;
         graph_[i].myFsFsConnectBack_rate = 0.5;
      } else {
         int ls1 = 1, ls2 = 2;
         {
            int ao_cnt=0, bo_cnt=0, ab_cnt=0;
            venn(graph_[i].leaders, graph_[i].followers, ao_cnt, bo_cnt, ab_cnt, NULL, NULL, NULL);
            graph_[i].pLO = (double)(ao_cnt+1)/(double)(ao_cnt+bo_cnt+ab_cnt+2);
            graph_[i].pFO = (double)(bo_cnt+1)/(double)(ao_cnt+bo_cnt+ab_cnt+2);
            graph_[i].pFR = (double)(ab_cnt+1)/(double)(ao_cnt+bo_cnt+ab_cnt+2);
            if(ao_cnt+ab_cnt+ls2 > 0) graph_[i].myLsConnectBack_rate = (double)(ab_cnt+ls1)/(double)(ao_cnt+ab_cnt+ls2);
            if(bo_cnt+ab_cnt+ls2 > 0) graph_[i].myFsConnectBack_rate = (double)(ab_cnt+ls1)/(double)(bo_cnt+ab_cnt+ls2);
         }
         {
            set<int> myLsLs;
            set<int> myLsFs;
            for(set<int>::iterator it_l = graph_[i].leaders.begin(); it_l != graph_[i].leaders.end(); it_l++) {
               myLsLs.insert(graph_[*it_l].leaders.begin(),graph_[*it_l].leaders.end());
               myLsFs.insert(graph_[*it_l].followers.begin(),graph_[*it_l].followers.end());
            }
            int ao_cnt=0, bo_cnt=0, ab_cnt=0;
            venn(myLsLs, graph_[i].followers, ao_cnt, bo_cnt, ab_cnt, NULL, NULL, NULL);
            if(ao_cnt+ab_cnt+ls2 > 0) graph_[i].myLsLsConnectBack_rate = (double)(ab_cnt+ls1)/(double)(ao_cnt+ab_cnt+ls2);

            ao_cnt=0, bo_cnt=0, ab_cnt=0;
            venn(myLsFs, graph_[i].followers, ao_cnt, bo_cnt, ab_cnt, NULL, NULL, NULL);
            if(ao_cnt+ab_cnt+ls2 > 0) graph_[i].myLsFsConnectBack_rate = (double)(ab_cnt+ls1)/(double)(ao_cnt+ab_cnt+ls2);
         }
         {
            set<int> myFsLs;
            set<int> myFsFs;
            for(set<int>::iterator it_l = graph_[i].followers.begin(); it_l != graph_[i].followers.end(); it_l++) {
               myFsLs.insert(graph_[*it_l].leaders.begin(),graph_[*it_l].leaders.end());
               myFsFs.insert(graph_[*it_l].followers.begin(),graph_[*it_l].followers.end());
            }
            int ao_cnt=0, bo_cnt=0, ab_cnt=0;
            venn(myFsLs, graph_[i].followers, ao_cnt, bo_cnt, ab_cnt, NULL, NULL, NULL);
            if(ao_cnt+ab_cnt+ls2 > 0) graph_[i].myFsLsConnectBack_rate = (double)(ab_cnt+ls1)/(double)(ao_cnt+ab_cnt+ls2);

            ao_cnt=0, bo_cnt=0, ab_cnt=0;
            venn(myFsFs, graph_[i].followers, ao_cnt, bo_cnt, ab_cnt, NULL, NULL, NULL);
            if(ao_cnt+ab_cnt+ls2 > 0) graph_[i].myFsFsConnectBack_rate = (double)(ab_cnt+ls1)/(double)(ao_cnt+ab_cnt+ls2);
         }
      }
      //cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" \
           << i+1 << "/" << graph_.size() << "; fbr: " << graph_[i].myLsConnectBack_rate << endl;
   }
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
   //string header1="Fs,FsLs,FsFs,LsLs,LsFs,LsFsLs";
   string header1;
   int types_count = 0;
   for(set<string>::iterator it_h1 = types_.begin(); it_h1 != types_.end(); it_h1++) {
      types_count++;
      if(*it_h1 == "F") header1 += "Fcb," + *it_h1;
      else if(*it_h1 == "FF") header1 += "FFcb," + *it_h1;
      else if(*it_h1 == "LF") header1 += "LFcb," + *it_h1;
      else if(*it_h1 == "FL") header1 += "FLcb," + *it_h1;
      else if(*it_h1 == "LL") header1 += "LLcb," + *it_h1;
      else header1 += *it_h1;
      if(types_count != types_.size()) header1 += ",";
   }
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

   counter=0;
   int counter2 = 0;
   for(map<int,set<int> >::iterator it = validate_.begin(); it != validate_.end(); it++) {
      if( (double)(rand() % 1000000)/1000000 > (double)limit_train/(double)leave_out_cnt) {
         counter2++;
         continue;
      }
      int id = it->first;

      rfValidate << id;
      for(set<int>::iterator it_val=validate_[id].begin(); it_val != validate_[id].end(); it_val++) {
         rfValidate << "," << *it_val;
      }
      rfValidate << endl;
      
      map<int,EdgeRec> myMissing;
      //getMissing(id,myMissing,rfTrain,rfTrainEdges, true /*isTrain*/);
      map<int,map<string,pair<double,int> > > prAfBrecSum;
      vector<int> myDepth(DEPTH,0);
      int maxdepth = recGetDepth(id,myDepth,0);
      cerr << id << " (" << maxdepth << ") : " << myDepth[0] << "," << myDepth[1] << "," << myDepth[2] << "," << myDepth[3] << "," << myDepth[4] << endl;
      getMissing2(id,id,prAfBrecSum,rfTrain,rfTrainEdges, true /*isTrain*/,"",0,maxdepth);
      counter++;
      cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
           << counter << "/" << counter2 << "/" << limit_train;
      cerr << endl;
      //if(counter == limit_train) break;
   }
   cerr << endl;
   
   counter = 0;
   for(vector<int>::iterator it = test_.begin(); it != test_.end(); it++) {
      int id = *it;
      map<int,EdgeRec> myMissing;
      //getMissing(id,myMissing,rfTest,rfTestEdges, false /*isTrain*/);
      map<int,map<string,pair<double,int> > > prAfBrecSum;
      vector<int> myDepth(DEPTH,0);
      int maxdepth = recGetDepth(id,myDepth,0);
      cerr << id << " (" << maxdepth << ") : " << myDepth[0] << "," << myDepth[1] << "," << myDepth[2] << "," << myDepth[3] << "," << myDepth[4] << endl;
      getMissing2(id,id,prAfBrecSum,rfTest,rfTestEdges, false /*isTrain*/,"",0,maxdepth);
      counter++;
      cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
           << counter << "/" << limit_test;
      cerr << endl;
      if(counter == limit_test) break;
   }
   cerr << endl;

   rfTrain.close();
   rfTrainEdges.close();
   rfTest.close();
   rfTestEdges.close();
   rfValidate.close();
   cerr << endl;
}

xData::~xData() {}
