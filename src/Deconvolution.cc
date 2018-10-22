#include "nurfana/Deconvolution.h" 




namespace nurfana
{

  void Deconvolver::deconvolve(Waveform & w, const Response & r, const Antenna & a, TVector3 dir)
  {
    FrequencyRepresentation fr( w.freq().Nt(), 0, w.freq().df()); 
    r.H(fr); 
    a.H(fr,dir);
    deconvolve(w,fr); 
  }

  void Dedisperser::deconvolve(Waveform & w, const FrequencyRepresentation & r) 
  {

    auto N = w.freq().Nf(); 
    auto Y = w.updateFreq().updateY(); 
    auto R = r.Y(); 
    for (size_t i = 0; i < N; i++) 
    {
      Y[i] *= std::abs(R[i])/R[i]; 
    }
  }


}

