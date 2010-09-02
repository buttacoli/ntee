#include "NTee.hpp"
#include "Error.hpp"
#include "Socket.hpp"
#include "Settings.hpp"
#include "UnixSignalHub.hpp"
#include "comm.hpp"
#include "TCPSocket.hpp"
#include "IPAddress.hpp"
#include <errno.h>
#include <algorithm>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <cstdio>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <iostream>

namespace ntee {

//! Instantiates a NTee instance with provided settings.
//!
//! Creates the instance, and stores a const reference to the Settings structure
//! into protected member so deriving classes can utilize it directly.
//!
//! @param s    The filled in Settings structure to work off of.
//!
NTee::NTee( const Settings& s ) : s_(s), Lsock_(0), Rsock_(0) 
{
   Lsock_ = new TCPSocket("L");
}


//! Destructor.  Virtual so deriving classes can write their own if need be.
NTee::~NTee()
{
   // empty
}


//! @brief  Waits on child termination to collect its process status.
//!
//! This routine is called when a SIGCHLD signal is recieved from the kernel.
//! It mearly waits() for the child process, and reports its termination 
//! status to the stdout.
//!
//! @param sig   Signal number recieved, should always be SIGCHLD only.
//!
void NTee::childExited( int sig )
{
   int status;
   pid_t pid = wait(&status);
   
   int exitWith = WEXITSTATUS(status);
   
   // We can't shutdown the Lsock_ socket, even though we know that the R
   // side won't be communicating anymore, because there still could be stuff
   // left on the R socket to read... and send.  But it should be returning
   // EOF real soon.
   
   // returns control to where we were.  If we are in a read, it will have
   // to handle EINTR errno.
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
      else if ( strcmp( *ptr, "@NTEESERVERHOSTADDR" ) == 0 )
         snprintf( *ptr, 19, serverip_.c_str());
      else if ( strcmp( *ptr, "@NTEESERVERHOSTNAME" ) == 0 )
         snprintf( *ptr, 19, serverhost_.c_str());

      ++ptr;
   }

   //** Start the client.
   pid_t pid;
   if ( (pid=fork()) == 0 ) {
      // in the child.
      SysErrIf( execvp( s_.R_cmd[0], s_.R_cmd ) == -1 );
   }
   SysErrIf( pid == -1 ).info("Unable to fork properly\n");
   
   //** trap the SIGCHLD which will be queued to us if the client dies and
   //** cause the program to collect the child process with wait.
   UnixSignalHub::trap(SIGCHLD, boost::bind( &NTee::childExited, this, _1 ));
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
Socket* NTee::constructService( )
{
   Socket* svc = new TCPSocket("Service");
   std::string sport = boost::lexical_cast<std::string>(s_.srv_port);
   IPAddress ipaddr( "127.0.0.1", sport.c_str() );   
   svc->listenOn(ipaddr);
   return svc;
}
   


//! @brief  Listens to both the L and R sockets.
//!
//! This routine will listen to both the L and R side sockets, and pass 
//! any information recieved from one side to the other as well as make
//! a recording of the communication.
void NTee::startListening()
{
   int rc=0;
   fd_set rd_fds;
   int max = std::max(Lsock_->getFD(),Rsock_->getFD());

   // force both L and R sockets to be non-blocking IO.
   fcntl(Lsock_->getFD(), F_SETFL, O_NONBLOCK);
   fcntl(Rsock_->getFD(), F_SETFL, O_NONBLOCK);
      
   FD_ZERO(&rd_fds);
   FD_SET(Lsock_->getFD(), &rd_fds);
   FD_SET(Rsock_->getFD(), &rd_fds);
   
   std::string msg;
   char buf[1024];
   
   try_again:
   while( (rc=select(max+1, &rd_fds, 0, 0, 0)) > 0 ) {
         
      if ( FD_ISSET( Lsock_->getFD(), &rd_fds ) )
         transfer(*Lsock_,*Rsock_);
         
      if ( FD_ISSET( Rsock_->getFD(), &rd_fds ) )
         transfer(*Rsock_,*Lsock_);
      
      FD_ZERO(&rd_fds);
      FD_SET(Lsock_->getFD(), &rd_fds);
      FD_SET(Rsock_->getFD(), &rd_fds);
      
   }
   if ( rc == -1 && errno == EINTR ) {
      goto try_again;    // SIGCHLD came in, but there could be still some
                         // data left to read. Goto's are bad form, but here
                         // we get a lot less code for using it.
   }
   
}   


//! @brief Read N , then Write it.
//!
//! Reads N bytes of data from the socket and puts the buffer which
//! had been dynamically allocated during the read function (with 
//! malloc not new) into a shared_ptr. Scoped_ptr didn't offer the
//! functionality of specifying a DE-allocator method, and that's 
//! why I didn't use it here.  If data had been read (eg len>0), then
//! a Write call is made to the alternate program. If len was returned
//! as zero, then the from socket is closed.
//! 
void NTee::transfer( const Socket& from, const Socket& to )
{
   char* buf = 0;
   size_t len = read_n( from.getFD(), &buf );
   boost::shared_ptr<char> ptr( buf, free );

   if ( len > 0 )
      write_n( to.getFD(), buf, len );
   else
      close(from.getFD());

   alertRecorders(from, to, buf, len);
}


//! @brief Call each Recorder instance back with data
//!
//! This routine loops over the recorders_ container kept by the 
//! NTee instance, calling the record() method of each Recorder instance
//! in turn.<br>
//! <b>NOTES:</b><br><ul>
//! <li>In the future, I'd like to be able to use boost::bind in a 
//!     for_each call, but this caused copies of the Socket instances which
//!     when they terminated, closed the descriptors.
//! <li>Also in the future, it might be good to put these writes into 
//!     another thread so they don't block the select loop from fetching
//!     the next message. NTee should be as transparent as possible to the
//!     timing of messages between R and L programs.
//! </ul>
//! 
void NTee::alertRecorders(const Socket& from, const Socket& to, 
                          const char* buf, 
                          size_t len)
{
   RecCont_t::iterator iter= recorders_.begin();
   for( ; iter != recorders_.end(); ++iter ) {
      (*iter)->record(from, to, buf, len);
   }
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
   Socket* svc = constructService();
   
   //** Not going to accept yet! Instead, we start the client.
   startChildProc();
         
   // Back in the parent (ntee) otherwise we'd have exited.
   SysErrIf( (Rsock_=svc->accept("R")) == 0 );
   svc->close();
   delete svc;
      
   // Report the client connection.
//   char buf[INET_ADDRSTRLEN];
//   inet_ntop(AF_INET, &addr, buf, sizeof(addr));   
//   std::cout << "Client connection: " << buf
//             << ":" << ntohs(addr.sin_port) << "\n";
   
   //** R process has connected... time to connect to the L process.
   int type = (s_.protocol==Settings::TCP)? SOCK_STREAM : SOCK_DGRAM;   
   IPAddress lip( s_.L_host_ip.c_str(), s_.L_port.c_str());
   SysErrIf( Lsock_->connectTo(lip) == -1 );                       
   std::cout << "NTee connected to L side: " << s_.L_host_ip << ":"
             << s_.L_port << "\n";
   
   //** Start listening to both sides and passing the information.
   startListening();
   
   //** shut down all recorders
   std::for_each( recorders_.begin(), recorders_.end(),
                  boost::bind(&Recorder::shutdown, _1));
   
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
