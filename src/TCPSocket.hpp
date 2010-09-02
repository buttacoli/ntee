#ifndef INCLUDED_TCPSOCKET_HPP
#define INCLUDED_TCPSOCKED_HPP

#include "IPAddress.hpp"
#include "Socket.hpp"
#include "Buffer.hpp"

namespace ntee {

class TCPSocket : public Socket {
public:
   
   TCPSocket(const char*);
   TCPSocket(const char*, int);
   
   int connectTo( Address& );
   int listenOn( Address& );
   Socket* accept(const char* name);
   
   int send( const Buffer& );
   Buffer* recv();
   
   bool good() const;
   int close();
   
};

}   // end namespace ntee

#endif

