#ifndef INCLUDE_SOCKET_HPP
#define INCLUDE_SOCKET_HPP

#include <unistd.h>
#include <iostream>

//! Wraps a Socket descriptor.
//! The class acts as a proxy to the integer socket descriptor specifically
//! so the destructor can be used to close the descriptor when the Socket 
//! goes out of scope.  We can then not have to worry about closing the Socket
//! if error's occur, it will do it by itself.
class Socket {
public:

   
   //! Constructor
   Socket() : fd_(-1) {}
   Socket(const std::string& name) : fd_(-1), name_(name) {}
   
   //! All important destructor.  This calls the close() function when the Socket
   //! goes out of scope, so we can use instances as automatic variables.
   ~Socket() { 
      std::cerr << "close(" << fd_ << ")\n";
      if ( fd_ > -1 ) close(fd_); 
   }
   
   //! Assignment from integer.  This allows the Socket to be initialized directly
   //! from the socket() function so it look natural.
   //! @param v   Integer value to assign fd_
   //! @returns Socket reference
   Socket& operator=( int v ) { fd_ = v; return *this; }
   
   //! Cast to int.  This operator is called whenever a Socket instance is being
   //! used in a situation where the integer would be expected (bind, listen, etc.)
   //! This just makes the code look more natural.
   operator int() const { return fd_; }
   
   //! Return the name of the Socket
   std::string name() const;
   int getDescriptor() const { return fd_; }
   
private:
   // Unassignable and uncopyable?
   Socket( const Socket& );
   Socket& operator=( const Socket& );

   int fd_;            //!< The descriptor  
   std::string name_;  //!< an Identity
};   


//! Socket comparison to integer types.  This template function is instantiated 
//! as the socket instance is compared to integer types.  This happens when 
//! socket() functions are called in such a way: <p>
//!    ErrIf( (sock=socket(...) == -1 ); <p>
//! Here the returned Socket reference being assigned, is compared to -1.
//!
//! @param s    Socket reference on lhs
//! @param v    Templated type (needs to be castable to int) on rhs
//!
//! @returns true if v == s.fd_ as an integer comparison. false otherwise.
//!
template <typename T> bool operator==( const Socket& s, const T& v ) {
  return s.getDescriptor() == v;
}

#endif
