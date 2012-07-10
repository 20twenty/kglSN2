#ifndef RANDOMFOREST_H
#define RANDOMFOREST_H

#include <vector>
#include <string>
#include "xData.h"

typedef struct Node {
   int attr_id;
   double ig;
   int exCnt;
   double cutVal;
   double class_a0, class_a1;
   vector<vector<int> > a_l_cnt;
   struct Node* node0;
   struct Node* node1;
} Node;

struct f_ig_cut {
   double ig;
   int attr;
   double cutVal;
   vector<vector<int> > a_l_cnt;
};

class RandomForest {
   public:
      bool isgood() {return good_;}

      void recAssignNode(Node* pNode, vector<pair<int,int> >& idx_orig, string pre, int depth, int& node_cnt, double& tree_gini);
      void build();
      void predict(Node* pNode, vector<vector<double> >* data, vector<pair<int,int> >& idx_eval, vector<pair<double,double> >& eval_predict, double weight);
      void eval(vector<pair<double,double> >& oob_predict, int tree, string type);

      void recPredict(Node* pNode, vector<pair<int,int> >& idx_orig, vector<vector<double> >* data, vector<double>& yih, int min_cnt, double min_ig);
      void recDelete(Node* pNode);

      RandomForest(xData* pXD, int fold, int folds, int Mtry, int trees, bool subset_on);
      ~RandomForest();
   private:
      xData* pXD_;
      bool good_;
      vector<int> good_features_;
      int base_feature_sample_cnt_;
      double progressive_factor_;
      int ls1_, ls2_;
      int fold_, folds_;
      int trees_;
};
#endif
