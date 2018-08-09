#ifndef _NURFANA_CHANNEL_H
#define _NURFANA_CHANNEL_H

/**
 * A detector channel . Its main job is to own the memory for the waveform. 
 *
 * It also takes an antenna, response, and digitizer, but since those
 * are presumably reused a lot, it doesn't own the memory for that. 
 */

#include "nurfana/Waveform.h" 
#include "nurfana/Antenna.h" 
#include "nurfana/Response.h" 
#include "nurfana/Digitizer.h" 
#include "TNamed.h" 

namespace nurfana
{

  class Channel  : public TNamed
  {
    public: 
      Channel(const char * name,
             Waveform* wf,
             const Antenna*  antenna,
             const Response* response,
             const Digitizer* digitizer
             )
        : TNamed(name,name) 
      {
        wf_ = wf;
        ant_ = antenna; 
        response_ = response; 
        digitizer_ = digitizer; 
      }
      
      Waveform * wf() { return wf_; } 
      const Antenna * antenna() const  { return ant_; } 
      const Response * response() const { return response_; } 
      const Digitizer * digitizer() const { return digitizer_; }

      virtual ~Channel() { delete wf_; }

    private: 
      Waveform* wf_; 
      const Antenna*  ant_; 
      const Response* response_; 
      const Digitizer* digitizer_; 

      ClassDef(Channel,1); 
  }; 
}

#endif 
