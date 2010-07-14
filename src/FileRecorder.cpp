#include "FileRecorder.hpp"
#include "Settings.hpp"
#include <iostream>

namespace ntee {

FileRecorder::FileRecorder( const Settings& s )
{
   /* Test the output stream? How do we handle a bad input? */
}


//! @brief Records the data to a file.
//!
//! Virtual function implementation of the Recorder::record interface. This
//! will write the message in a human readable format to the output file.
//!
void FileRecorder::record()
{
   //! TODO - record
   std::cout << "FileRecorder: would record\n";
}

} // end namespace ntee
