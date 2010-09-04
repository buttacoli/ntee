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


//! @brief A common data flow buffer.
//! Encapsulates one message from one direction to another and records the
//! time of the transfer.  The destructor of a Buffer will call delete on
//! any buf available --so make sure you assign buffers which have been 
//! dynamically allocated.
struct Buffer {
   
   Buffer(bool dyn=true);
   Buffer(const TransferType& tt, bool dyn=true);
   ~Buffer();
   void setTime();
   
   TransferType type;  //!< The type of buffer
   ssize_t len;        //!< The length in bytes of the buf char array
   timespec ts;        //!< The time the buffer was recorded
   
   const char* buf;    //!< payload
   bool dyn_;          //!< true if payload was dynamically allocated with malloc
};


} // end namespace ntee

#endif

