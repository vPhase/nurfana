#ifndef _NURFANA_FFT_H
#define _NURFANA_FFT_H

/** Thread-safe FFTW3 interface */ 

#include <complex> 

namespace nurfana
{
  namespace fft 
  {
    /** Sets a wisdom file. Wisdom will be loaded from and saved to this file */ 
    void setWisdomFile(const char * f); 

    /** Forward FFT */ 
    void forward(size_t N, const double * y, std::complex<double> * Y); 

    /** Inverse FFT. N is the time domain length (i.e. Y is N/2+1 long) */ 
    void inverse(size_t N, const std::complex<double> * Y, double * y); 
  }
}


#endif
