#ifndef INCLUDED_ADDRESS_HPP
#define INCLUDED_ADDRESS_HPP

#include <sys/types.h>
#include <sys/socket.h>

class Address {
public:
   virtual ~Address() { /* empty */ }
   virtual sockaddr* getAddr() = 0;
   virtual int getLen() = 0;
   
   virtual Address* clone() const = 0;
};

#endif
