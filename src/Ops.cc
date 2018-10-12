#include "nurfana/SignalOps.h" 
#include "nurfana/Logging.h" 

namespace nurfana
{
  namespace ops 
  {
    FrequencyRepresentation * doHilbertTransform(const FrequencyRepresentation * in, FrequencyRepresentation * out) 
    {

      if (!out) out = new FrequencyRepresentation(*in); 
      else if (in!=out) *out = *in; 

      auto Y = out->updateY(); 
      for (unsigned i = 0; i < out->Nf(); i++) 
      {
        Y[i] = std::complex<double>( -Y[i].imag(), Y[i].real()); 
      }

      return out; 
    }


    Waveform * correlation(const Waveform *A, const Waveform *B, int npad = 0, double scale = 1, Waveform * out = 0) 
    {

      if (A->freq().Nt() != B->freq().Nt())
      {
        log::out(log::LOG_WARN,"correlation does not handle the case where A and B are of different lengths (%zu vs. %zu)!\n", A->freq().Nt(), B->freq().Nt()); 
        return 0; 
      }

      double offset = A->freq().t0()  - B->freq().t0(); 

      if (!out) out = new Waveform(*A); 
      else if (A!=out || B!=out) *out = *A; 

      unsigned N = out->freq().Nt(); 
      double inv = 1./(scale*N);
      auto Y = out->updateFreq().updateY(); 
      for (unsigned i = 0; i < A->freq().Nf(); i++) 
      {
        std::complex<double> vA = A->freq()[i]; 
        std::complex<double> vB = B->freq()[i]; 
        Y[i] = std::complex<double> ( 
             vA.real() * vB.real() + vA.imag() * vB.imag(),
             vA.imag() * vB.real() - vA.real() * vB.imag() 
             ) *inv; 
      }

      // now we want to 

      if (npad) out->updateFreq().pad(npad); 

      //we need to rotate appropriately the Y values to get 0 in the middle 
      auto y = out->updateEven().updateY(); 
      std::rotate(y, y + N/2,y+N); 
      out->updateEven().setT0( offset - N/2 * out->even().dt()); 

      return out; 
    }

  }
}
