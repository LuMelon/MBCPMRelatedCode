#ifndef _BMRMEXCEPTION_HPP_
#define _BMRMEXCEPTION_HPP_

#include <exception>
#include <string>

class SGDException : public std::exception 
{
public:
	SGDException(const std::string& theMessage, const std::string& theThrower);
	virtual ~SGDException() throw();
	virtual const std::string& Report();

protected:
	std::string message;
	std::string thrower;      
};


#endif
