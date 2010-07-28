#include "UnixSignalHub.hpp"
#include <boost/bind.hpp>
#include <algorithm>
#include <iostream>

#define MAXSIGNO SIGRTMAX + 1

//! Create the only instance
UnixSignalHub UnixSignalHub::hub_;


//! This callback is the default route for all signals that have 
//! registered. From here the hub will make a callback to all registered
//! functions for the signal which had been recieved.
void UnixSignalHub::callback( int sig )
{
   std::vector<USigHandler_t>& list = hub_.sigset_.at(sig).list;
   std::for_each( list.begin(), list.end(),
                  boost::bind( &USigHandler_t::operator(), _1, sig ) );   // call: F(sig);
}



//! Constructor, initializes the sigset vector to right length
UnixSignalHub::UnixSignalHub()
 : sigset_(MAXSIGNO) {
   sigemptyset( &sa_.sa_mask );
   sa_.sa_handler = &UnixSignalHub::callback;
}





//! Allow clients to add a signal handler function to the list being 
//! kept for this signal instance.
//! @returns the return value from sigaction() sys call, see sigaction
//!           man page for more information.
int UnixSignalHub::trap( int sig, USigHandler_t hfun )
{

   // sa_mask has all registered signals calling the same callback function.
   sigaddset( &hub_.sa_.sa_mask, sig );
   
   info& sa_info = hub_.sigset_.at(sig);
   sa_info.list.push_back( hfun );
   
   return sigaction( sig, &hub_.sa_, &sa_info.previous );
}

