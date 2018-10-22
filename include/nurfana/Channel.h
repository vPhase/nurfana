#ifndef _NURFANA_CHANNEL_H
#define _NURFANA_CHANNEL_H

/**
 * A detector channel . Its main job is to own the memory for the waveform. 
 *
 * It also takes an antenna, response, and digitizer, but since those
 * are presumably reused a lot, it doesn't own the memory for that. 
 *
 * It also has a "timing group," which is an integer defining a class of channels
 * whose times may accurately be compared (for e.g. correlation). 
 *
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
             const Digitizer* digitizer, 
             int timing_group = 0
             )
        : TNamed(name,name) 
      {
        wf_ = wf;
        ant_ = antenna; 
        response_ = response; 
        digitizer_ = digitizer; 
        timing_group_ =timing_group; 
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
      int timing_group_; 

      ClassDef(Channel,1); 
  }; 
}

#endif 
