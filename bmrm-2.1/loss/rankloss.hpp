/* Copyright (c) 2009, NICTA
 * All rights reserved. 
 * 
 * The contents of this file are subject to the Mozilla Public License 
 * Version 1.1 (the "License"); you may not use this file except in 
 * compliance with the License. You may obtain a copy of the License at 
 * http://www.mozilla.org/MPL/ 
 * 
 * Software distributed under the License is distributed on an "AS IS" 
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the 
 * License for the specific language governing rights and limitations 
 * under the License. 
 * 
 * Authors: Choon Hui Teo (ChoonHui.Teo@anu.edu.au)
 *
 * Created: (02/11/2007) 
 *
 * Last Updated: (13/11/2007)   
 */

#ifndef _RANKLOSS_HPP_
#define _RANKLOSS_HPP_


#include <vector>
#include "sml.hpp"
#include "scalarloss.hpp"
#include "vecdata.hpp"
#include "model.hpp"


/**  
 * Abstract base class for encapsulating binary classification losses. 
 * 
 * The assumption is that the labels are +1, -1, and the loss measures
 * the discrepancy between predicted values and these binary valued
 * labels. This class also contains prediction and performance
 * evaluation metrics for binary classification loss. 
 */
class CRankLoss : public CScalarLoss 
{
   protected:
      virtual void Loss(double& loss, TheMatrix& f) = 0;
      virtual void LossAndGrad(double& loss, TheMatrix& f, TheMatrix& l) = 0;
      void Transform(TheMatrix& f);
      void Perf(TheMatrix& f, TheMatrix& predict);
      void eval_dcg(int offset, int size, int truncation, 
                    double *y, std::vector<int> pi, double &dcg);
      void eval_ndcg(int offset, int size, int truncation, 
                     double *y, double *f, double &ndcg);
      void eval_auc(int offset, int size, int truncation, 
                    double *y, double *f, double &auc);
   public:          
      CModel *mymodel;
      
      CRankLoss(CModel* &model, CVecData* &data)
         : CScalarLoss(model, data) {mymodel = model;}
      
      virtual ~CRankLoss() {}
};

#endif
