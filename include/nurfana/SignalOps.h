#ifndef _NURFANA_SIGNAL_OPS_H
#define _NURFANA_SIGNAL_OPS_H

#include "TimeRepresentation.h" 
#include "FrequencyRepresentation.h" 
#include "Waveform.h" 

/** Here we define a whole bunch of operations on signals */ 
namespace nurfana
{
  namespace ops
  {


    /** Performs a hilbert transform on freq. 
     *  if in == out, then the hilbert transform is done in place. 
     **/ 
    FrequencyRepresentation * doHilbertTransform(const FrequencyRepresentation * in, FrequencyRepresentation *out =0 ); 

    inline void doHilbertTransform(FrequencyRepresentation & freq) { doHilbertTransform(&freq,&freq); } 



    /** Computes the correlation of A and B, putting it in out.
     * It is safe for out to equal either A or B. 
     **/ 
    Waveform * correlation(const Waveform * A, const Waveform * B, Waveform * out = 0); 


  /** Computes what I call the Impulsivity Measure of a signal
   *
   *  This is based on something I call a distance CDF, which starts at the
   *  peak of the signal and integrates normalized power outward (on both sides). Points
   *  outside the waveform are considered to be zero. This makes something sort of akin to an ROC curve for impulsivity.  
   *
   *  If all the power is contained within a single sample, the distance CDF 
   *  will just be flat with a value of 1. 
   *  
   *  If all the power is evenly distributed, the distance CDF will be linear with a y-mean of 0.5. 
   *
   *  Therefore, the impulsivity measure is defined as twice the y-mean of the distance CDF minus one. 
   *
   *  
   *  @param wf The waveform to consider
   *  @param distance_cdf If you want to look at the distance cdf, pass a pointer to a TGraph here. It will be resized appropriately. 
   *  @param pt The point to integrate from, otherwise the max abs(value) will be used. 
   *  @param use_envelope True to use the Hilbert Envelope of the waveform to integrate (recommended). Otherwise uses evenly-sampled version. 
   *  @returns The impulsivity measure 
   */ 
 
    double impulsivityMeasure(const Waveform *wf, TGraph * distance_cdf = 0, int pt = -1, bool use_envelope = true); 








  }; 
}


#endif
