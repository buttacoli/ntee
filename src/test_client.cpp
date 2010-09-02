#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/lexical_cast.hpp>
#include "Error.hpp"
#include "TCPSocket.hpp"
#include "IPAddress.hpp"
using ntee::Socket;
using ntee::TCPSocket;

static int send( int sock, int nb )
{
   static char buf[16384];
   if ( buf[0] == 0 )
      memset( buf, 'X', sizeof(buf));
      
   if ( nb > 16384 ) nb=16384; 
   std::cout << "sending: " << nb << "\n";     
   return send( sock, buf, nb, 0 );
}

static int recv( int sock )
{
   size_t msg = 0;
   SysErrIf( recv( sock, &msg, sizeof(msg), 0 ) == -1 );
   msg = ntohl(msg); 
   std::cout << "recv: " << msg << "\n";
   return msg;
}


//! This is a simple client to test the ntee program.
//! The client simply sends messages of 10 bytes, and after each send
//!  blocks to recieve a message from the server, which is the number
//!  of bytes that the server sent, if the number of bytes doesn't match
//!  the number sent, the message is re-sent until the checks are valid
//!  coming back from the server.
//!
int main(int argc, char** argv)
{
   // echo the command line
   std::cout << "testCli started command:\n\t";
   std::copy(argv, &argv[argc], std::ostream_iterator<char*>(std::cout, " "));
   std::cout << "\n";
   
   std::string USAGE("Usage: testCli <server ip addr> <server port> [N pings=10]\n");
   ErrIf( argc < 3 ).info(USAGE);

   //** Get the server address, port and maxPings arguments
   std::string serverIP( argv[1] );
   
   in_port_t port;
   ErrIfCatch(boost::bad_lexical_cast,
              port = boost::lexical_cast<in_port_t>( argv[2] ))
             .info("%s is not a port number!\n",argv[2]);
   
   
   // Get the last (optional) argument which is the number of times to send to the
   // server. Default to 10.
   int maxPings = 10;
   if ( argc > 3 ) {
      ErrIfCatch( boost::bad_lexical_cast,
                  maxPings=boost::lexical_cast<int>(argv[3]))
               .info("Couldn't convert num of pings arg: '%s' to an int\n", argv[3]);
   }


   
   //** Connect to the server
   Socket* sock = new TCPSocket("Cli");
   IPAddress ipaddr( serverIP.c_str(), port );
   SysErrIf( sock->connectTo(ipaddr) == -1 );
   
   //** Send maxPing messages to the server, each progressively longer then the last
   const int MAX_RETRY = 3;
   int pingCnt = 0;
   while( pingCnt < maxPings ) {
      
      int snb=0;
      int rnb=0;
      int resend = 0;
      do {
         SysErrIf( (snb = send( sock->getFD(), 1<<pingCnt )) == -1 );
         SysErrIf( (rnb = recv( sock->getFD() )) == -1 );
      } while( resend++ < MAX_RETRY && rnb != snb );
      ErrIf( resend == MAX_RETRY )
         .info("Unable to checksum last packet, %d sent != %d recv\n",snb,rnb);
         
      ++pingCnt; 
   }
   
   return 0;
}

