#include "comm.hpp"
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>


//! @brief  Write a full buffer.
//!
//! Writes all of the buffer contents to the designated file descriptor.
//! If the write is interrupted it will attempt to continue to send 
//! information after picking up where it left off.  
//!
//! @param fd   File descriptor to write to
//! @param buf  Buffer to send
//! @param len  length in bytes of the buffer.
//!
//! @returns the number of bytes actually sent to the file descriptor,
//!          in the case of an error -1 will be returned.
//!
//! @note { <i>Credit:</i>  Unix Network Programming Vol 1. Richard Stevens (pp. 79) }
//!
size_t write_n( int fd, const void* buf, size_t len )
{
   ssize_t sent = 0;
   const char* ptr = reinterpret_cast<const char*>(buf);
   
   std::cout << "Writing: " << len << " (" << ptr << ")\n";
   
   while( len > 0 ) {
      if ( (sent=write(fd, ptr, len)) <= 0 ) {
         if ( errno == EINTR )
            sent = 0;
         else
            return -1;
      }
      len -= sent;
      ptr += sent;
   }
   return len;
}



//! @brief Write a full buffer
//!
//! This is a utility function for using c++ strings.  It merely wraps the void*
//! version of write_n.
//!
//! @param fd   File descriptor
//! @param s    C++ string
//!
//! @returns the number of bytes actually transferred. -1 will be returned if 
//!          there was any unrecoverable errors during the write.
//!
size_t write_n( int fd, const std::string& s )
{
   return write_n( fd, s.c_str(), s.length() );
}



//! @brief  Reads a full buffer.
//!
//! Reads all of the buffer contents from the designated file descriptor.
//! If the read is interrupted it will attempt to continue to recieve 
//! information after picking up where it left off.  
//!
//! @param fd   File descriptor to write to
//! @param buf  Buffer to fill
//! @param len  max length in bytes of the buffer.
//!
//! @returns the number of bytes actually read from the file descriptor,
//!          in the case of an error -1 will be returned.
//!
//! @note { <i>Credit:</i>  Unix Network Programming Vol 1. Richard Stevens (pp. 79) }
//!
size_t read_n( int fd, void* buf, size_t len )
{
   ssize_t got = 0;
   ssize_t sofar = 0;
   char* ptr = reinterpret_cast<char*>(buf);
   
   while( len > 0 ) {
      if ( (got=read(fd, ptr, len)) < 0 ) {
         if ( errno == EINTR )
            got = 0;  // try agian
         else if ( errno == EAGAIN || errno == EWOULDBLOCK )
            break;    // return what you've got so far.
         else
            return -1;
      }
      else if ( got == 0 ) {
         break;  // EOF
      }
      
      len -= got;
      ptr += got;
      sofar += got;
   }
   return sofar;
}


//! @brief Reads a full buffer into a C++ string
//!
//! This is different from the void* read_n version in that the read continues
//! to read until it reaches an end of file marker.  This works for streams that
//! carry ascii text, but for binary streams its not very useful.
//!
//! @param fd  File descriptor
//! @param s   String to write to.
//!
//! @returns the number of bytes actually written into the string. If there
//!          is an error during the reads, this will return -1
size_t read_n( int fd, std::string& s )
{
   #define BUFSIZE 1025
   static char buf[BUFSIZE];
   ssize_t got;
   
   s.clear();
   while ( (got=read(fd,buf,BUFSIZE-1)) != 0 ) {
      if ( got < 0 ) {
         if (errno == EINTR )
            continue;     // try again
         else if (errno == EAGAIN || errno == EWOULDBLOCK ) {
            std::cout << "EAGAIN\n"; 
            break;        // return what we've got so far
         }
         else
            return -1;    // bad day. return error
      }
      else {
         buf[got] = 0;  // null terminate
         s.append(buf); // append to string.
         std::cout << "reading: " << got << ":" << buf << ":" << s << ":\n";
      }   
   }
   return s.length();
}


//! @brief Dynamically allocates buffer as it is read.
//!
//! This routine will allocate a buffer of the proper length to hold the 
//! entire stream of data from a file descriptor.  Allocation is done with
//! malloc, and re-alloc routines, and it is the client's responsibility
//! to free the memory with free() when it is done with.
//!
//! @param fd   File descriptor to read from
//! @param buf  pointer to a character pointer to set with the address of 
//!              the final allocated buffer.  In case of an allocation 
//!              error this pointer will contain undefined contents.
//! @param allocsz This is a hint at how much to allocate when reading a 
//!                buffer.  If the client doesn't pass this argument, it
//!                will default to 1024 bytes.
//!
//! @returns integer length of allocated buffer and the pointer at buf will
//!           be assigned with the address of the allocated buffer.
//!
size_t read_n( int fd, char** buf, size_t allocsz )
{
   *buf = (char*) malloc(allocsz);
   char* p = *buf;

   size_t total = 0;
   size_t got = 0;
   while ( (got=read_n(fd, p, allocsz)) == allocsz ) {
      total += got;
      *buf = (char*) realloc( *buf, total+allocsz );
      p = *buf + total;
      if ( *buf == 0 ) 
         throw std::bad_alloc();
   }
   
   return total+got;
}
         
