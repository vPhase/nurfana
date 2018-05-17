#ifndef _NURFANA_WAVEFORM_H
#define _NURFANA_WAVEFORM_H
/*** 
 *  nurfana main waveform storage class. 
 *  
 *  This provides nice ways to draw things, automatic conversion between time and frequency domain, and a number of signal-processing helpers. 
 *
 *  Cosmin Deaconu <cozzyd@kicp.uchicago.edu>  
 */ 


#include "TObject.h" 
#include "TNamed.h" 
#include "TAttLine.h" 
#include "TAttMarker.h" 
#include "TAttFill.h" 
#include <complex> 
#include <vector> 
#include "nurfana/Interpolation.h" 
#include "nurfana/TimeRepresentation.h" 
#include "nurfana/FrequencyRepresentation.h" 

class TGraph; 
class TH1; 

namespace nurfana
{
  class Waveform : 
     public TNamed, public TAttLine , public TAttMarker, public TAttFill
  {

#ifdef NOTDONEYET

    public: 

      Waveform(const TGraph & g) ; 
      
      /** Create a waveform from an evenly-sampled array */ 
      Waveform(const EvenRepresentation & even); 
      Waveform(EvenRepresentation && even); //move constructor

      /** Create a waveform from an unevenly-sampled array. The unevenly-sampled
       * array will be lost as soon as a transformation is required into an evenly-sampled array */ 
      Waveform(const UnevenRepresentation & uneven); 
      Waveform(UnevenRepresentation && uneven); 

      /** Create a waveform from evenly spaced points in the Fourier domain */ 
      Waveform(const FrequencyRepresentation & freq); 
      Waveform(FrequencyRepresentation && freq); 


      /** Draw this waveform. By default, will draw the type of waveform used for initialization 
       * Other options: 
       *   EVEN    -> Evenly sampled wavefor
       *   UNEVEN  -> Draw uneven waveform (only if initialized, otherwise will draw EVEN) 
       *   POWER, POWER_DB -> Power spectrum 
       *   PHASE    -> Phase
       *   GROUP_DELAY -> Group Delay 
       *   ENVELOPE -> Hilbert Envelope
       **/ 


      /* Returns the hilbert transform of the waveform */ 
      const Waveform & hilbertTransform() const; 
      const Waveform & envelope() const; 

      virtual void Draw(Option_t * opt); 
      virtual void Paint(Option_t * opt); 

      const EvenRepresentation & even() const; 
      EvenRepresentation & updateEven(); 

      const UnevenRepresentation & uneven() const; 
      UnevenRepresentation & updateUneven() ; 

      const FrequencyRepresentation & freq() const; 
      FrequencyRepresentation & updateFreq() ; 

    private: 

      mutable bool _even_dirty; 
      mutable bool _uneven_dirty; 
      mutable bool _freq_dirty; 
      mutable bool _hilbert_dirty; 
      mutable bool _envelope_dirty; 

      mutable EvenRepresentation _even;
      mutable UnevenRepresentation _uneven;
      mutable FrequencyRepresentation _freq;
      mutable Waveform *_hilbert; 
      mutable Waveform *_envelope; 

#endif 
      ClassDef(Waveform,1); 
  }; 
}


#endif
