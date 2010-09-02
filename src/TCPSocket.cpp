#include "TCPSocket.hpp"
#include "Error.hpp"
#include <iostream>
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
   
   // Get values from the kernel for what was assigned during the bind
   socklen_t len = addr.getLen();
   SysErrIf( getsockname(sockfd_, addr.getAddr(), &len) == -1 );
   return err;
}


Socket* TCPSocket::accept(const char* name) {
   int err = 0;
   sockaddr_in saddr;
   socklen_t len = sizeof(saddr);
   SysErrIf( (err=::accept(sockfd_, (sockaddr*) &saddr, &len )) == -1 );
   // make an Socket out of client
   return new TCPSocket( name, err );
}


int TCPSocket::send( const Buffer& buf )
{
   return ::send(sockfd_, buf.buf, buf.len, 0);
}

//! @returns Address of a dynamically allocated Buffer filled with 
//!          dynamically allocated char* content OR NULL if zero
//!          bytes were recieved.
ntee::Buffer* TCPSocket::recv()
{
   int err = 0;
   char* buf = (char*) malloc(10000);
std::cerr << "recieving data from fd=" << sockfd_ << "\n";
   SysErrIf( (err=::recv(sockfd_,buf,10000,0)) == -1 );
   if ( err == 0 ) return 0;
   
   Buffer* pB = new Buffer();
   pB->len = err;
   pB->buf = buf;
   return pB;
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
