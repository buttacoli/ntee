#ifndef INCLUDED_SOCKET_HPP
#define INCLUDED_SOCKET_HPP

#include "Buffer.hpp"
#include "Address.hpp"
#include <string>

namespace ntee {

class Socket {
public:
   
   Socket(const char* cpp) : sockfd_(0), desc_(cpp) { /* empty */ }
   
   virtual ~Socket() { /* empty */ }
   
   virtual int connectTo( Address& ) = 0;
   virtual int listenOn( Address& ) = 0;
   virtual Socket* accept(const char*) = 0;
   
   virtual int send( const ntee::Buffer& ) = 0;
   virtual Buffer* recv() = 0;
   
   virtual bool good() const = 0;
   virtual int close() = 0;

   int getFD() const { return sockfd_; }
   std::string name() const { return desc_; }
   
protected:
   int sockfd_;
   std::string desc_;
};

} // end namespace ntee
#endif
