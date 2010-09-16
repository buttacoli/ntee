#ifndef INCLUDED_SETTINGS_HPP
#define INCLUDED_SETTINGS_HPP

#include <string>
#include <vector>

namespace ntee {

//! Defines the value for using a wildcard to determine the ntee service port.
const int WILDCARD_PORT = 0;

//! Defines the value of the default output path to use when saving information
//! to a recording file.
const std::string DEFAULT_OUTPUT("ntee_output");


//! @brief Structure to hold ntee configuration.
//!
//! The settings structure centralizes all the configuration settings for
//! the NTee instance.  It stores values for the protocol to configure, 
//! port and output filename settings, plus holds a vector of strings which
//! define the command to execute when starting the R side client.
struct Settings {

   //! These are the supported protocol options
   enum proto { TCP, UDP };
   
   proto protocol;                     //!< protocol to use when connecting
   unsigned short srv_port;            //!< port to start on (0 means wildcard!)
   std::string srv_host;               //!< hostname to use when setting up service
   std::string output_filename;        //!< output file path name
   std::string L_host_ip;              //!< IP address of the L program
   std::string L_port;                 //!< Port at which L is listening
   char** R_cmd;                       //!< Command line args to start R with
   bool hex_only;
   bool binary_only;
   
   //! @brief Initializes a default Settings structure.
   //!
   //! The values assigned through the initializer are the default settings for
   //! the ntee program.
   Settings() : protocol(TCP), 
                srv_port(WILDCARD_PORT),
                srv_host("localhost"),
                output_filename(DEFAULT_OUTPUT),
                L_host_ip(""),
                L_port(""),
                hex_only(false),
                binary_only(false)
   {  /* empty */ }
};


} /* end namespace ntee */

#endif

