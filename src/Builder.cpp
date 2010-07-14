#include "Builder.hpp"
#include "Settings.hpp"
#include "NTee.hpp"

namespace ntee {

//! @brief  Builds an NTee instance to support the options in the
//!         Settings.
//!
//! @param s   Settings structure specifies the options.
//!
//! @returns a new NTee instance off the heap which the client is
//!          responsible for deleting.
//!
NTee* Builder::build( const Settings& s )
{
   return new NTee(s);
}

}
