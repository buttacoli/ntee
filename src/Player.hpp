#ifndef INCLUDED_PLAYER_HPP
#define INCLUDED_PLAYER_HPP

#include <string>

namespace ntee {
   class Socket;
   class BinaryDataReader;
}

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
      std::string port;
      std::string file;
   };

   //! Instantiates and configures a player
   Player( const Config& );
   
   //! Starts the player state machine
   int start();

private:

   int playback( ntee::Socket*, ntee::BinaryDataReader& );
   ntee::Socket* connect();
   
   Config cfg_;
         
};

#endif


