#ifndef _NURFANA_PHASED_ARRAY_H
#define _NURFANA_PHASED_ARRAY_H

#include "nurfana/Event.h" 
#include "TChain.h" 

#ifdef HAVE_NUPHASE
#include "nuphaseEvent.h" 
#include "nuphaseHeader.h" 
#endif

namespace nurfana
{

  class PhasedArrayReader  : public Reader 
  {

    public:

      PhasedArrayReader(const char * dir, int nruns, const int *runs) { setupChain(dir,nruns,runs);  } 
      PhasedArrayReader(const char * dir, int run) { setupChain(dir,1,&run);  } 

      virtual int nEvents() const { return ch_ev_.GetEntries(); }

    protected:
      virtual int loadEvent(int i); 
      virtual int loadMeta(int i); 
    private: 

      void setupChain(const char * dir, int nruns, const int * runs); 
      TChain ch_ev_; 
      TChain ch_hd_; 

#ifdef HAVE_NUPHASE
      nuphase::Event * npEv_; 
      nuphase::Header * npHd_; 
#endif 
  }; 





}
#endif
