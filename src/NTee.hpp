#ifndef INCLUDED_NTEE_HPP
#define INCLUDED_NTEE_HPP

#include <list>
#include <string>
#include <boost/shared_ptr.hpp>
#include "Socket.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


namespace ntee {

// forward declaration
class Settings;


//! An interface class which abstracts the behavior of recording data.
class Recorder {
public:
   //! @brief Destructor
   virtual ~Recorder() {}
   
   //! Called once for each message transferred by the NTee instance. Durning
   //! this callback the Recorder implementation should transfer the message
   //! data to its target.
   //! @param from   Message data is coming from this Socket.
   //! @param to     Message data is going to be sent to this Socket.
   //! @param buf    The message buffer itself.
   //! @param len    Size of the message buffer.
   //!
   virtual void record( const Socket& from, const Socket& to, 
                        const char* buf, size_t len ) = 0;
   
   //! Called when all the sockets have closed and there is no more information
   //! to be recorded.  This allows the Record implementation the opportunity
   //! to gracefully shut itself down, and return any resources.
   virtual void shutdown() = 0;
};


//! This is the main class of the project.
class NTee {
public:
   NTee( const Settings& s );
   
   virtual ~NTee();
   virtual int start();
   
   void addRecorder( const boost::shared_ptr<Recorder>& );

protected:
   const Settings& s_;   //< The information from command line args
   
private:
   
   void startChildProc();
   int constructService( sockaddr_in*, socklen_t* );
   void startListening();
   void transfer(const Socket&, const Socket& );
   void alertRecorders( const Socket&, const Socket&, 
                        const char*, size_t len );
   void childExited( int );
   
   typedef std::list<boost::shared_ptr<Recorder> > RecCont_t;
   
   RecCont_t recorders_;
   std::string serverhost_;
   std::string serverip_;
   unsigned int srvPort_;
   Socket Lsock_;
   Socket Rsock_;
};

} // end namespace ntee


#endif
