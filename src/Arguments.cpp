#include "Arguments.hpp"

namespace ntee {

//! @brief This routine initializes the command line argument parser.
//! @details  It does not parse the actual command line arguments however, 
//! that is left for the parse() method to do.  The usage and help strings 
//! are sent in as a ref so that they can be defined in the main --so a new
//! developer can't help but see them upon starting to read the program.
//!
//! @param usage    string defining the usage of the ntee command line 
//!                 program.
//! @param help     string (a long one) documenting the ntee program and 
//!                 its command line interface.
//! @throws NEVER
Arguments::Arguments( const std::string& usage, const std::string& help)
 : usage_(usage), help_(help)
{
   // empty
}



//! @brief Parses the command line arguments into a Settings structure.
//! @details  This structure is meant to be used with the Builder to produce the 
//! right version of NTee instance.
//!
//! @param argc     number of char* in argv argument
//! @param argv     array of char* argc in length which defines the options
//!                 being passed the ntee command line program.  The zeroth
//!                 element of this array is expected to be the ntee command
//!                 itself, and could potentially be used to extract path
//!                 information if necessary.
//!
//! @returns A Setting structure properly populated and ready to be handed
//!          over to the ntee::Builder to get a new NTee instance.
//!
Settings Arguments::parse( int argc, char** argv )
{
   Settings s;
   
   //! TODO: Parse the args
   
   return s;
}

} /* end namespace ntee */
