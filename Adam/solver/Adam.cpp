/* Copyright (c) 2009 NICTA
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
 * Created: (14/11/2007) 
 *
 * Last Updated: (07/01/2009)   
 */

#ifndef _BMRM_CPP_
#define _BMRM_CPP_

#include "common.hpp"
#include "Adam.hpp"
#include "timer.hpp"
#include "configuration.hpp"
#include "adamexception.hpp"
#include "loss.hpp"
#include "glog/logging.h"
#include <fstream>
#include <sstream>

using namespace std;


Adam::Adam(CModel *model, CLoss* loss)
   : CSolver(model, loss),
     verbosity(0),
     maxNumOfIter(10000),
     epsilonTol(-1.0),
     relEpsilonTol(-1.0),
     gammaTol(-1.0),
     relGammaTol(-1.0),
     lambda(1.0),
     checkpointPrefix("model.checkpoint"),
     checkpointInterval(100000),
     checkpointMode(KEEP_LATEST)
{
   ConfirmProgramParameters();
   printf("GetBMRMInnerSolver\n");
}


/**  Destructor
 */
Adam::~Adam()
{

}

void Adam::UpdateModel(double* model,double* multiplier,double* gradient, double residual, int len){
   for (int i = 0; i < len; ++i){
      model[i]=model[i]-LearningRate*gradient[i]/(sqrt(multiplier[i])+residual);
   }
}

double Adam::GetCurrentWallClock(){
   gettimeofday(&wallclock, NULL);
   double wallclock_cur_time = double(wallclock.tv_sec) + double(wallclock.tv_usec)/1e6;
   return wallclock_cur_time-start_wallclock;
}
/**  Start training/learning a model w.r.t. the loss object (and the data supplied to it).
 *
 */
void Adam::Train()
{
   printf("Train\n");
   CTimer totalTime;             // total runtime of the training
   CTimer innerSolverTime;       // time for inner optimization (e.g., QP or LP)
   CTimer lossAndGradientTime;   // time for loss and gradient computation
   
   unsigned int iter = 0;        // iteration count
   double loss = 0.0;            // loss function value
   double validateLoss=0.0;

   double exactObjVal = 0.0;     // (exact) objective function value
   double approxObjVal = -1e99;  // convex lower-bound (approximate) of objective function value
   double maxApproxObjVal=-1e99;
   double minExactObjVal = 1e99; // minimum of all previously evaluated (exact) objective function value
   double regVal = 0.0;          // value of the regularizer term e.g., 0.5*w'*w
   double epsilon = 0.0;         // := minExactObjVal - approxObjVal       
   double gamma = 0.0;           // := exactObjVal - approxObjVal
   double prevEpsilon = 0.0;
   double innerSolverTol = 1.0;  // optimization tolerance for inner solver
   int exitFlag = 0;
   int badcount=0;

   std::vector<bool> sink(100);
   std::vector<double> Scales(100);
   for (int i = 0; i < 100; ++i)
   {
      Scales[i]=0.1;
   }

   unsigned int row = 0; 
   unsigned int col = 0;
   TheMatrix &w = _model->GetW();
   testModel->SetW(&w);

   w.Shape(row, col);   
   TheMatrix a(row, col, SML::DENSE);   // gradient vector
   TheMatrix w_best(row,col,SML::DENSE);  // w_t at which pobj is the smallest
   TheMatrix grad(row,col,SML::DENSE);
   TheMatrix multiplier(row,col,SML::DENSE);
   TheMatrix momentum(row,col,SML::DENSE);
   TheMatrix multiplierBar(row,col,SML::DENSE);
   TheMatrix momentumBar(row,col,SML::DENSE);
   double residual=1e-8;

   // start training
   totalTime.Start();
   grad.Zero();
   multiplier.Zero();
   momentum.Zero();
   gettimeofday(&wallclock, NULL);
   start_wallclock = double(wallclock.tv_sec) + double(wallclock.tv_usec)/1e6;
   // Initialize piecewise linear lower bound of empirical risk
   {
      iter = 1;
      lossAndGradientTime.Start();
      _loss->InitStochasticSolver(batchsize);
      printf("before StochasticComputeLossAndGradient\n");
      _loss->StochasticComputeLossAndGradient(loss, a, batchsize);
      a.ScaleAdd(lambda,w);
      printf("iter:%d loss:%f, Objective:%f\n",iter,loss,loss);
      lossAndGradientTime.Stop();

      {
         if(verbosity)
         {
            printf("Initial iteration: computing first linearization at w_0... loss(w_0)=%.6e\n",loss);
            fflush(stdout);
         }
      }

   }
   
   do
   {
      iter++;
      {
         innerSolverTime.Start();
         momentum.Scale(belta1);
         momentum.ScaleAdd(1- belta1, a);
         momentumBar.Zero();
         momentumBar.ScaleAdd(1.0/(1-pow(belta1,iter)),momentum);
         grad.Assign(a);
         grad.ElementWiseMult(a);
         multiplier.Scale(Adamgamma);
         multiplier.ScaleAdd(1-Adamgamma,grad);
         multiplierBar.Zero();
         multiplierBar.ScaleAdd(1.0/(1-pow(Adamgamma,iter)),multiplier);
         UpdateModel(w.Data(),multiplierBar.Data(),momentumBar.Data(),residual,row*col);
         innerSolverTime.Stop();
      }

      lossAndGradientTime.Start();
      _loss->StochasticComputeLossAndGradient(loss, a,batchsize);
      a.ScaleAdd(lambda,w);
      lossAndGradientTime.Stop();
      totalTime.Stop();     

      testLoss->ComputeLoss(validateLoss);
      grad.Zero();
      grad.Add(a);
      grad.ScaleAdd(lambda,w);
      printf("iter:%d loss:%f,Objective:%f, Norm2:%f\nValidateLoss:%f, Validate Objective:%f \n",iter,loss,loss+regVal,grad.Norm2(),validateLoss,validateLoss+regVal);
      LOG(INFO)<<"iter:"<<iter<<" loss:"<<loss<<" Objective:"<<loss+regVal<<" Time:"<<GetCurrentWallClock();
      LOG(INFO)<<"validateLoss:"<<validateLoss<<" ValidateObjective:"<<validateLoss+regVal;
      
      totalTime.Start();
      {
         // Update iteration details and keep best minimizer
         w.Norm2(regVal);
         regVal *= regVal;;
         regVal *= 0.5 * lambda;

         exactObjVal = loss + regVal;
         if(exactObjVal < minExactObjVal)
         {
            minExactObjVal = exactObjVal;
            w_best.Assign(w);
         }
         DisplayIterationInfo(iter,exactObjVal,approxObjVal,epsilon,gamma,
                              loss,regVal,totalTime.CurrentCPUTotal());
            
         // Save model obtained in previous iteration
         SaveCheckpointModel(iter);
      
         // Check if termination criteria satisfied
         // exitFlag = CheckTermination(iter,epsilon,gamma,minExactObjVal,exactObjVal);
      }
   } while(iter<iterTol);

   totalTime.Stop();
   
   {
      // Display after-training details
      DisplayAfterTrainingInfo(iter,minExactObjVal,approxObjVal,loss,
                               w_best,lossAndGradientTime,innerSolverTime,totalTime);
   }
}


/**   Validate program parameters set in Configuration.
 */
void Adam::ConfirmProgramParameters()
{
   printf("ConfirmProgramParameters\n");
   Configuration &config = Configuration::GetInstance();  // make sure configuration file is read before this!
   
   if(config.IsSet("BMRM.verbosity")) 
      verbosity = config.GetInt("BMRM.verbosity");
   
   printf("maxNumOfIter\n");
   if(config.IsSet("BMRM.maxNumOfIter")) 
   {
      maxNumOfIter = config.GetInt("BMRM.maxNumOfIter");
      if(maxNumOfIter < 0)
         throw AdamException("BMRM.maxNumOfIter must be > 0\n","Adam::ConfirmProgramParameters()");
   }

   printf("epsilonTol\n"); 
   if(config.IsSet("BMRM.epsilonTol")) 
      epsilonTol = config.GetDouble("BMRM.epsilonTol");
   
   printf("relEpsilonTol\n");
   if(config.IsSet("BMRM.relEpsilonTol")) 
      relEpsilonTol = config.GetDouble("BMRM.relEpsilonTol");
   printf("gammaTol\n");
   if(config.IsSet("BMRM.gammaTol")) 
      gammaTol = config.GetDouble("BMRM.gammaTol");
   
   if(config.IsSet("BMRM.relGammaTol")) 
      relGammaTol = config.GetDouble("BMRM.relGammaTol");
   
   if(epsilonTol<0.0 && relEpsilonTol<0.0 && gammaTol<0.0 && relGammaTol<0.0)
      throw AdamException("At least one of the BMRM.{epsilonTol, gammaTol, relEpsilonTol, relGammaTol} must be > 0\n","Adam::ConfirmProgramParameters()");
   
   if(config.IsSet("BMRM.lambda"))           
   {
      lambda = config.GetDouble("BMRM.lambda");
      if(lambda <= 0)
         throw AdamException("BMRM.lambda must be > 0\n","Adam::ConfirmProgramParameters()");
   }
   
   printf("checkpointInterval\n");
   if(config.IsSet("BMRM.checkpointInterval")) 
   {
      checkpointInterval = config.GetInt("BMRM.checkpointInterval");
      if(checkpointInterval < 1)
         throw AdamException("BMRM.checkpointInterval must be a positive integer!\n","Adam::ConfirmProgramParameters()");
   }
   printf("checkpointPrefix\n");
   if(config.IsSet("BMRM.checkpointPrefix")) 
      checkpointPrefix = config.GetString("BMRM.checkpointPrefix");
   printf("checkpointMode\n");
   if(config.IsSet("BMRM.checkpointMode")) 
   {
      string mode = config.GetString("BMRM.checkpointMode");
      if(mode == "LATEST")
         checkpointMode = KEEP_LATEST;
      if(mode == "ALL")
         checkpointMode = KEEP_ALL;
   }   

   if (config.IsSet("LearningRate"))
      LearningRate=config.GetDouble("LearningRate");
   if (config.IsSet("BatchSize"))
      batchsize=config.GetInt("BatchSize");
   else
      throw AdamException("No BatchSize Defined!\n","Adam::ConfirmProgramParameters()\n");
   if (config.IsSet("iterTol"))
      iterTol=config.GetInt("iterTol");

   if (config.IsSet("Belta1"))
      belta1=config.GetDouble("Belta1");
   else
      throw AdamException("No Belta1 Defined!\n","CBMRM::ConfirmProgramParameters()\n");
   if (config.IsSet("Gamma"))
         Adamgamma=config.GetDouble("Gamma");
   else
      throw AdamException("No Gamma Defined!\n","CBMRM::ConfirmProgramParameters()\n");
 
   std::string TestLabelFile="";
   std::string TestFeatureFile="";
   if (config.IsSet("TestLabelFile"))
      TestLabelFile=config.GetString("TestLabelFile");
   else
      throw AdamException("Cannot find test label file\n","Adam::ConfirmProgramParameters()\n");
   if (config.IsSet("TestFeatureFile"))
      TestFeatureFile=config.GetString("TestFeatureFile");
   else
      throw AdamException("Cannot find test Feature file\n","Adam::ConfirmProgramParameters()\n");
   testData=CDataFactory::GetData(0,1,TestFeatureFile,TestLabelFile);
   testModel=new CModel();  
   testLoss=CLossFactory::GetLoss(testModel, testData);
}


void Adam::DisplayIterationInfo(unsigned int iter, double exactObjVal, double approxObjVal, 
                                 double epsilon,  double gamma, double loss, 
                                 double regVal, double curTime)
{
   if(verbosity <= 0) 
   {
      printf(".");
      if(iter%100 == 0) 
         printf("%d",iter);
   }
   else if(verbosity == 1)
      printf("#%d   eps %.6e  loss %.6e  reg %.6e\n",iter, epsilon, loss, regVal);      
   else if(verbosity == 2)
   {
      printf("#%d   pobj %.6e  aobj %.6e  eps %.6e  gam %.6e  loss %.6e  reg %.6e\n", 
             iter, exactObjVal, approxObjVal, epsilon, gamma, loss, regVal);   
   }
   else if(verbosity >= 3)
   {
      printf("#%d   pobj %.6e  aobj %.6e  eps %.6e  gam %.6e  loss %.6e  reg %.6e  time %0.6e\n", 
             iter, exactObjVal, approxObjVal, epsilon, gamma, loss, regVal, curTime); 
   } 
   fflush(stdout);
}


void Adam::SaveCheckpointModel(unsigned int iter)
{
   if(iter%checkpointInterval == 0) 
   {
      if(checkpointMode == KEEP_LATEST)
         _model->Save(checkpointPrefix);
      else 
      {
         ostringstream oss;
         oss << checkpointPrefix << "." << iter;
         _model->Save(oss.str());
      }
   }
}


int Adam::CheckTermination(unsigned int iter, double epsilon, double gamma, 
                            double minExactObjVal, double exactObjVal)
{
   if(epsilon < 0.0)
   {
      printf("\nProgram status: Error occurred; epsilon should be >= 0  (epsilon=%.6e)!\n",epsilon);
      return -1;
   }

   if(gamma < 0.0)
   {
      printf("\nProgram status: Error occurred; gamma should be >= 0  (gamma=%.6e)!\n",gamma);
      return -2;
   }

   if(iter >= 2)
   {
      double relEpsilon = epsilon/minExactObjVal;
      double relGamma = gamma/exactObjVal;

      if(gamma < gammaTol)
      {
         printf("\nProgram status: Converged. (gamma < gammaTol : %.6e < %.6e)", 
                gamma, gammaTol);
         return 1;
      }
      
      if(epsilon < epsilonTol)
      {
         printf("\nProgram status: Converged. (epsilon < epsilonTol : %.6e < %.6e)", 
                epsilon, epsilonTol);
         return 2;
      }
      
      if(relGamma < relGammaTol)
      {
         printf("\nProgram status: Converged. (rel. gamma < relGammaTol : %.6e < %.6e)", 
                relGamma, relGammaTol);
         return 3;
      }
      
      if(relEpsilon < relEpsilonTol)
      {
         printf("\nProgram status: Converged. (rel. epsilon < relEpsilonTol : %.6e < %.6e)", 
                relEpsilon, relEpsilonTol);
         return 4;
      }
   }

   if(iter >= maxNumOfIter)
   { 
      printf("\nProgram status: Exceeded maximum number of iterations (%d)!\n", 
             maxNumOfIter);
      return 5;
   }

   return 0;
}


void Adam::AdjustInnerSolverOptTol(double& innerSolverTol, double prevEpsilon, double epsilon)
{
}


void Adam::DisplayAfterTrainingInfo(unsigned int iter, double finalExactObjVal, 
                                      double approxObjVal, double loss, 
                                      TheMatrix& w_best, CTimer& lossAndGradientTime,
                                      CTimer& innerSolverTime, CTimer& totalTime)
{
   // legends
   if(verbosity >= 1) 
   {
      printf("\n[Legends]\n");
      if(verbosity > 1)
         printf("pobj: primal objective function value"
                "\naobj: approximate objective function value\n");

      printf("gam: gamma (approximation error) "
             "\neps: lower bound on gam "
             "\nloss: loss function value "
             "\nreg: regularizer value\n");
   }
   
   double norm1 = 0, norm2 = 0, norminf = 0;
   w_best.Norm1(norm1);
   w_best.Norm2(norm2);
   w_best.NormInf(norminf);
   
   printf("\nNote: the final w is the w_t where J(w_t) is the smallest.\n");
   printf("No. of iterations:  %d\n",iter);
   printf("Primal obj. val.: %.6e\n",finalExactObjVal);
   printf("Approx obj. val.: %.6e\n",approxObjVal);
   printf("Primal - Approx.: %.6e\n",finalExactObjVal-approxObjVal);
   printf("Loss:             %.6e\n",loss);
   printf("|w|_1:            %.6e\n",norm1);
   printf("|w|_2:            %.6e\n",norm2);
   printf("|w|_oo:           %.6e\n",norminf);
   
   
   // display timing profile
   printf("\nCPU seconds in:\n");
   printf("1. loss and gradient: %8.2f\n", lossAndGradientTime.CPUTotal());
   printf("2. solver:            %8.2f\n", innerSolverTime.CPUTotal()); 
   printf("               Total: %8.2f\n", totalTime.CPUTotal());
   printf("Wall-clock total:     %8.2f\n", totalTime.WallclockTotal());
}

#endif
