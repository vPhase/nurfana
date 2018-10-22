#ifndef _NURFANA_DECONVOLUTION_H
#define _NURFANA_DECONVOLUTION_H


/** Deconvolution (and dedispersion) routines.
 *
 * 
 *
 * */ 


#include "nurfana/Waveform.h"
#include "nurfana/Response.h"
#include "nurfana/Antenna.h"
#include "TVector3.h" 
#include "TMath.h" 


namespace nurfana 
{

  class Deconvolver
  {
    public: 


      /** This is the method that deconvolvers must implement, 
       * something that takes the waveform to be deconvolved and
       * the response that is to be deconvolved */ 
      virtual void deconvolve(Waveform & w, const FrequencyRepresentation & response) = 0; 


      virtual void deconvolve(Waveform & w, const Response & r, const Antenna & a, TVector3 dir = TVector3(1,0,0)); 

      virtual void deconvolve(Waveform & w, const Response & r, const Antenna & a, double phi_deg = 0, double theta_deg = 0) 
      {
        double phi = phi_deg * TMath::DegToRad(); 
        double theta = theta_deg * TMath::DegToRad(); 
        deconvolve(w,r,a,TVector3( cos(phi) * cos(theta), sin(phi) * cos(theta), sin(theta))); 
      }
  }; 



  class Dedisperser  : public Deconvolver
  {

    public: 
      virtual void deconvolve(Waveform & w, const FrequencyRepresentation & r); 
  }; 


}



#endif
