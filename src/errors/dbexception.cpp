#include "errors/dbexception.hpp"

DBException::DBException(const char *msg) : message(msg)
{
}

const char* DBException::what() const throw() { return message.c_str(); }


DBException::~DBException() {};