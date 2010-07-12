#ifndef INCLUDED_COMM_HPP
#define INCLUDED_COMM_HPP

#include <string>
#include <sys/types.h>

//! Writes an entire std::string to the file descriptor
size_t write_n( int fd, const std::string& );

//! Writes an entire buffer to the file descriptor
size_t write_n( int fd, const void* buf, size_t len);

//! Reads an entire buffer length prior to returning.
size_t read_n( int fd, void* buf, size_t maxlen);

//! Reads an entire buffer into a string
size_t read_n( int fd, std::string& s );

#endif
