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
   virtual ~Recorder() {}
   virtual void record( const Socket& from, const Socket& to, 
                        const char* buf, size_t len ) = 0;
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
   const Settings& s_;
   
private:
   
   void startChildProc();
   int constructService( sockaddr_in*, socklen_t* );
   void startListening();
   void transfer(const Socket&, const Socket& );
   void alertRecorders( const Socket&, const Socket&, 
                        const char*, size_t len );
   
   typedef std::list<boost::shared_ptr<Recorder> > RecCont_t;
   
   RecCont_t recorders_;
   std::string serverhost_;
   unsigned int srvPort_;
   Socket Lsock_;
   Socket Rsock_;
};

} // end namespace ntee


#endif
