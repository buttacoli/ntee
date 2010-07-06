#ifndef INCLUDED_ERROR_HPP
#define INCLUDED_ERROR_HPP

#include <cstdarg>
#include <string>

#define DO_ERR_IF(expr,level) \
     if ( expr ) \
        ErrorHandler(__FILE__,__LINE__,level,#expr)

#define InfoIf(expr) DO_ERR_IF(expr, ErrorHandler::Info)
#define WarnIf(expr) DO_ERR_IF(expr, ErrorHandler::Warn)
#define ErrIf(expr) DO_ERR_IF(expr, ErrorHandler::Err)
#define SysErrIf(expr) DO_ERR_IF(expr, ErrorHandler::SysErr)
        

#define DO_TRY_CATCH(exception, expr, level) \
     ErrorHandler::__caught__ = false; \
     try { (expr); } \
     catch ( exception e ) { ErrorHandler::__caught__ = true; }\
     if ( ErrorHandler::__caught__ ) \
        ErrorHandler(__FILE__,__LINE__,level,#expr).info("threw %s\n",#exception)
        
#define ErrIfCatch( exception, expr ) DO_TRY_CATCH(exception, expr, ErrorHandler::Err)

  
//! @brief A solid error handling system makes everything easier.
//!
//! NOTE: It was intended that this functionality be used through the 
//!       macro definitions InfoIf, WarnIf, ErrIf, and SysErrIf. These
//!       macros will make use of the C/C++ preprocessor and pass the
//!       necessary arguments to create a reasonable stack and statement
//!       trace when errors occur.
//! --<b>USE THE MACROS !</b>--
class ErrorHandler {
public:
  //! Public try/catch variable.  This value is used for the try/catch macro
  //! DO_TRY_CATCH as a non-local variable which we can count on being initialized.
  //! and globally available.
  static bool __caught__;
  
  //! Different levels of severity.
  enum Level { Info, Warn, Err, SysErr };
  
  ErrorHandler(const char* f, long line, Level lv, const char* sexpr);
  ~ErrorHandler();
  
  const ErrorHandler& info(const char* fmt, ...) const;
  const ErrorHandler& info(const std::string&, ...) const;
  
private:
  
  const ErrorHandler& reportStackInfo(const char*) const;
  void respondAppropriately() const;

  const char* file_;  //< typically __FILE__
  long line_;         //< typically __LINE__
  Level level_;       //< Severity
  int errSave_;       //< Stored value of errno for system errors
};


#endif
