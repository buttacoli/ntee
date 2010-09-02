#include "TCPSocket.hpp"
#include "Error.hpp"
#include <unistd.h>

namespace ntee {

TCPSocket::TCPSocket(const char* ccp)
 : Socket(ccp)
{
   sockfd_ = socket( AF_INET, SOCK_STREAM, 0 );
}


TCPSocket::TCPSocket(const char* ccp, int fd )
 : Socket(ccp)
{
   sockfd_ = fd;
}


int TCPSocket::connectTo( Address& addr )
{
   return connect( sockfd_, addr.getAddr(), addr.getLen() );
}


int TCPSocket::listenOn( Address& addr )
{
   int err = 0;
   SysErrIf( (err=bind(sockfd_, addr.getAddr(), addr.getLen() )) == -1 );   
   SysErrIf( (err=listen(sockfd_, 5)) == -1 );
   return err;
}

Socket* TCPSocket::accept(const char* name) {
   int err = 0;
   sockaddr_in saddr;
   socklen_t len = sizeof(saddr);
   SysErrIf( (err=::accept(sockfd_, (sockaddr*) &saddr, &len )) == -1 );
      
   // make an Address out of client
   return new TCPSocket( name, err );
}


int TCPSocket::send( const Buffer& buf )
{
   return ::send(sockfd_, buf.buf, buf.len, 0);
}

ntee::Buffer* TCPSocket::recv()
{
   return new Buffer();
}


bool TCPSocket::good() const
{
   return 1;
}


int TCPSocket::close() {
   return ::close(sockfd_);
}

} // end namespace ntee

/* Test code for TCPSocket

int main(int argc, char** argv)
{
   using namespace ntee;
   IPAddress ip( argv[1], argv[2] );
   TCPSocket sock;
   
   SysErrIf( sock.connectTo( ip ) == -1 );
   sock.close();
   
   return 0;
}

   
*/
