#ifndef INCLUDED_ARGUMENTS_HPP
#define INCLUDED_ARGUMENTS_HPP

#include "Settings.hpp"
#include <string>

namespace ntee {

//! Class for parsing command line args.
class Arguments {
public:

  //! Initializes arg parser, and sets usage and help statements.
  Arguments( const std::string& usage, const std::string& help);
  
  //! Builds a Settings structure that the ntee::Builder can use.
  Settings parse( int argc, char** argv );

private:
  const std::string& usage_;  //!< Usage statement
  const std::string& help_;   //!< Help statement
  
};

} /* end ntee namespace */
#endif
