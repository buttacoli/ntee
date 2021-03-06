#include "Player.hpp"
#include "TCPSocket.hpp"
#include "BinaryDataReader.hpp"
#include "Error.hpp"
#include "IPAddress.hpp"
#include <iostream>
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
   BinaryDataReader data;
   SysErrIf( data.open(cfg_.file) != 0 );
   
   // Connect with the client (or server);
   Socket *pS;
   ErrIf( (pS=connect()) == 0 );
   boost::scoped_ptr<Socket> sock(pS);
   
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
Socket* Player::connect()
{
   Socket *sock = new TCPSocket("Player");
   IPAddress ip( cfg_.host.c_str(), cfg_.port.c_str() );

   if ( cfg_.type == Config::CLIENT ) {
      sock->connectTo(ip);
   }
   else {
      sock->listenOn(ip);
      Socket* client = sock->accept("Cli");
      sock->close();
      delete sock;
      return client;
   }
   return sock;
}


//! @brief  Play back the data in the file
//!
//! Reads data from the data file, and sends it over the connected socket.
//! The method will return -1 if there is any problems sending the data.
//! @param pS    pointer to Socket upon which to play data
//! @param data  Reference to the data source (the Reader) itself.
//! @return 0 if there were no error sending the data. -1 if something 
//!         bad happened.
//!
int Player::playback( Socket* pS, BinaryDataReader& data )
{
   TransferType buf_T = (cfg_.type == Config::CLIENT)?R_to_L:L_to_R;
   while ( pS->good() && data.good() && ! data.eof() ) {
      Buffer* pB;
      if ((pB = data.getNext()) != 0 ) {
         if ( pB->type == buf_T ) {
            // Got a buffer we're supposed to send...
            pS->send( *pB );
         }
         else {
            // Got a buffer we're supposed to recieve! See what we get?
            Buffer* pBGot = pS->recv();
            if ( pBGot->len != pB->len ) {
               std::cerr << "Recieved a message of unmatching length: got=" << pBGot->len
                         << " bytes, expected=" << pB->len << " bytes.\n";
            }
            delete pBGot;
         }  
         delete pB;
      }
   }
   return (pS->good() && data.eof())?0:-1;
}


   
