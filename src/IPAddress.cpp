#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <boost/lexical_cast.hpp>
#include "IPAddress.hpp"
#include "Error.hpp"


//! Instantiate an IPAddress
IPAddress::IPAddress(const char* host, const char* port)
 : pAI_(0), hostname_(host), port_(port)
{
   compute();
}

#include <iostream>
IPAddress::IPAddress(const char* host, int port )
 : pAI_(0), hostname_(host)
{
   port_ = boost::lexical_cast<std::string>(port);
   compute();
}
 


//! Free any allocated address info structures
IPAddress::~IPAddress()
{
   freemem();
}

void IPAddress::freemem()
{
   if ( pAI_ )
      freeaddrinfo(pAI_);
}


//! Returns the size in bytes of the sockaddr_in buffer being used to store
//! information.
int IPAddress::getLen()
{
   return ( pAI_ )?pAI_->ai_addrlen:0;
}


//! Returns the address of sockaddr_in cast to a sockaddr pointer for use
//! in the standard socket routines like accept, connect, etc.
sockaddr* IPAddress::getAddr()
{
   return ( pAI_ )?pAI_->ai_addr:0;
}

int IPAddress::getPort()
{
   int port = ( pAI_ )?((sockaddr_in*)pAI_->ai_addr)->sin_port:0;
   port = htons(port);
   return port;
}

void IPAddress::compute()
{ 
   freemem();
     
   struct addrinfo hints;
   memset(&hints, 0, sizeof(hints));

   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;

   int err = 0;
   SysErrIf( (err=getaddrinfo( hostname_.c_str(), port_.c_str(), 
                               &hints, &pAI_ )) != 0 )
           .info("%s? %d:%s\n",hostname_.c_str(),err,gai_strerror(err));
}


//! Good enough to just do a straight copy of the data.
IPAddress* IPAddress::clone() const
{
   return new IPAddress(*this);
}



/** Test code: g++ -ggdb -o test_code IPAddress.cpp ../lib/ntee.a -lrt
    
int main(int argc, char** argv )
{
   IPAddress ip( argv[1], argv[2] );
   
   int sock = socket(AF_INET, SOCK_STREAM, 0);
   
   int err = 0;
   SysErrIf( (err=connect( sock, ip.getAddr(), ip.getLen() )) == -1 );
   close(sock);
   return err;
}

*/
