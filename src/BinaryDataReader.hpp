#ifndef INCLUDED_BinaryDataReader_HPP
#define INCLUDED_BinaryDataReader_HPP

#include <fstream>
#include "Buffer.hpp"
#include "NTee.hpp"

namespace ntee {


//! @brief  Reads a data file which is in binary compressed format
//!
class BinaryDataReader {
public:

   int open(const std::string& file);
   
   Buffer* getNext( const TransferType& tt );
   Buffer* getNext();

   bool good() const;
   bool eof() const;
   
   
private:
   Buffer* mkBuffer();

private:   
   std::ifstream fd_;   //!< The file
};

} // end namespace ntee

#endif

