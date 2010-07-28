#include "Socket.hpp"
#include <boost/lexical_cast.hpp>

//! @brief Returns the human readable name.
//!
//! Returns the name of the Socket if one was assigned at instantiation, otherwise
//! it will return a string format of the file descriptor integer value being
//! used.  A value of -1 would mean that the socket had never been assigned a 
//! file descriptor value.
//! 
std::string Socket::name() const
{
   return (name_.length() > 0)? name_ : boost::lexical_cast<std::string>(fd_);
}
