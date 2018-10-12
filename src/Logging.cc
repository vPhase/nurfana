#include "nurfana/Logging.h" 

#include <vector> 
#include <stdarg.h> 
#include "TMutex.h" 
#include <algorithm>


namespace nurfana
{
  namespace log
  {

  
    static TMutex m; 
    static std::vector<FILE *> outputs; 
    static std::vector<int> min_levels; 
    static std::vector<int> max_levels; 

    __attribute__((constructor)) 
    static void init() 
    {
      set(stdout, LOG_INFO, LOG_INFO); 
      set(stderr, LOG_WARN, LOG_FATAL); 
    }


    void set(FILE * f, LogLevel minl, LogLevel maxl) 
    {

      TLockGuard l(&m); 

      auto o = std::find(outputs.begin(), outputs.end(), f); 
      if (o != outputs.end()) 
      {
        unsigned index = o-outputs.begin(); 
        min_levels[index] = minl; 
        max_levels[index] = maxl; 
      }
      else
      {
        outputs.push_back(f); 
        min_levels.push_back(minl); 
        max_levels.push_back(maxl); 
      }

    }

    int unset(FILE *f) 
    {
      TLockGuard l(&m); 

      auto o = std::find(outputs.begin(), outputs.end(), f); 
      if (o != outputs.end())
      {
        *o = 0; 
      }
      return 0; 
    }

    int out(LogLevel level, const char * format, ...) 
    {

      va_list args; 
      va_start(args, format); 

      TLockGuard l(&m); 
      int ret = 0; 
      for (unsigned i = 0; i < outputs.size(); i++) 
      {
        if (level >= min_levels[i] && level <= max_levels[i]) ret+=vfprintf(outputs[i], format, args); 
      }

      return ret; 
    }

  }
}

