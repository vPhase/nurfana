#ifndef _NURFANA_LOGGING_H
#define _NURFANA_LOGGING_H

/** Logging utilities */ 

#include <stdio.h> 

namespace nurfana
{
  namespace log
  {

    enum LogLevel
    {
        LOG_CHATTY = 0,  /// If you want an exorbitant amount of output
        LOG_DEBUG = 1,  /// Messages helpful for debugging
        LOG_INFO = 2,   /// "normal" output 
        LOG_WARN = 3,   /// Warnings, should not be ignored
        LOG_ERROR =4,   /// Errors, that means something is really wrong, but
        LOG_FATAL = 5  ///Fatal errors (that will probably result in an abort) 

    };

    /** add another target to logging*/ 
    void set( FILE * f, LogLevel min_level = LOG_INFO, LogLevel max_level = LOG_FATAL ); 

    /** remove a target from logging*/ 
    int unset( FILE * f); 

    int out(LogLevel level, const char * format, ... ); 

  }

} 

#endif
