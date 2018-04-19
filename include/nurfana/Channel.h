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
             std::unique_ptr<Waveform> wf,
             std::unique_ptr<Antenna>  antenna,
             std::unique_ptr<Response> response)
        : TNamed(name,name) 
      {
        wf_ = std::move(wf);
        ant_ = std::move(antenna);
        response_ = std::move(response);
      }
      
      Waveform & wf() { return *wf_; } 
      Antenna & antenna() { return *ant_; } 
      Response & response() { return *response_; } 

      virtual ~Channel() { ; }

    private: 
      std::unique_ptr<Waveform> wf_; 
      std::unique_ptr<Antenna>  ant_; 
      std::unique_ptr<Response> response_; 
  }; 
}

#endif 
