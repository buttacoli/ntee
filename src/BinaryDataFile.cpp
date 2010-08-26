#include "BinaryDataFile.hpp"

namespace ntee {

//! @returns 0 when everything is okay, -1 if there were problems with
//!          opening the file.
int BinaryDataFile::open( const std::string& s )
{
   using namespace std;
   fd_.open( s.c_str(), fstream::in|fstream::binary );
   return (fd_.good())?0:-1;
}


//! @returns true if the file stream is okay
bool BinaryDataFile::good() const
{
   return fd_.good();
}


//! @returns  a dynamically allocated buffer which contains all the data
//!           from the next frame with transfer type tt. OR null if there
//!           are no more buffers of type tt.
Buffer* BinaryDataFile::getNext( const TransferType& tt )
{
   return 0;
}


//! @returns  true if the stream has reached the eof.  false if not.
bool BinaryDataFile::eof() const 
{
   return fd_.eof();
}

} // end namespace ntee