#ifndef _BMRMEXCEPTION_CPP_
#define _BMRMEXCEPTION_CPP_

#include "sgdexception.hpp"
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
SGDException::SGDException(const string& theMessage, const string& theThrower="NONAME")
{
   message = theMessage;
   thrower = theThrower;
}

/**   BMRM Exception class destructor
 */
SGDException::~SGDException() throw()
{}


/**   Report error message and related information
 */
const string& SGDException::Report()
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
