#include "NTee.hpp"
#include "Error.hpp"
#include "Socket.hpp"
#include "Settings.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


namespace ntee {

//! Instantiates a NTee instance with provided settings.
//!
//! Creates the instance, and stores a const reference to the Settings structure
//! into protected member so deriving classes can utilize it directly.
//!
//! @param s    The filled in Settings structure to work off of.
//!
NTee::NTee( const Settings& s ) : s_(s)
{
   // empty
}


//! Destructor.  Virtual so deriving classes can write their own if need be.
NTee::~NTee()
{
   // empty
}



//! @brief  Begins the services.
int NTee::start()
{
   //** Build up the service port.
   Socket sock;
   int type = (s_.protocol==Settings::TCP)? SOCK_STREAM : SOCK_DGRAM;   
   SysErrIf( (sock=socket(AF_INET, type, 0 )) == -1 );

   // Here we set the close-on-exec flag for the socket, because we don't want
   // it to carry over to the client process.
   SysErrIf( fcntl( sock, F_SETFD, FD_CLOEXEC ) == -1 );
   
   // Set up the internet address and port 
   sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_port = htons(s_.srv_port);  // 0 is the wildcard port
   SysErrIf( bind(sock, reinterpret_cast<sockaddr*>(&addr),
                  sizeof(addr)) == -1);

   // Get values for whatever the kernel decided to bind us to.   
   char buf[INET_ADDRSTRLEN];
   inet_ntop(AF_INET, &addr, buf, sizeof(addr));
   std::cout << "Server on: " << buf 
             << ":" << ntohs(addr.sin_port) << "\n"; 
   
   // magic # 5 is just what most examples use.
   SysErrIf( listen(sock,5) == -1 ); 
   
   // Not going to accept yet! Instead, we start the client.
   //** Start the client.
   pid_t pid;
   if ( (pid=fork()) == 0 ) {
      // in the child.
      SysErrIf( execvp( s_.R_cmd[0], s_.R_cmd ) == -1 );
   }
   SysErrIf( pid == -1 ).info("Unable to fork properly\n");
         
   // Back in the parent (ntee) otherwise we'd have exited.
   Socket cliSock;
   socklen_t len = sizeof(addr);
   SysErrIf( (cliSock=accept(sock, reinterpret_cast<sockaddr*>(&addr),
                             &len)) == -1 );

   inet_ntop(AF_INET, &addr, buf, sizeof(addr));   
   std::cout << "Client connection: " << buf
             << ":" << ntohs(addr.sin_port) << "\n";
   
   //** R process has connected... time to connect to the L process.
   
   return 0;
}


//! @brief  Adds a data recorder to the NTee list.
//! 
//! Data recorders are signaled when a new message is pulled off one of the 
//! sockets and they should log the data appropriately.
//!
//! @param r   Address of a dynamically allocated Recorder instance which has
//!            been wrapped into a boost::shared_ptr.  The shared_ptr will be
//!            inserted into a list, and will be deleted when its reference
//!            count drops to zero.
//!
void NTee::addRecorder( const boost::shared_ptr<Recorder>& r )
{
   recorders_.push_back( r );
}

} // end namespace ntee
