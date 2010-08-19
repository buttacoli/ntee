#include "Player.hpp"
#include "Error.hpp"
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <cstring>

//! @mainpage  Player
//!
//! Read an input data file and play-back the contents.
//!
int main(int argc, char** argv) 
{
   std::string USAGE(
     "Usage: ntee_player [-h] <--client|--server> <host> <port> <datafile>\n");
   std::string HELP(
     "Purpose: Acts as either a client or server program and plays back\n"
     "         canned data from the input data file.\n"
     "\n"
     "Arguments:\n"
     "  --client     Causes the playback program to act like a client,\n"
     "                connecting on the <host> and <port> address. Once\n"
     "                a connection is established, the program will play\n"
     "                client data read from the input file.\n"
     "  --server     Causes the playback program to act like a server,\n"
     "                listening on <host> and <port> and once a connection\n"
     "                is made, playing back server messages from the input\n"
     "                data file.\n"
     "  <host>       Symbolic or IP address to start socket on\n"
     "  <port>       Integer number of port to utilize when running\n"
     "  <datafile>   NTee output file which defines the message data to\n"
     "                be played back with.\n"
     "\n"
     "NOTE:\n"
     "  Argument ordering is important and should exactly follow the usage\n"
     "  statement (ie. --client or --server must be first followed by <host>,\n"
     "  <port>, and <datafile> arguments.  The help option must be the first\n"
     "  option supplied.\n" );
   
   // handle case where we have no arguments given
   ErrIf( argc < 2 ).info(USAGE);
   
   // At least one argument is present. Is it -h, --client, --server 
   // or is it something else?
   Player::Config pc;
   if ( ! strcmp(argv[1],"-h") || ! strcmp(argv[1],"--help") ) {
      std::cerr << USAGE << HELP;
      std::exit(0);
   }
   else if ( strcmp(argv[1],"--client") == 0 ) 
      pc.type = Player::Config::CLIENT;
   else if ( strcmp(argv[1],"--server") == 0 )
      pc.type = Player::Config::SERVER;
   else
      ErrIf( argv[1] ).info("First arg must be either -h, --client, or --server...\n%s\n",
                            USAGE.c_str());

   // handle case where too few options are provided
   ErrIf( argc < 5 ).info("Too few arguments...\n%s\n",USAGE.c_str());


   // Grab the other args    
   pc.host = argv[2];
   ErrIfCatch( boost::bad_lexical_cast, 
               pc.port = boost::lexical_cast<int>( argv[3] ) );
   pc.file = argv[4];

   // Create and configure the player
   Player p( pc );
   
   // Cause connection 
   return p.start();   
}
