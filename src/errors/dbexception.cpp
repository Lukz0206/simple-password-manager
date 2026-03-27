#include "errors/dbexception.hpp"

DBException::DBException(const std::string msg) : std::runtime_error(msg)  
{
}
