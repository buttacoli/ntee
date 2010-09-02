#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <boost/lexical_cast.hpp>
#include "Error.hpp"
#include "TCPSocket.hpp"
#include "IPAddress.hpp"
#include "Buffer.hpp"
using namespace ntee;

static void handleClient(Socket&);

//! This is a simple server to test the ntee program.
//! The server simply sends back to the client the number of bytes it
//! just recieved from the client.
//!
int main(int argc, char** argv)
{
   std::string USAGE("Usage: testSrv <server port>\n");
   ErrIf( argc < 2 ).info(USAGE);

   // Convert the port number arg to an int
   in_port_t port;
   ErrIfCatch(boost::bad_lexical_cast,
              port = boost::lexical_cast<in_port_t>( argv[1] ))
             .info("%s is not a port number!\n",argv[1]);
   
   // Put the supplied ip addr into the sockaddr_in struct, we will reuse this
   // memory later to store client addresses in it as they connect.
   sockaddr_in addr;
   memset( &addr, 0, sizeof(addr) );
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Wildcard address kernel picks best bind     
   addr.sin_port = htons(port);
      
   
   //** Get the server to accept stage
   IPAddress ip(addr);
   Socket* sock = new TCPSocket("Svc");
   sock->listenOn(ip);

   
   //** Forever accept new connections and reply
   for( ;; ) {
     Socket* cliSock = sock->accept("Client");
     std::cout << "Connection made\n";
     
//     char cliName[INET_ADDRSTRLEN];
//     WarnIf( inet_ntop(AF_INET, &addr, cliName, sizeof(cliName) ) == 0 );
//     std::cout << "CONNECTED to: " << ((cliName)?cliName:"Unknown") 
//               << ":" << addr.sin_port << "\n";
     
     handleClient( *cliSock );
     delete cliSock;
   }
   
   delete sock;
   return 0;
}


//! Handles client communication details.
//! The server will loop until the client closes the socket waiting to read
//! up to 1024 bytes off the socket.
//!
//! @param cliSock   client socket
//!
void handleClient( Socket& cliSock ) {

   Buffer* pB;
   size_t rnb, total;

   while( 1 ) {
      total = 0;
      while ( (pB=cliSock.recv()) != 0 ) {
         rnb = pB->len;
         total += rnb;
         delete pB;
      }
      total += rnb;
      if ( total == 0 ) return;
      
      std::cout << "    RECIEVED " << total << " bytes\n"; 

      size_t snb = htonl(total);
      Buffer b;
      b.buf = (const char*) &snb;
      b.len = sizeof(snb);
      WarnIf( (snb=cliSock.send(b)) == -1 );
   } 
}
