#ifndef INCLUDED_BUFFER_HPP
#define INCLUDED_BUFFER_HPP

#include <time.h>
#include <cstdlib>

namespace ntee {

//! Describes the type of transfer direction.
enum TransferType {
   L_to_R,    //!< Server to Client
   R_to_L     //!< Client to Server
};


//! Encapsulates one message from one direction to another and records the
//! time of the transfer.  The destructor of a Buffer will call delete on
//! any buf available --so make sure you assign buffers which have been 
//! dynamically allocated.
struct Buffer {
   
   Buffer();
   Buffer(const TransferType& tt);
   ~Buffer();
   void setTime();
   
   TransferType type;
   ssize_t len;
   timespec ts;
   
   const char* buf;
   
};


} // end namespace ntee

#endif

