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
      
   //** Get the server to accept stage
   IPAddress ip("localhost", port);
   Socket* sock = new TCPSocket("Svc");
   sock->listenOn(ip);

   
   //** Forever accept new connections and reply
   for( ;; ) {
     Socket* cliSock = sock->accept("Client");
     std::cout << "Connection made: fd = " << cliSock->getFD() << "\n";     
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
   size_t total;

   while( (pB=cliSock.recv()) != 0 ) {
      total = pB->len;
      delete pB;
      
      std::cout << "    RECIEVED " << total << " bytes\n"; 

      size_t snb = htonl(total);
      Buffer b(false);    // NOT dynamic memory.
      b.buf = (const char*) &snb;
      b.len = sizeof(snb);
      WarnIf( (snb=cliSock.send(b)) < b.len );
   }
   std::cout << "Client DISCONNECTED\n"; 
}
