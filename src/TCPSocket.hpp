#ifndef INCLUDED_TCPSOCKET_HPP
#define INCLUDED_TCPSOCKED_HPP

#include "IPAddress.hpp"
#include "Socket2.hpp"
#include "Buffer.hpp"

namespace ntee {

class TCPSocket : public Socket2 {
public:
   
   TCPSocket();
   TCPSocket(int);
   
   int connectTo( Address& );
   Socket2* listenOn( Address& );
   
   int send( const Buffer& );
   Buffer* recv();
   
   bool good() const;
   int close();
   
private:
   int sockfd_;   
};

}   // end namespace ntee

#endif

