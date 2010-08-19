#include "Player.hpp"

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
   return 0;
}

