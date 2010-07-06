#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/lexical_cast.hpp>
#include "Error.hpp"
#include "Socket.hpp"

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
   Socket sock;
   SysErrIf( (sock=socket(AF_INET, SOCK_STREAM, 0)) == -1 );
   SysErrIf( bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1 );
   SysErrIf( listen(sock, 5) == -1 );
   
   //** Forever accept new connections and reply
   for( ;; ) {
     Socket cliSock;
     memset( &addr, 0, sizeof(addr) );
     socklen_t len = sizeof(addr);
     SysErrIf( (cliSock=accept(sock,reinterpret_cast<sockaddr*>(&addr),&len)) == -1 );
     
     char cliName[INET_ADDRSTRLEN];
     WarnIf( inet_ntop(AF_INET, &addr, cliName, sizeof(cliName) ) == 0 );
     std::cout << "CONNECTED to: " << ((cliName)?cliName:"Unknown") 
               << ":" << addr.sin_port << "\n";
               
     handleClient( cliSock );
  
   }
   
   return 0;
}


//! Handles client communication details.
//! The server will loop until the client closes the socket waiting to read
//! up to 1024 bytes off the socket.
//!
//! @param cliSock   client socket
//!
void handleClient( Socket& cliSock ) {

   int rnb;
   static char buf[1024];

   while( 1 ) {
      WarnIf( (rnb=recv(cliSock,buf,sizeof(buf),0)) == -1 );     
      if ( rnb == 0 ) break;
      std::cout << "    RECIEVED " << rnb << " bytes\n"; 

      int snb = htonl(rnb);
      WarnIf( (snb=send(cliSock, &snb, sizeof(snb), 0)) == -1 );
   } 
}
