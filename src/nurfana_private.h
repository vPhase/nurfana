#ifndef _nurfana_private_h
#define _nurfana_private_h

#include <string.h> 
#include <string> 

/** Private utility methods, not exported */ 
namespace nurfana
{

    bool consume(std::string & opt, const char * what) 
    {
       size_t where = opt.find(what); 

      if (where!=std::string::npos) 
      {
        for (size_t i = where; i < where+strlen(what); i++) opt[i]=' '; 
        return true; 
      }
      return false; 
    }


}

#endif
