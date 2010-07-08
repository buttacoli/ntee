#include "NTee.hpp"
#include <iostream>

namespace ntee {

NTee::~NTee()
{
   // empty
}


int NTee::start()
{
   std::cout << __FUNCTION__ << "\n";
   return 0;
}


void NTee::addRecorder( const boost::shared_ptr<Recorder>& r )
{
   recorders_.push_back( r );
}

} // end namespace ntee
