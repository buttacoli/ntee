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
#include <cstdio>


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


//! @brief Starts the child R side process.
//!
//! Forks and execs the R side process. The parent process, before it forks
//! alter the R side client arguments supplied in the Settings structure, it
//! will replace any instances of @NTEESERVERADDR or @NTEEPORT strings with
//! the ip address and port id upon which the ntee program is currently 
//! attached and listening to.<br>
//! 
//! Since the FD_CLOEXEC flag was set on the listening socket, it will close
//! when this process execs the R side child.
//!
void NTee::startChildProc() {

   //** Replace any @NTEEPORT values in R_cmd with the actual port number
   char** ptr = &s_.R_cmd[1];
   while( *ptr ) {
      if ( strcmp( *ptr, "@NTEEPORT" ) == 0 )
         snprintf( *ptr, 9,"%d",srvPort_);
      else if ( strcmp( *ptr, "@NTEESERVERADDR" ) == 0 )
         snprintf( *ptr, 15, serverhost_.c_str());

      ++ptr;
   }

   //** Start the client.
   pid_t pid;
   if ( (pid=fork()) == 0 ) {
      // in the child.
      SysErrIf( execvp( s_.R_cmd[0], s_.R_cmd ) == -1 );
   }
   SysErrIf( pid == -1 ).info("Unable to fork properly\n");
}


//! @brief  Makes the server side of ntee.
//!
//! Ntee sets up a service of its own that the R side process will connect to.
//! This routine sets up the listening port... but it does not call accept yet.
//! The client must be started prior to ntee blocking in the accept call.
//!
//! @returns the socket value upon which to eventually accept upon.
//!          two more post-conditions of this routine are the setting of the
//!          NTee instance's serverhost_ and srvPort_ members.
Socket NTee::constructService( sockaddr_in* psa, socklen_t* plen )
{
   Socket sock;
   int type = (s_.protocol==Settings::TCP)? SOCK_STREAM : SOCK_DGRAM;   
   SysErrIf( (sock=socket(AF_INET, type, 0 )) == -1 );

   // Here we set the close-on-exec flag for the socket, because we don't want
   // it to carry over to the client process.
   SysErrIf( fcntl( sock, F_SETFD, FD_CLOEXEC ) == -1 );
   
   // Set up the internet address and port 
   memset( psa, 0, *plen);
   psa->sin_family = AF_INET;
   psa->sin_addr.s_addr = htonl(INADDR_ANY);
   psa->sin_port = htons(s_.srv_port);  // 0 is the wildcard port
   SysErrIf( bind(sock, reinterpret_cast<sockaddr*>(psa),
                  *plen) == -1);

   // Get values for whatever the kernel decided to bind us to.
   SysErrIf( getsockname(sock, reinterpret_cast<sockaddr*>(psa),
                         plen) == -1 );

   char buf[INET_ADDRSTRLEN];
   inet_ntop(AF_INET, psa, buf, *plen);
   std::cout << "Server on: " << buf 
             << ":" << ntohs(psa->sin_port) << "\n"; 

   //** Save the server port that was assigned and the host address  
   serverhost_.assign(buf);
   srvPort_ = ntohs(psa->sin_port);

   
   // magic # 5 is just what most examples use.
   SysErrIf( listen(sock,5) == -1 );
   
   return sock;
}



//! @brief  Begins the services.
//!
//! This is the primary interface which clients call to start the ntee 
//! middle man service.  First step, listen for the R side client.  Next
//! step, connect with the L side service.  From this point forward the
//! two processes are now in communication through ntee, this routine 
//! falls into an infinite select loop on both the R side and L side 
//! connection points.
//!
//! @returns status of the commands.  In reality though, the program
//!          exits when it first goes wrong, which is really not a good
//!          thing because this is supposed to be a LIBRARY routine.
//! TODO: fix this problem.
int NTee::start()
{
   //** Build up the service port.
   sockaddr_in addr;
   socklen_t len = sizeof(addr);
   Socket sock = constructService( &addr, &len );
   
   //** Not going to accept yet! Instead, we start the client.
   startChildProc();
         
   // Back in the parent (ntee) otherwise we'd have exited.
   Socket Rsock;
   SysErrIf( (Rsock=accept(sock, reinterpret_cast<sockaddr*>(&addr),
                             &len)) == -1 );
      
   // Report the client connection.
   char buf[INET_ADDRSTRLEN];
   inet_ntop(AF_INET, &addr, buf, sizeof(addr));   
   std::cout << "Client connection: " << buf
             << ":" << ntohs(addr.sin_port) << "\n";
   
   //** R process has connected... time to connect to the L process.
   Socket Lsock;
   int type = (s_.protocol==Settings::TCP)? SOCK_STREAM : SOCK_DGRAM;   
   SysErrIf( (Lsock=socket(AF_INET, type, 0 )) == -1 );

   // Set up the internet address and port 
   memset(&addr, 0, sizeof(addr));
   SysErrIf( inet_pton(AF_INET, s_.L_host_ip.c_str(),
                       reinterpret_cast<sockaddr*>(&addr)) <= 0 );
   addr.sin_family = AF_INET;
   addr.sin_port = htons(s_.L_port);

   SysErrIf( connect(Lsock, reinterpret_cast<sockaddr*>(&addr),
                     sizeof(addr)) == -1 );
                       
   std::cout << "NTee connected to L side: " << s_.L_host_ip << ":"
             << s_.L_port << "\n";
             
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
