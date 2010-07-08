//! @mainpage <i>Network</i> tee (ntee)
//! @author M. Buttacoli
//! last updated:  7/3/2010
//! 
//! @section sect_purpose Purpose
//! <b><i>ntee</i></b> is a program which acts as the unix utility tee would
//! except for the fact that it connects two programs communicating through
//! TCP/IP socket interfaces.
//!
//! @section sect_introduction Introduction
//!  This program was started as a training exercise in using the GIT versioning
//!  tool, and the GIThub public site, C++, the Boost libraries and Doxygen. 
//!  The tool was thought to be useful for debugging simulation processes which
//!  communicate through sockets.
//!
//! @section sect_installation Installation
//!  Needs work.  I would like to setup autoconf and automake, but haven't 
//!  got enough experience with either to know where to start.
//!
//! @section sect_intentions Intentions
//!  <p>It is my intention to develop the program to sufficient quality that it
//!  can be used to debug the simulation problems being encountered and to 
//!  learn some useful lessons about distributed version control and the newer
//!  C++ libraries along the way.</p>
//!  <p>Also a goal is to make this program stand on its own as a C++ library
//!  upon which a cool Java or Qt front end can be wrapped upon.  As I've been
//!  learning, tools which have a nice look to them get a lot more consideration
//!  even if they're not really all that functional.  Silly, but undeniable.</p>
//!

#include <iostream>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "Arguments.hpp"
#include "Settings.hpp"
#include "Builder.hpp"
#include "FileRecorder.hpp"
#include "NTee.hpp"

int main(int argc, char** argv)
{
   //! The @NTEEPORT string when seen in the arguments will be expanded to whatever
   //! port the kernel selected for the ntee server.
   std::string USAGE("Usage: ntee [-h|--help] [-o <path>] [--sock <tcp|udp>] [-p <N>]\n"
                     "             -L <host> <port> -R <cmd> [@NTEEPORT] [args...]\n");
   std::string HELP( "Purpose: NTEE is a program which sits between two other programs communicating\n"
                     "         through sockets.  As traffic comes between programs L and R, ntee\n"
                     "         records the timing and data being sent for further analysis or play-\n"
                     "         back.  The options in the command line usage reflect that there are\n"
                     "         two programs, a left side process 'L', and right side process 'R'\n"
                     "         with ntee sitting in the middle.\n"
                     "\n"
                     "   +---+                           +------+                         +---+\n"
                     "   | L |---listen---><---connect---| ntee |----fork/exec----------->| R |\n"
                     "   |___|                           |______|---listen--><--connect---|___|\n"
                     "\n"
                     "Flags:\n"
                     "  -o <path>         Path to where the output should be written.\n" 
                     "                    Defaults to ./ntee_output\n"
                     "  --sock <tcp|udp>  Defines the socket type which the L and R side processes\n"
                     "                    are using.  Defaults to tcp.\n"
                     "  -p <int>          Forces the port number upon which ntee will serve to the\n"
                     "                    R side process.  This is here in case the R program has\n"
                     "                    its connection hardcoded to use a certain port.\n"
                     "  -L <ip> <int>     The ip address and port number of the L side process.\n"
                     "                    ntee will connect to this process after the R side program\n"
                     "                    has been started and decides to connect with ntees service\n"
                     "                    port.\n"
                     "  -R <cmd> [args]   The user specifies the R side process by giving its\n"
                     "                    command line argument, the special argument @NTEEPORT can\n"
                     "                    be inserted anywhere in the args where the port number\n"
                     "                    that the process would connect would ordinarily be specified\n"
                     "                    with.  Ntee will also define the $NTEEPORT environment var\n"
                     "                    to the value which is being used for the ntee service port.\n"
                     "\n"
                     "Notes:\n"
                     "  Both the -l and -r arguments must be specified for ntee to start properly.\n"
                     "\n"
               );  /* end of HELP */
                   
   //** Collect the arguments
   using namespace ntee;
   Arguments args(USAGE, HELP);
   Settings s = args.parse( argc, argv );
   
   //** Based on the settings, build the right NTee type and add the FileRecorder
   boost::scoped_ptr<NTee> pNT( Builder::build( s ) );
   pNT->addRecorder( boost::shared_ptr<Recorder>(new FileRecorder(s) ));
   return pNT->start();
}
   
