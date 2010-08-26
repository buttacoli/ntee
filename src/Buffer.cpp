#include "Buffer.hpp"

namespace ntee {

Buffer::Buffer()
{
   // empty
}


Buffer::Buffer(const TransferType& tt) 
 : type(tt), len(0), buf(0)
{
   setTime();
}


Buffer::~Buffer()
{
   if ( buf )
      delete buf;
}


void Buffer::setTime()
{
   clock_gettime(CLOCK_MONOTONIC, &ts );
}
   

} // end namespace ntee
