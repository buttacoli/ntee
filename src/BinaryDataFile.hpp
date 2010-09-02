#ifndef INCLUDED_BINARYDATAFILE_HPP
#define INCLUDED_BINARYDATAFILE_HPP

#include <fstream>
#include "Buffer.hpp"
#include "NTee.hpp"

namespace ntee {


//! @brief  Reads a data file which is in binary compressed format
//!
class BinaryDataFile : public Recorder {
public:

   int open(const std::string& file);
   bool good() const;
   Buffer* getNext( const TransferType& tt );
   bool eof() const;
   
   //! recorder method implementations
   void record( const Socket& from, const Socket& to,
                const char* buf, size_t len ) {}  // TODO: impl              
   void shutdown() {} // TODO: impl
   
private:
   Buffer* readNext();
   
   std::ifstream fd_;   //!< The file
};

} // end namespace ntee

#endif

