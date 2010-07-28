#include "FileRecorder.hpp"
#include "Settings.hpp"
#include "Error.hpp"
#include <time.h>    // for clock_gettime() high precision
#include <fstream>
#include <iomanip>

 
namespace ntee {

//! @brief Constructs the FileRecorder
//!
//! Opens the file descriptor.  If the file can't be opened the program is terminated.
//!
FileRecorder::FileRecorder( const Settings& s )
{
   /* Test the output stream? How do we handle a bad input? */
   out_.open(s.output_filename.c_str(), std::ios_base::out|std::ios_base::trunc);
   ErrIf( out_.fail() ).info("FileRecorder unable to open: %s\n",s.output_filename.c_str());
}


//! @brief Closes the descriptor.
//!
//! Implementation of the Recorder virtual function and it closes the file
//! stream.
void FileRecorder::shutdown()
{
   out_.close();
}


//! @brief Records the data to a file.
//!
//! Virtual function implementation of the Recorder::record interface. This
//! will write the message in a human readable format to the output file.
//!
void FileRecorder::record( const Socket& from, const Socket& to, 
                           const char* buf, size_t len )
{
   header(from,to);
   body(buf, len);
}


void FileRecorder::header( const Socket& from, const Socket& to )
{
   timespec ts;
   clock_gettime(CLOCK_MONOTONIC, &ts);
   out_ << std::dec << ts.tv_sec << ":" << ts.tv_nsec << "  from: " 
        << from.name() << " to: " << to.name() << "\n";
}

void FileRecorder::body( const char* buf, size_t len )
{
   using std::hex;
   using std::setw;
   using std::setfill;
   
   // TODO: convert to hex
   const unsigned char* p = (const unsigned char*) buf;
   size_t at = 0;
   while( at < len ) {
      out_ << setw(8) << setfill('0') << hex << at;
      for( int c=0; at<len && c<8; ++c ) {
         out_ << " ";
         for( int j=0; at<len && j<2; ++j ) {
            out_ << setw(2) << setfill('0') << hex << (int) *p++;
            ++at;
         }
      }
      out_ << "\n";
   }
   out_ << "\n";
}

} // end namespace ntee
