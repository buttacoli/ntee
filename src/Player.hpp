#ifndef INCLUDED_PLAYER_HPP
#define INCLUDED_PLAYER_HPP

#include <string>

//! The Player class reads ntee output files and pushes data out
//! over a socket.  Acting like the server.
//!
class Player {
public:
   
   //! Structure defining the options passed in through the command line
   //! for the player's operation.
   struct Config {
      enum { CLIENT, SERVER } type;
      std::string host;
      int port;
      std::string file;
   };

   //! Instantiates and configures a player
   Player( const Config& );
   
   //! Starts the player state machine
   int start();

private:
   Config cfg_;
         
};

#endif


