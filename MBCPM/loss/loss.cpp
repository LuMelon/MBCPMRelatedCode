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
 *          S V N Vishwanathan (SVN.Vishwanathan@nicta.com.au)
 *
 * Created: (02/11/2007) 
 *
 * Last Updated: (14/11/2007)   
 */

#ifndef _LOSS_CPP_
#define _LOSS_CPP_

#include <fstream>
#include "loss.hpp"
#include "configuration.hpp"
#include <stdlib.h>     /* srand, rand */
#include <time.h> 

using namespace std;

CLoss::CLoss()
   : verbosity(0),
     scalingFactor(1.0),
     _model(0)
{
   GetParameters();
}


CLoss::CLoss(CModel* model, unsigned int numOfW, unsigned int dimOfW, bool biasFeature)
   : verbosity(0),
     scalingFactor(1.0),
     _model(model)
{
   GetParameters();
   
   // initialize model
   if(not model->IsInitialized())
   {
      model->Initialize(numOfW, dimOfW, biasFeature);
   }
}
 
    
void CLoss::GetParameters()
{
   Configuration &config = Configuration::GetInstance();
   
   if(config.IsSet("Loss.verbosity")) 
      verbosity = config.GetInt("Loss.verbosity"); 
   
   if(config.IsSet("Loss.scalingFactor")) 
      scalingFactor = config.GetDouble("Loss.scalingFactor");        
}

void CLoss::StochasticIndex(int *array, int batchsize, int len){
    assert(batchsize>0);
    srand (time(NULL));
    int start=0;
    int end=len-batchsize;
    if (end==start)
      array[0]=start;
    else
      array[0]=rand()%(end-start)+start;
    for(int i =1;i<batchsize;i++){
      start=array[i-1]+1;
      end++;
      if (end==start)
        array[i]=start;
      else
        array[i]=rand()%(end-start)+start;
    }
}

#endif
