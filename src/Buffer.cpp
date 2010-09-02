#include "Buffer.hpp"

namespace ntee {

Buffer::Buffer(bool dyn)
 : len(0), buf(0), dyn_(dyn)
{
   setTime();
}


Buffer::Buffer(const TransferType& tt, bool dyn) 
 : type(tt), len(0), buf(0), dyn_(dyn)
{
   setTime();
}


Buffer::~Buffer()
{
   if ( dyn_ && buf )
      delete buf;
}


void Buffer::setTime()
{
   clock_gettime(CLOCK_MONOTONIC, &ts );
}
   

} // end namespace ntee
