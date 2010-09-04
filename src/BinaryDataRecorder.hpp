#ifndef INCLUDED_BinaryDataRecorder_HPP
#define INCLUDED_BinaryDataRecorder_HPP

#include "NTee.hpp"
#include <fstream>

namespace ntee {

class BinaryDataRecorder : public Recorder {
public:

   //! Open a file
   int open(const std::string& filename);
   
   //! recorder method implementations
   void record( const Socket& from, const Socket& to,
                const char* buf, size_t len );              
   
   //! Close and release any resources
   void shutdown();

private:
   std::ofstream fd_;  //!< The file stream
};

} // end namespace ntee

#endif
