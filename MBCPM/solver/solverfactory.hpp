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
 *
 * Created: (29/11/2007) 
 *
 * Last Updated:
 */

#ifndef _SOLVERFACTORY_HPP_
#define _SOLVERFACTORY_HPP_

#include <string>

#include "configuration.hpp"
#include "mbcpmexception.hpp"
#include "model.hpp"
#include "loss.hpp"

// solver headers
#include "MBCPM.hpp"

// BT solver is not parallelized yet
/**  
 * Factory class for creating new solvers 
 *
 * When you subclass the CSolver class you must also add an entry into
 * this factory class (grep YOUR_SOLVER for an example). 
 */
class CSolverFactory
{
   public:
      
      /**  Return an instance of solver based on user's argument in configuration file
       *
       *   @param model [read] Pointer to CModel object
       *   @return loss object
       */
      static CSolver* GetSolver(CModel* &model, CLoss* &loss, int procID=0, int nProc=1)
      {         
         CSolver* solver = 0;
         Configuration &config = Configuration::GetInstance();
         
         // select the loss function specified by user (in configuration file)
         if(config.IsSet("Solver.type"))
         {
            std::string solverType = config.GetString("Solver.type");
            if(solverType == "BMRM")
            { 
               solver = new MBCPM(model, loss);
            }
            //{
            //  solver = new CYourSolver(model);
            //}
            else
            {
               throw MBCPMException("ERROR: unrecognised solver ("+solverType+")\n", "CSolverFactory::GetSolver()");
            }
         } 
         else
         {
            throw MBCPMException("ERROR: no solver specified!\n", "CSolverFactory::GetSolver()");
         }
         
         return solver;  
      }
      
};

#endif
