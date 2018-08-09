#ifndef _nurfana_private_h
#define _nurfana_private_h

#include <string.h> 
#include "TString.h" 

/** Private utility methods, not exported */ 
namespace nurfana
{

    inline TString & remove_spaces(TString & str) 
    {

      char * tmp = new char[str.Length() + 1]; 
      memset(tmp,0,str.Length()+1); 
      int j = 0; 
      int last= str.Length(); 
      for (int i = str.Length()-1; i >= 0; i--)
      {
        if (str[i]!=' ') break; 
        last--; 
      }

      for (int i = 0; i < last; i++) 
      {
        if (str[i] != ' ' || (i > 0 && str[i-1] != ' '))
        {
          tmp[j++] = str[i]; 
        }
      }

      str = tmp; 
      delete [] tmp; 
      return str; 
    }


    template <typename T> 
    inline bool parse_opt(TString & opt, const char * name, T * val)
    {
      int where = opt.Index(name); 

      if (where == kNPOS)
        return false; 

      double dval; 

      int n; 
      TString scanstr; scanstr.Form("%s=%%lg%%n", name); 
      int found = sscanf(opt.Data() + where, scanstr.Data(),&dval,&n);
      if (found>0)
      {
        *val = dval; 
        for (int i = 0; i < n; i++) 
        {
          opt[where+i] = ' '; 
        }
        remove_spaces(opt); 
      }
      else
      {
        opt.ReplaceAll(name,""); 
      }
      return true; 
    }

  // used for DRAW handling
    inline bool consume(TString & opt, const char * what) 
    {
      int where = opt.Index(what); 

      if (where!=kNPOS) 
      {
        int len = strlen(what); 
        for (int i = where; i < where+len; i++) opt[i]=' '; 
    //    printf("Found %s\n", what); 
        remove_spaces(opt); 
        return true; 
      }
      return false; 
    }

 

}

#endif
