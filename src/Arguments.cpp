#include "Arguments.hpp"
#include "Error.hpp"
#include <algorithm>
#include <functional>
#include <string.h>
#include <iostream>
#include <iterator>
#include <boost/lexical_cast.hpp>


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
   
   ErrIf( argc == 1 ).info(usage_);
   
   int i = 1;   
   std::string prog = argv[0];
   while ( i < argc ) {
      if ( ! strcmp(argv[i],"-h") || ! strcmp(argv[i],"--help") ) {
         std::cerr << usage_ << "\n" << help_ << "\n";
         std::exit(0);
      }
      else if ( ! strcmp(argv[i],"--usage") ) {
         std::cerr << usage_ << "\n";
         std::exit(0);
      }
      else if ( ! strcmp(argv[i],"-o") ) {
         s.output_filename.assign(argv[++i]);
      }
      else if ( ! strcmp(argv[i],"--sock") ) {
         s.protocol = (!strcmp(argv[++i],"udp"))?Settings::UDP:Settings::TCP;
      }
      else if ( ! strcmp(argv[i],"-p") ) {
         ErrIfCatch(boost::bad_lexical_cast,
                    s.srv_port=boost::lexical_cast<unsigned short>(argv[++i]))
                  .info("Bad port number specification\n");
      }
      else if ( ! strcmp(argv[i],"-L") ) {
         s.L_host_ip.assign(argv[++i]);
         ErrIfCatch(boost::bad_lexical_cast,
                    s.L_port=boost::lexical_cast<unsigned short>(argv[++i]))
                 .info("Bad L-port number\n");
      }
      else if ( ! strcmp(argv[i],"-R") ) {
         for( ; i<argc; ++i ) 
            s.R_cmd.push_back( argv[i] );
      }
      else {
         std::cerr << "bad argument : " << argv[i] << "\n";
         std::exit(0);
      }
      ++i;
   }
   
   //Print the structure
   //!TODO - Remove printout
   #define named_value( expr ) #expr << " = " << (expr) << "\n"
   std::cout << named_value( s.protocol )
             << named_value( s.srv_port )
             << named_value( s.output_filename )
             << named_value( s.L_host_ip )
             << named_value( s.L_port )
             << "s.R_cmd = [";
   std::copy( s.R_cmd.begin(), s.R_cmd.end(), 
              std::ostream_iterator<std::string>(std::cout,", "));
   std::cout << "]\n";

   return s;
}

} /* end namespace ntee */
