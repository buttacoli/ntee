#include "Error.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <string>

//! Initialize the static global __caught__ variable used for try and catch block.
bool ErrorHandler::__caught__ = false;

//! Returns a human readable name for Level enumeration.
//! @param lv   The Level value
//! @returns a static constant character pointer to the string which names
//!           the level
//! @throws Never
//!
static const char* level2Name( ErrorHandler::Level lv ) {
   static const char* buf[] = { "INFO", "WARNING", "ERROR", "SYSTEM ERROR" };
   return buf[lv];
} 


//! Constructs an instance of ErrorHandler.
//! The instance returned will have the file and line number initialized
//! to the values being passed in through the args.  Clients should avoid
//! using this method directly, and should instead use the MACRO to allow
//! the pre-processor to fill in the information automatically.
//!
//! @param file    The name of the calling file
//! @param line    The line number where the call to this function occured.
//! @param lv      The severity level setting
//! @param sexpr   The string (char*) expansion of the expression which evaluated
//!                 to true.
//!
//! @throws NEVER
//!
ErrorHandler::ErrorHandler( const char* file, long line, Level lv, const char* sexpr )
{
   if (file)
      file_ = file;
   else
      file_ = "Unknown";
      
   line_ = line;
   level_ = lv;
   errSave_ = errno;
   
   reportStackInfo( sexpr );  
}


//! Destructor.
//! This is the key class... because it allows us to execute the actions necessary
//! when the class goes out of scope, but not until then!
//!
//! @throws NEVER      
ErrorHandler::~ErrorHandler()
{
   respondAppropriately();
}

//! Utility function for printing extra info.  This was put in so that 
//! both versions of the info method could utilize the same code for 
//! actually writting the information.
//!
//! @param fmt   printf style format
//! @param args  variable length argument list reference.  The client
//!              must remember to va_end the list himself.
//!
static void print_info( const char* fmt, va_list& args )
{
   fprintf(stderr,"   :info: ");
   vfprintf(stderr,fmt,args);
}


//! Writes extra user information to screen if the error occurred.
//! This routine will construct a string from the fmt parameter and write to
//! the stderr stream using standard printf formats.
//! 
//! @param fmt  printf style format.
//!
//! @returns a reference to the constant ErrorHandler instance so it can be
//!          chained with more commands.
//!
const ErrorHandler& ErrorHandler::info(const char* fmt, ...) const
{
   va_list args;
   va_start(args,fmt);
      print_info( fmt, args );
   va_end(args);
   return *this;
}


//! Writes extra user information to screen if the error occurred.
//! This routine will construct a string from the fmt parameter and write to
//! the stderr stream using standard printf formats.
//! 
//! @param sfmt  std::string printf style format.
//!
//! @returns a reference to the constant ErrorHandler instance so it can be
//!          chained with more commands.
//!
const ErrorHandler& ErrorHandler::info(const std::string& sfmt, ...) const
{
   va_list args;
   va_start(args, sfmt);
      print_info( sfmt.c_str(), args );
   va_end(args);
   return *this;
}



//! Report the stack trace info.
//! Places information regarding the file and line of the caller, as well as the
//! expression itself in the output line of the stream.
//!
//! @param sexpr   String expansion of the expression being evaluated up the 
//!                 stack.
//! @returns A reference to a constant ErrorHandler instance this.
//!
const ErrorHandler& ErrorHandler::reportStackInfo(const char* sexpr) const
{
   fprintf(stderr, "[%s:%d] %s: %s\n",file_, line_
                                     ,level2Name(level_), sexpr);
   return *this;
}



//! Responce action in ErrorHandler.
//! This routine switches based on level to provide varying responce actions 
//! based on the severity of the error. Severity is indicated by the current
//! Level setting in the ErrorHandler.
//!
void ErrorHandler::respondAppropriately() const
{
   switch (level_) {
   case Info:  return;
   case Warn:  return;
   case Err:  exit(1);
   case SysErr:
      fprintf(stderr, "   :sys : [errno=%d] %s\n", errSave_, strerror(errSave_));
      exit(errSave_);
   default:
      // Unreachable statement
      return;
      break;
   }
}
