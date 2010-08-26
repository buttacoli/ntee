#include "Player.hpp"
#include "TCPSocket.hpp"
#include "BinaryDataFile.hpp"
#include "Error.hpp"
#include "IPAddress.hpp"
#include <boost/scoped_ptr.hpp>
using namespace ntee;

//! @brief Player Constructor
//!
//! Instantiates a player instance by copying the configuration settings
//! into its own local copy.
//!
//! @param cfg   The configuration settings.
//!
Player::Player( const Config& cfg )
 : cfg_(cfg)
{
   //empty
}


//! @brief  Kicks off the start state of the player.
//!
//! The player is a FSM with it first state being the connection details.
//! When connection is complete the player moves into a broadcast state
//! which start reading through the data file passing data over the 
//! socket as distinct messages, and waiting when it encounters a message
//! from the other side.
//!
//! @returns integer state value the machine reached. END state is zero.
//!
int Player::start()
{
   // Open the file
   BinaryDataFile data;
   SysErrIf( data.open(cfg_.file) != 0 );
   
   // Connect with the client (or server);
   Socket2 *pS;
   ErrIf( (pS=connect()) == 0 );
   boost::scoped_ptr<Socket2> sock(pS);
   
   // Playback the data into the socket
   playback( pS, data );
   
   return 0;
}


//! @brief  Connect with the other side process.  Either waiting for, or
//!         actively connecting with.
//!
//! Based on client or server configuration either actively connect or await
//! for a connection.  The routine should return the established socket for
//! writing to.
//!
//! @returns  Established socket connection
//!
Socket2* Player::connect()
{
   Socket2 *sock = new TCPSocket();
   IPAddress ip( cfg_.host.c_str(), cfg_.port.c_str() );

   if ( cfg_.type == Config::CLIENT ) {
      sock->connectTo(ip);
   }
   else {
      Socket2* client = sock->listenOn(ip);
      sock->close();
      return client;
   }
   return sock;
}


//! @brief  Play back the data in the file
//!
//! Reads data from the data file, and sends it over the connected socket.
//! The method will return -1 if there is any problems sending the data.
//!
//! @return 0 if there were no error sending the data. -1 if something 
//!         bad happened.
//!
int Player::playback( Socket2* pS, BinaryDataFile& data )
{
   TransferType buf_T = (cfg_.type == Config::CLIENT)?R_to_L:L_to_R;
   while ( pS->good() && data.good() && ! data.eof() ) {
      Buffer* pB;
      if ((pB = data.getNext( buf_T )) != 0 ) {
         pS->send( *pB );
         delete pB;
      }
   }
   return (pS->good() && data.eof())?0:-1;
}


   
