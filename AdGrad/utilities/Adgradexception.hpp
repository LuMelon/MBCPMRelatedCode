#ifndef _BMRMEXCEPTION_HPP_
#define _BMRMEXCEPTION_HPP_

#include <exception>
#include <string>

class AdgradException : public std::exception 
{
public:
	AdgradException(const std::string& theMessage, const std::string& theThrower);
	virtual ~AdgradException() throw();
	virtual const std::string& Report();

protected:
	std::string message;
	std::string thrower;      
};


#endif
