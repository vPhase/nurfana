#ifndef _NURFANA_FREQUENCY_REPRESENTATION_H
#define _NURFANA_FREQUENCY_REPRESENTATION_H

/*Frequency-Domain Representation of a signal
 *
 **/


#include <vector> 
#include "TObject.h" 
#include "TMutex.h" 
#include "TAttFill.h" 
#include "TAttLine.h" 
#include "TAttMarker.h" 
#include <complex> 

namespace nurfana
{
  class EvenRepresentation; 
  class FrequencyRepresentation : public TObject, public TAttFill, public TAttLine, public TAttMarker
  {

    public: 

      FrequencyRepresentation(size_t num_samples_time_domain, 
                              const std::complex<double>  * Y, 
                              double delta_freq, double t0 = 0); 


      FrequencyRepresentation(const EvenRepresentation & even); 

      virtual void Draw(Option_t * option); 

      /** Number of samples in time domain */
      size_t Nt() const { return Nt_; } 

      /** Number of samples in frequency domain */ 
      size_t Nf() const {return Nt_/2 +1; }

      /** Spacing between frequencies */ 
      double df() const { return df_; } 

      /** Time offset */ 
      double t0() const { return t0_; } 

      /** The frequency at an index */ 
      double f(size_t i) const { return i * df(); } 

      /** Fill an array with the frequency at each index */ 
      void f(double * dest) const; 

      /** Accessors for values */ 
      std::complex<double> Y(size_t i) const { return Y_[i]; }
      std::complex<double> * updateY() { invalidate(); return &Y_[0]; }
      const std::complex<double> * Y() const { return &Y_[0]; }

      /** Change size */
      void setNt(size_t new_N); 

      double mag(size_t i, bool dB = false) const;
      double phase(size_t i, bool unwrapped = false) const; 
      double norm(size_t i, bool dB = false) const; 
      double groupDelay(size_t i) const; 

      void groupDelay(double * dest) const; 
      void mag(double * dest, bool db = false) const; 
      void phase(double * dest, bool unwrap = false) const; 
      void norm(double * dest, bool db = false) const; 

    private: 
     size_t Nt_; 
     double t0_; 
     double df_; 
     std::vector<std::complex<double> > Y_; 
     mutable bool unwrapped_invalid_; 
     mutable bool delay_invalid_; 
     void invalidate() { unwrapped_invalid_ =  true; delay_invalid_ = true; }; 
     void calc_delay() const; 
     void calc_unwrapped() const; 

     mutable std::vector<double> unwrapped_phase_; 
     mutable std::vector<double> group_delay_; 

     mutable TMutex m_unwrap_; 
     mutable TMutex m_delay_; 
     
  }; 



}

#endif
