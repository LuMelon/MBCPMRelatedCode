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
 * Last Updated: 15/01/2009
 *
 * Note: [chteo:100108:1743] now with support for model selection over lambda parameters.
 *       The model files will be appended with "_lambda_XXX" where XXX is the lambda used 
 *       to produce that model.
 */

#include <vector>
#include <fstream>
#include "configuration.hpp"
#include "sml.hpp"
#include "model.hpp"
#include "modelfactory.hpp"
#include "solver.hpp"
#include "solverfactory.hpp"
#include "SGD.hpp"
#include "loss.hpp"
#include "lossfactory.hpp"
#include "data.hpp"
#include "datafactory.hpp"
#include "glog/logging.h"

using namespace std;

int main(int argc, char** argv)
{	
   // sanity check
   if(argc < 2) 
   {
      cout << "Usage: ./linear-bmrm-train config.file" << endl;
      cout << "Check the configfiles directory for examples" << endl;
      cout << "ERROR: No configuration file given!" << endl;
      exit(EXIT_FAILURE);
   }

   CData* data = 0;
   CLoss* loss = 0;
   CSolver* solver = 0;
   CModel* model = 0;
   
   try {
      google::InitGoogleLogging(argv[0]);
      Configuration &config = Configuration::GetInstance();
      printf("ReadFromFile\n");
      config.ReadFromFile(argv[1]);
      printf("after ReadFromFile\n");
      // DO NOT output decision function values and predicted labels to files after evaluation
      config.SetBool("Prediction.outputFvalAndLabels", false);
      
      model = CModelFactory::GetModel();  
      if(config.IsSet("Model.hotStartModel")) 
         model->Initialize(config.GetString("Model.hotStartModel"));		

      data = CDataFactory::GetData();

      loss = CLossFactory::GetLoss(model, data); // loss will initialize model if model is not hot-started

      vector<double> lambdas;
      printf("before GetSolver\n");
      if(config.IsSet("BMRM.lambdas"))
      {
         lambdas = config.GetDoubleVector("BMRM.lambdas");
         cout << "Main(): Multiple lambda parameters detected! Training with each of them..." << endl;
         
         for(size_t i=0; i < lambdas.size(); i++)
         {
            config.SetDouble("BMRM.lambda", lambdas[i]);
            cout << "\n[Learning using lambda: " << lambdas[i] << "]" << endl;
            
            if(solver) delete solver;
            solver = CSolverFactory::GetSolver(model,loss);

            printf("before Train\n");
            solver->Train();		
            // in parallel computation, master holds only a subset of whole dataset
            // so evaluation of training will be bogus
            loss->Evaluate(model);

            {
               ostringstream oss;
               oss << lambdas[i];
               string lambda_str = oss.str();
               string modelFn = "model_lambda_" + lambda_str;
               if(config.IsSet("Model.modelFile"))
                  modelFn = config.GetString("Model.modelFile");
               modelFn = modelFn + "_lambda_" + lambda_str;
               model->Save(modelFn);
            }
         }
      }
      else
      {
         printf("single lambda\n");
         printf("GetSolver\n");
         solver = CSolverFactory::GetSolver(model,loss);
         printf("before Train\n");
         solver->Train();		
         {
            // in parallel computation, master holds only a subset of whole dataset
            // so evaluation of training will be bogus
            loss->Evaluate(model);
            string modelFn = "model";
            if(config.IsSet("Model.modelFile"))
               modelFn = config.GetString("Model.modelFile");
            model->Save(modelFn);
         }
      }                
      
      // cleaning up 
      if(solver) delete solver;
      if(model) delete model;
      if(loss) delete loss;
      if(data) delete data;		
   } 
   catch(SGDException e) {
      cerr << e.Report() << endl;
   }
   
   return EXIT_SUCCESS;	
}
