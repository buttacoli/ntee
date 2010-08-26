#ifndef INCLUDED_SOCKET2_HPP
#define INCLUDED_SOCKET2_HPP

#include "Buffer.hpp"

namespace ntee {

class Socket2 {
public:
   
   virtual ~Socket2() { /* empty */ }
   
   virtual int connectTo( Address& ) = 0;
   virtual Socket2* listenOn( Address& ) = 0;
   
   virtual int send( const ntee::Buffer& ) = 0;
   virtual Buffer* recv() = 0;
   
   virtual bool good() const = 0;
   virtual int close() = 0;
      
};

} // end namespace ntee
#endif
