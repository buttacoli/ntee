#ifndef INCLUDED_FILERECORDER_HPP
#define INCLUDED_FILERECORDER_HPP

#include "NTee.hpp"
#include <fstream>

// forward decl. but Socket is not in ntee namespace!
class Socket;

namespace ntee {

class Settings;

class FileRecorder : public Recorder {
public:
   explicit FileRecorder( const Settings& );
   virtual void record( const Socket&, const Socket&, const char*, size_t );
   virtual void shutdown();
   
private:
   void header( const Socket&, const Socket& );
   void body( const char*, size_t );
   
   std::ofstream out_;
};

} // end namespace ntee

#endif
   
