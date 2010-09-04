#include "BinaryDataReader.hpp"
#include "Buffer.hpp"

namespace ntee {

//! @brief Private routine to read and make the next buffer.
//! @returns a dynamically allocated Buffer filled with the next entry from the
//!          data file being read, OR null if the file is empty.
Buffer* BinaryDataReader::mkBuffer()
{
   char dest;
   fd_.read( &dest, sizeof(char));
   uint32_t llen;
   fd_.read( reinterpret_cast<char*>(&llen), sizeof(uint32_t));
   llen = ntohl(llen);
   char* buf = (char*) malloc( llen );
   llen = fd_.readsome( buf, llen );
   
   // If any of the reads got to EOF, then delete the malloc, and return null.
   if ( fd_.eof() ) {
      free( buf ); 
      return 0;
   }
   
   // Not the EOF, so allocate a buffer and fill it up.
   Buffer* pB = new Buffer();
   pB->len = llen;
   pB->buf = buf;
   pB->type = (dest == 'L')?R_to_L
                           :L_to_R;
   return pB;
}


//! @brief Opens the data file for reading.
//!
//! Opens a file for reading.  If the file does not exist, or can't be opened this
//! routine will return -1.
//!
//! @param s  path to a file name.
//! @returns 0 when everything is okay, -1 if there were problems with
//!          opening the file.
int BinaryDataReader::open( const std::string& s )
{
   using namespace std;
   fd_.open( s.c_str(), fstream::in|fstream::binary );
   return (fd_.good())?0:-1;
}


//! @returns true if the file stream is okay
bool BinaryDataReader::good() const
{
   return fd_.good();
}


//! @brief Returns the next buffer of the proper transfer type.  If no more
//!        buffers exist, the routine will return null.
//!
//! @returns  a dynamically allocated buffer which contains all the data
//!           from the next frame with transfer type tt. OR null if there
//!           are no more buffers of type tt.
Buffer* BinaryDataReader::getNext( const TransferType& tt )
{
   Buffer* pB = 0;
   while ( (pB=mkBuffer()) != 0 && pB->type != tt ) {
      delete pB;  // throw buffers of wrong type away!
   }
   return pB;
}


//! @brief  Returns next Buffer in datafile.
//!
//! This routine is different from its cousin which takes an TransferType argument
//! in that this routine returns the next buffer in the file regardless of its 
//! transmission type.
//!
//! @returns a dynamically allocated Buffer, or NULL if the eof has been reached.
Buffer* BinaryDataReader::getNext()
{
   return mkBuffer();
}



//! @brief  Check for end of file marker.
//!
//! True if the end of file marker has been read by the stream.
//!
//! @returns  true if the stream has reached the eof.  false if not.
bool BinaryDataReader::eof() const 
{
   return fd_.eof();
}

} // end namespace ntee
