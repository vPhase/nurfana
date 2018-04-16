#ifndef _nurfana_private_h
#define _nurfana_private_h

#include <string.h> 

/** Private utility methods, not exported */ 
namespace nurfana
{

    bool consume(Option_t * opt, const char * what) 
    {
      char * where = strstr(opt,what); 

      if (where) 
      {
        for (int i = 0; i < strlen(what); i++) where[i]=' '; 
        return true; 
      }
      return false; 
    }


}

#endif
