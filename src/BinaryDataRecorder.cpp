#include "BinaryDataRecorder.hpp"
#include <fstream>
#include <sys/types.h>

namespace ntee {

//! @returns 0 when everything is okay, -1 if there were problems with
//!          opening the file.
int BinaryDataRecorder::open( const std::string& s )
{
   using namespace std;
   fd_.open( s.c_str(), fstream::out|fstream::binary );
   return (fd_.good())?0:-1;
}


//! recorder method implementations
void BinaryDataRecorder::record( const Socket& from, const Socket& to,
                                 const char* buf, size_t len )
{
   // First element of the record is the direction
   char dest = ( from.name() == "L" )?'R':'L';   // destination transmission
   uint32_t llen = htonl(len); 
   fd_.write( &dest, sizeof(char));
   fd_.write( reinterpret_cast<char*>( &llen ), sizeof(uint32_t));
   fd_.write( buf, len );
}


void BinaryDataRecorder::shutdown() {
   fd_.close();
}

} // end namespace ntee
