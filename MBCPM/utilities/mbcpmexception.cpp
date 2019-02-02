#ifndef _BMRMEXCEPTION_CPP_
#define _BMRMEXCEPTION_CPP_

#include "mbcpmexception.hpp"
#include <sstream>
#include <glog/logging.h>
#include "configuration.hpp"
#include <stdio.h>
using namespace std;

/**   BMRM Exception class constructor
 *
 *    \param theMessage [read] The error message
 *    \param theThrower [read] Description of the object throwing this error
 */
MBCPMException::MBCPMException(const string& theMessage, const string& theThrower="NONAME")
{
   message = theMessage;
   thrower = theThrower;
   LOG(ERROR)<<"theThrower"<<theThrower;
   LOG(ERROR)<<"message:"<<message;
}

/**   BMRM Exception class destructor
 */
MBCPMException::~MBCPMException() throw()
{}


/**   Report error message and related information
 */
const string& MBCPMException::Report()
{
   ostringstream ostr;
   Configuration &config = Configuration::GetInstance();
   // printf("client ID:%d\n",config.GetInt("client_id"));
   ostr << "[BMRM error message] (Thrower: " 
	<< thrower 
	<< ")\n" 
	<< message 
	<< "\n";

   message = ostr.str();
   return message;
}

#endif
