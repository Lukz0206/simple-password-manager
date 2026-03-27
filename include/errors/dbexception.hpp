#ifndef DBEXCEPTION_HPP
#define DBEXCEPTION_HPP
#include <exception>
#include <string>
#include <stdexcept>

class DBException : public std::runtime_error
{

public:
    explicit DBException(const std::string msg);
};

#endif