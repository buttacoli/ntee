#ifndef INCLUDED_BINARYDATAFILE_HPP
#define INCLUDED_BINARYDATAFILE_HPP

#include <fstream>
#include "Buffer.hpp"

namespace ntee {


//! @brief  Reads a data file which is in binary compressed format
//!
class BinaryDataFile {
public:

   int open(const std::string& file);
   bool good() const;
   Buffer* getNext( const TransferType& tt );
   bool eof() const;
   
private:
   std::ifstream fd_;   //!< The file
};

} // end namespace ntee

#endif

