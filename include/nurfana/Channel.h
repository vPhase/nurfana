#ifndef _NURFANA_CHANNEL_H
#define _NURFANA_CHANNEL_H

/**
 * A detector channel . Its main job is to own the memory for the waveform, antenna, and response. 
 */

#include <memory> 
#include "nurfana/Waveform.h" 
#include "nurfana/Antenna.h" 
#include "nurfana/Response.h" 
#include "TNamed.h" 

namespace nurfana
{

  class Channel  : public TNamed
  {
    public: 
      Channel(const char * name,
             Waveform* wf,
             Antenna*  antenna,
             Response* response)
        : TNamed(name,name) 
      {
        wf_ = wf;
        ant_ = antenna; 
        response_ = response; 
      }
      
      Waveform * wf() { return wf_; } 
      Antenna * antenna() { return ant_; } 
      Response * response() { return response_; } 

      virtual ~Channel() { ; }

    private: 
      Waveform* wf_; 
      Antenna*  ant_; 
      Response* response_; 
      ClassDef(Channel,1); 
  }; 
}

#endif 
