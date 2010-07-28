#ifndef INCLUDED_UNIXSIGNALS_HPP
#define INCLUDED_UNIXSIGNALS_HPP

#include <boost/function.hpp>
#include <vector>
#include <signal.h>


typedef boost::function<void (int)> USigHandler_t;

class UnixSignalHub {
public:
   static int trap( int sig, USigHandler_t );

private:
   UnixSignalHub();
   UnixSignalHub(const UnixSignalHub& );
   UnixSignalHub& operator=( const UnixSignalHub& );

   static void callback(int);  //< Kernel calls this method
   static UnixSignalHub hub_;  //< the only singleton instance   

   //! for each signal, there will be one of these info structures to hold
   //! the previous action, and a list of callback functions that have 
   //! registered for the signal.
   struct info {
      struct sigaction previous;
      std::vector<USigHandler_t> list;
   };
   
   //! A vector, with each element being indexed by signal number
   std::vector<info> sigset_;

   //! set used by the trap method to track current signal mask.
   struct sigaction sa_;
   
};

#endif
