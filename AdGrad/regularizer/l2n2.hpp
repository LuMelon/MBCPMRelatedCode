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
 * Created: 01/09/2008
 *
 * Last Updated:
 */

#ifndef _L2N2_HPP_
#define _L2N2_HPP_


#include "regularizer.hpp"


/** Class for l2-norm square regularizer.
 *  Always vectorize the parameter vector before computing regularization value and 
 *  (sub)gradient.
 */
class CL2N2 : public CRegularizer
{
   public:
      CL2N2() : CRegularizer() {}
      virtual ~CL2N2() {}
      
      virtual void ComputeReg(CModel& model, double& regVal);
      virtual void ComputeRegAndGradient(CModel& model, double& regVal, TheMatrix& regGrad);
};

#endif
