#ifndef _BMRMEXCEPTION_HPP_
#define _BMRMEXCEPTION_HPP_

#include <exception>
#include <string>

class AdamException : public std::exception 
{
public:
	AdamException(const std::string& theMessage, const std::string& theThrower);
	virtual ~AdamException() throw();
	virtual const std::string& Report();

protected:
	std::string message;
	std::string thrower;      
};


#endif
