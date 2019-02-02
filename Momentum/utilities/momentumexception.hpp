#ifndef _BMRMEXCEPTION_HPP_
#define _BMRMEXCEPTION_HPP_

#include <exception>
#include <string>

class MomentumException : public std::exception 
{
public:
	MomentumException(const std::string& theMessage, const std::string& theThrower);
	virtual ~MomentumException() throw();
	virtual const std::string& Report();

protected:
	std::string message;
	std::string thrower;      
};


#endif
