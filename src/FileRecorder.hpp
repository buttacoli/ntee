#ifndef INCLUDED_FILERECORDER_HPP
#define INCLUDED_FILERECORDER_HPP

#include "NTee.hpp"
#include <fstream>

namespace ntee {

class Settings;

class FileRecorder : public Recorder {
public:
   explicit FileRecorder( const Settings& );
   virtual void record();

private:
   std::ofstream out_;
};

} // end namespace ntee

#endif
   
