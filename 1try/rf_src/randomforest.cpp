#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cmath>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include "xData.h"
#include "randomforest.h"

bool debug = true;

bool idx_double_sort(pair<int,double> a, pair<int,double> b) {
   return a.second < b.second;
}

bool f_ig_cut_sort(f_ig_cut a, f_ig_cut b) {
   return a.ig > b.ig;
}

/*
double Entropy (int aCnt, int bCnt) {
   if(aCnt == 0 || bCnt == 0) return 0.0;
   double fA = (double)aCnt/(double)(aCnt+bCnt);
   double fB = (double)bCnt/(double)(aCnt+bCnt);
   return -(fA*log(fA)+fB*log(fB));
}
*/

double Entropy (int aCnt, int bCnt) { // actually gini impurity
   return 1 - pow(((double)aCnt/(double)(aCnt+bCnt)),2) - pow(((double)bCnt/(double)(aCnt+bCnt)),2);
}

double infoGain(vector<vector<int> >& al) {
   int total_l0=0, total_l1=0;
   vector<double> Es(al.size(),0);
   for(int a = 0; a < al.size(); a++) {
      total_l0 += al[a][0];
      total_l1 += al[a][1];
      Es[a] = Entropy(al[a][0],al[a][1]);
   }
   int total = total_l0 + total_l1;
   double E = Entropy(total_l0,total_l1);
   double IG = E;
   for(int a = 0; a < al.size(); a++) {
      IG -= (Es[a] * (double)(al[a][0]+al[a][1])/(double)total);
   }
   if(IG < 1e-5 || IG != IG) IG = 0;
   //cerr << IG << "," << E << "," << Es[0] << "," << Es[1] << endl;
   return IG;
}


void create_bag(vector<pair<int,int> >& bag, vector<pair<int,int> >& oob, vector<int>& orig_idx, vector<int>& w) {
   map<int,int> bagMap;
   int sample_cnt = orig_idx.size();
   for(int sample=0; sample < sample_cnt; sample++) {
      int idx = rand() % sample_cnt;
      int id = orig_idx[idx];
      bagMap[id]+=w[idx];  //with replacement
      //bagMap[id]=1;  //without replacement
   }
   for(map<int,int>::iterator it = bagMap.begin(); it != bagMap.end(); it++) {
      bag.push_back(pair<int,int>(it->first,it->second));
   }

   for(vector<int>::iterator it = orig_idx.begin(); it != orig_idx.end(); it++) {
      if(bagMap.find(*it) == bagMap.end()) {
         oob.push_back(pair<int,int>(*it,1));
      }
   }
   /*
   int last=-1;
   for(int i = 0; i < bag.size(); i++) {
      if(bag[i].first > last+1) {
         for(int oob_id=last+1; oob_id < bag[i].first; oob_id++) {
            oob.push_back(pair<int,int>(oob_id,1));
         }
      }
      last = bag[i].first;
   }
   if(bag[bag.size()-1].first < sample_cnt-1) {
      for(int oob_id = bag[bag.size()-1].first+1; oob_id < sample_cnt; oob_id++) {
         oob.push_back(pair<int,int>(oob_id,1));
      }
   }
   */
   sort(oob.begin(),oob.end());
}

void create_feature_list(vector<int>& features, int fSamp, vector<int>& good_features) {
   set<int> samples;
   while(samples.size() < fSamp) {
      samples.insert(good_features[rand() % good_features.size()]);
   }
   for(set<int>::iterator it = samples.begin(); it != samples.end(); it++) {
      features.push_back(*it);
      //cerr << "," << *it;
   }
   //cerr << endl;
}

RandomForest::RandomForest(xData* pXD, int fold, int folds, int Mtry, int trees, bool subset_on) : pXD_(pXD), fold_(fold), folds_(folds), 
                                                                                   base_feature_sample_cnt_(Mtry), trees_(trees), 
                                                                                   good_(true) {
   ls1_ = 1;
   ls2_ = 2;
   if(subset_on) {
      int myints[] = {12,13,14};

      good_features_.assign(myints, myints+sizeof(myints)/sizeof(int) );
      
      for(int f_idx = 0; f_idx < good_features_.size(); f_idx++) {
         good_features_[f_idx]--;
         if(good_features_[f_idx] >= pXD->attr_class.size()) {
            good_ = false;
            cerr << "Good features specified are out of bounds" << endl;
            return;
         }
      }
   } else {
      for(int f = 0; f < pXD_->trn_cols; f++) {
         good_features_.push_back(f);
      }
   }
      
   cerr << "good_features_cnt: " << good_features_.size() << endl;
   cerr << "learning..." << endl;
   progressive_factor_ = 1;
   if(good_) build();
}

void RandomForest::recAssignNode(Node* pNode, vector<pair<int,int> >& idx_orig, string pre, int depth, int& node_cnt, double& tree_gini) {
   double best_ig=-1;
   int best_attr=-1;
   bool rand_best_f = false;
   double best_cutVal=0.5;
   vector<vector<int> > best_a_l_cnt(2,vector<int>(2,0));
   vector<pair<int,int> > idx0, idx1;
   idx0.empty();
   idx1.empty();
   pNode->node0 = NULL;
   pNode->node1 = NULL;

   vector<int> features;
   int feature_sample_cnt = (int)((double)base_feature_sample_cnt_ * pow(progressive_factor_,depth));
   if(feature_sample_cnt > good_features_.size()) feature_sample_cnt = good_features_.size();
   create_feature_list(features, feature_sample_cnt, good_features_);
   if(debug && 0) {
      int last=-1;
      for(int i=0; i<features.size(); i++) {
         if(features[i] == last) cerr << endl << "ERROR: repeat feature" << endl;
         last = features[i];
         cerr << features[i] << ",";
      }
      cerr << endl;
   }
   vector<vector<int> > a_l_cnt(2,vector<int>(2,0));
   double cutVal;
   double ig=-1;
   int approach = 1;
   vector<f_ig_cut> vBestF;
   for(int attr_idx = 0; attr_idx < feature_sample_cnt; attr_idx++) {
      //double cutVal = (double)((rand() % 1000)+1)/(double)(1001);
      //cerr << features[attr_idx] << ":" << pXD_->attr_class[features[attr_idx]] << ":" << pXD_->attr_class.size() << endl;
      if(pXD_->attr_class[features[attr_idx]] == 1 and approach == 1) {
         double best_ig = -1;
         vector<pair<int,double> > attr_sort_idx(idx_orig.size(),pair<int,double>(0,0.0));
         a_l_cnt.assign(2,vector<int>(2,0));
         for(int exi = 0; exi < idx_orig.size(); exi++) {
            attr_sort_idx[exi] = pair<int,double>(exi,pXD_->trn_attr[idx_orig[exi].first][features[attr_idx]]);
            a_l_cnt[1][(int)pXD_->trn_labl[idx_orig[exi].first]] += idx_orig[exi].second;
         }
         sort(attr_sort_idx.begin(),attr_sort_idx.end(),idx_double_sort);
         int ex_id, ex_id_next, ex_cnt, labl;
         double attr_val;
         for(int exii = 0; exii < (attr_sort_idx.size()-1); exii++) {
            ex_id = idx_orig[attr_sort_idx[exii].first].first;
            ex_id_next = idx_orig[attr_sort_idx[exii+1].first].first;
            ex_cnt = idx_orig[attr_sort_idx[exii].first].second;
            labl = (int)pXD_->trn_labl[ex_id];
            attr_val = pXD_->trn_attr[ex_id][features[attr_idx]];
            a_l_cnt[0][labl] += ex_cnt;
            a_l_cnt[1][labl] -= ex_cnt;
            if (pXD_->trn_attr[ex_id_next][features[attr_idx]] == attr_val) continue;
            ig = infoGain(a_l_cnt);
            if(ig>best_ig) {
               best_ig = ig;
               cutVal = attr_val;  //be sure to cut at <= cutVal or > cutVal
            }
         }
         ig = best_ig;
      } else {
         a_l_cnt.assign(2,vector<int>(2,0));
         if(approach == 1 || approach == 2) {
            cutVal = pXD_->attr_avg[features[attr_idx]];
         } else {
            cutVal = (double)((rand() % 1000)+1)/(double)(1001);
         }
         int attr_split;
         for(int exi = 0; exi < idx_orig.size(); exi++) {
            attr_split = (pXD_->trn_attr[idx_orig[exi].first][features[attr_idx]] > cutVal) ? 1 : 0;
            a_l_cnt[attr_split][(int)pXD_->trn_labl[idx_orig[exi].first]] += idx_orig[exi].second;
         }
         ig = infoGain(a_l_cnt);
      }

      if(rand_best_f) {
         f_ig_cut newcut;
         newcut.ig = ig;
         newcut.attr = features[attr_idx];
         newcut.cutVal = cutVal;
         newcut.a_l_cnt = a_l_cnt;
         vBestF.push_back(newcut);
      }

      if(ig > best_ig) {
         best_ig = ig;
         best_attr = features[attr_idx];
         best_cutVal = cutVal;
         best_a_l_cnt = a_l_cnt;
      }
      //cerr << features[attr_idx] << "," << pXD_->attr_class[features[attr_idx]] << "," << ig << endl;
   }
   if(rand_best_f) {
      sort(vBestF.begin(), vBestF.end(), f_ig_cut_sort);
      int id = (int)(pow((double)(rand() % 1000)/1000.0,11) * 3 /*vBestF.size()*/); 
      //cerr << id << endl;
      //cerr << vBestF[0].ig << "," << best_ig << endl;
      best_ig = vBestF[id].ig;
      best_attr = vBestF[id].attr;
      best_cutVal = vBestF[id].cutVal;
      best_a_l_cnt = vBestF[id].a_l_cnt;
   }

   pNode->ig = best_ig;
   pNode->attr_id = best_attr;
   pNode->cutVal = best_cutVal;
   pNode->a_l_cnt = best_a_l_cnt;

   int cnt_a1=0, cnt_a0=0, onesCnt_a1=0, onesCnt_a0=0;
   for(int exi = 0; exi < idx_orig.size(); exi++) {
      if(pXD_->trn_attr[idx_orig[exi].first][best_attr] > best_cutVal) {
         idx1.push_back(idx_orig[exi]);
         onesCnt_a1 += (pXD_->trn_labl[idx_orig[exi].first] > 0.5) ? idx_orig[exi].second : 0;
         cnt_a1 += idx_orig[exi].second;
      } else {
         idx0.push_back(idx_orig[exi]);
         onesCnt_a0 += (pXD_->trn_labl[idx_orig[exi].first] > 0.5) ? idx_orig[exi].second : 0;
         cnt_a0 += idx_orig[exi].second;
      }
   }
   //double ls1=1, ls2=2; // laplace smoothing to avoid infinite values
   double ls1=0, ls2=0; // laplace smoothing to avoid infinite values
   if(cnt_a0 > 0) pNode->class_a0 = ((double)onesCnt_a0 + ls1)/((double)cnt_a0 + ls2);
   else pNode->class_a0 = 0.5;
   if(cnt_a1 > 0) pNode->class_a1 = ((double)onesCnt_a1 + ls1)/((double)cnt_a1 + ls2);
   else pNode->class_a1 = 0.5;
   pNode->exCnt = cnt_a0 + cnt_a1;
   /*
   cerr << pre << idx_orig.size() << "," << best_attr << "(" << pXD_->attr_class[best_attr] << ")," << best_ig << "," 
        << idx0.size() << "," << pNode->class_a0 << "," 
        << idx1.size() << "," << pNode->class_a1 << endl;
   */
   {
   int ls1=0, ls2=0;
   if(best_ig > 0 or (feature_sample_cnt < good_features_.size() and progressive_factor_ > 1)) {
      if(idx0.size() > 1 and onesCnt_a0 != 0 and onesCnt_a0 != cnt_a0) {
         pNode->node0 = new Node;
         recAssignNode(pNode->node0, idx0, pre + "  ", depth+1, node_cnt, tree_gini);
      } else {
         if(cnt_a0 > 0) {
            node_cnt++;
            tree_gini += (double)(cnt_a0*2*(cnt_a0-onesCnt_a0+ls1)*(onesCnt_a0+ls1)) / (double)((cnt_a0 + ls2)*(cnt_a0 + ls2));
         }
      }
      if(idx1.size() > 1 and onesCnt_a1 != 0 and onesCnt_a1 != cnt_a1) {
         pNode->node1 = new Node;
         recAssignNode(pNode->node1, idx1, pre + "  ", depth+1, node_cnt, tree_gini);
      } else {
         if(cnt_a1 > 0) {
            node_cnt++;
            tree_gini += (double)(cnt_a1*2*(cnt_a1-onesCnt_a1+ls1)*(onesCnt_a1+ls1)) / (double)((cnt_a1 + ls2)*(cnt_a1+ls2));
         }
      }
   } else {
      if(cnt_a0 > 0) {
         node_cnt += 1;
         tree_gini += (double)(cnt_a0*2*(cnt_a0-onesCnt_a0+ls1)*(onesCnt_a0+ls1)) / (double)((cnt_a0 + ls2)*(cnt_a0+ls2));
      }
      if(cnt_a1 > 0) {
         node_cnt += 1;
         tree_gini += (double)(cnt_a1*2*(cnt_a1-onesCnt_a1+ls1)*(onesCnt_a1+ls1)) / (double)((cnt_a1 + ls2)*(cnt_a1+ls2));
      }
   }
   }
}

void RandomForest::recPredict(Node* pNode, vector<pair<int,int> >& idx_orig, vector<vector<double> >* data, vector<double>& yih, int min_cnt, double min_ig) {
   vector<pair<int,int> > idx0, idx1;
   for(int i = 0; i < idx_orig.size(); i++) {
      if(data->at(idx_orig[i].first)[pNode->attr_id] > pNode->cutVal) {
         idx1.push_back(idx_orig[i]);
         yih[idx_orig[i].first] = pNode->class_a1;
      } else {
         idx0.push_back(idx_orig[i]);
         yih[idx_orig[i].first] = pNode->class_a0;
      }
   }
   if(pNode->exCnt > min_cnt && pNode->ig > min_ig) {
      if(pNode->node0 != 0) recPredict(pNode->node0,idx0,data,yih,min_cnt,min_ig);
      if(pNode->node1 != 0) recPredict(pNode->node1,idx1,data,yih,min_cnt,min_ig);
   }
}

void RandomForest::build() {
   srand(9167543);
   vector<pair<double,double> > oob_predict_acum(pXD_->trn_rows,pair<double,double>(0,0));
   vector<pair<double,double> > validate_predict_acum(pXD_->trn_rows,pair<double,double>(0,0));
   vector<pair<double,double> > tst_predict_acum(pXD_->tst_rows,pair<double,double>(0,0));
   vector<double> tst_predict(pXD_->tst_rows,0);
   vector<pair<int,int> > tst;
   for(int tst_idx=0; tst_idx < pXD_->tst_rows; tst_idx++) {
      tst.push_back(pair<int,int>(tst_idx,1));
   }
   int good_tree_cnt = 0;

   // the below is setup for the key press checking
   char c;
   int n, tem;
   int cnt = 0;
   tem = fcntl(0, F_GETFL, 0);
   fcntl (0, F_SETFL, (tem | O_NDELAY));
   // end key press setup

   vector<int> train_idx;
   vector<pair<int,int> > validate;
   for(int i=0; i<pXD_->trn_labl.size(); i++) {
      if((rand() % folds_) == fold_) validate.push_back(pair<int,int>(i,1));
      else train_idx.push_back(i);
   }

   int tree = 0;
   while(1) {
      //base_feature_sample_cnt_ = (int)(pow((double)(rand() % 1000)/1000.0,2) * 400); //(int)((double)train_idx.size()/3.0);
      vector<pair<int,int> > bag, oob;
      vector<int> w(train_idx.size(),0);
      for(int idx=0; idx<train_idx.size(); idx++) {
         //w[idx]=(int)(338 / pXD_->trn_attr[train_idx[idx]][(pXD_->trn_cols)-1]);
         w[idx]=1;
         //cerr << pXD_->trn_attr[train_idx[idx]][(pXD_->trn_cols)-1] << "," << w[idx] << endl;
      }
      create_bag(bag,oob,train_idx,w);
      //Node* root = new Node;
      Node root;
      int node_cnt=0; 
      double tree_gini=0;
      recAssignNode(&root, bag, "", 0, node_cnt, tree_gini);

      /*
      vector<double> v_predict(pXD_->trn_rows,0);
      recPredict(&root, validate, &(pXD_->trn_attr), v_predict, 0, 0);
      double d2 = 0;
      for(int i=0; i < validate.size(); i++) {
         d2+=pow(pXD_->trn_labl[validate[i].first] - v_predict[validate[i].first],2);
      }
      d2 /= (double)validate.size();
      */

      cerr << tree+1 << "/" << trees_ << "; M: " << base_feature_sample_cnt_ << "; bag_size: " << bag.size() \
           << "; oob_size: " << oob.size() /*<< "; d2: " << d2  */<< "; leaf_node_cnt: " << node_cnt \
           << "; tree_gini: " << tree_gini << endl;

      if(/*d2 < 0.3 || */1) {
         //double weight = 1 / pow((double)node_cnt,20);
         double weight = 1;
         predict(&root, &(pXD_->trn_attr), oob, oob_predict_acum, weight);
         predict(&root, &(pXD_->trn_attr), validate, validate_predict_acum, weight);
         predict(&root, &(pXD_->tst_attr), tst, tst_predict_acum, weight);
         good_tree_cnt++;
      }
      cerr << "good_tree_cnt: " << good_tree_cnt << endl;
      eval(oob_predict_acum,good_tree_cnt,"oob");
      eval(validate_predict_acum,good_tree_cnt,"validate");
      tree++;
      n=0;
      //n = read(0, &c, 1);
      if (n > 0 || good_tree_cnt == trees_) {
         cerr << "Key pressed: " << c << endl;
         break;
      }
   }
   cerr << endl;
   eval(oob_predict_acum,good_tree_cnt,"oob");
   eval(validate_predict_acum,good_tree_cnt,"validate");
   cerr << "writing tst set predictions..." << endl;

   char tstFile[100];
   sprintf(tstFile, "workspace/tst_predict_%d_%d_%d_%d.csv", fold_, folds_, base_feature_sample_cnt_, trees_);
   ofstream fTest(tstFile);
   if(!fTest.is_open()) {
      cerr << "Cannot open " << tstFile << " for writing" << endl;
      return;
   }
   for(int tst_idx=0; tst_idx < pXD_->tst_rows; tst_idx++) {
      double p = tst_predict_acum[tst_idx].first/(double)tst_predict_acum[tst_idx].second;
      p = (p*(double)good_tree_cnt + (double)ls1_)/(double)(good_tree_cnt+ls2_);
      fTest << p << endl;
   }
   fTest.close();

   char oobFile[100];
   sprintf(oobFile, "workspace/oob_predict_%d_%d_%d_%d.csv", fold_, folds_, base_feature_sample_cnt_, trees_);
   ofstream fOob(oobFile);
   if(!fOob.is_open()) {
      cerr << "Cannot open " << oobFile << " for writing" << endl;
      return;
   }
   for(int trn_idx=0;trn_idx < pXD_->trn_rows; trn_idx++) {
      double p = oob_predict_acum[trn_idx].first/(double)oob_predict_acum[trn_idx].second;
      fOob << pXD_->trn_labl[trn_idx] << "," << p << endl;
   }
   fOob.close();
}

void RandomForest::eval(vector<pair<double,double> >& oob_predict_acum, int trees, string type) {
   int noob = 0;
   double correct = 0;
   double ll_sum = 0;

   for(int i=0; i<oob_predict_acum.size(); i++) {
      if(oob_predict_acum[i].second == 0) {
         noob++;
      } else {
         double cnt = (double)oob_predict_acum[i].second;
         double p = oob_predict_acum[i].first/oob_predict_acum[i].second;
         if(p > 0.5 and pXD_->trn_labl[i] > 0.5) correct++;
         if(p <= 0.5 and pXD_->trn_labl[i] <= 0.5) correct++;
         p = (p*(double)trees + (double)ls1_)/(double)(trees + ls2_);
         ll_sum += (pXD_->trn_labl[i] > 0.5) ? log(p) : log(1-p);
      }
   }
   cerr << "never out: " << noob \
        << "; " << type << "_correct: " << correct/(double)(oob_predict_acum.size()-noob) \
        << "; " << type << "_ll: " << -ll_sum/(double)(oob_predict_acum.size()-noob) \
        << endl;
}

void RandomForest::predict(Node* pNode, vector<vector<double> >* data, vector<pair<int,int> >& idx_eval, vector<pair<double,double> >& eval_predict, double weight) {
   int min_cnt = 0;
   double min_ig = 0;
   vector<double> yih(data->size(),-1.0);
   recPredict(pNode, idx_eval, data, yih, min_cnt, min_ig);
   for(int i=0; i < idx_eval.size(); i++) {
      int id = idx_eval[i].first;
      if(yih[id] < 0) cerr << "ERROR:::" << endl;
      double vote = yih[id];
      //double vote = (yih[id] > 0.5) ? 1 : 0;
      eval_predict[id].first += weight*vote;
      eval_predict[id].second += weight;
   }
}

RandomForest::~RandomForest() {}
