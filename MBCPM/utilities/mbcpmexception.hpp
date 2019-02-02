#ifndef _BMRMEXCEPTION_HPP_
#define _BMRMEXCEPTION_HPP_

#include <exception>
#include <string>

class MBCPMException : public std::exception 
{
public:
	MBCPMException(const std::string& theMessage, const std::string& theThrower);
	virtual ~MBCPMException() throw();
	virtual const std::string& Report();

protected:
	std::string message;
	std::string thrower;      
};


#endif
