#ifndef DBEXCEPTION_HPP
#define DBEXCEPTION_HPP
#include <exception>
#include <string>

class DBException : public std::exception
{

public:
    DBException(const char *msg);
    ~DBException();

    const char* what() const throw();
private:
    std::string message;
};

#endif