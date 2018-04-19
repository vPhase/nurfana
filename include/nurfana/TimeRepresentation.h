#ifndef _NURFANA_TIME_REPRESENTATION_H 
#define _NURFANA_TIME_REPRESENTATION_H 

/**  Waveforms in the time domain may either be represented by an
 * UnevenRepresentation (which is not necessarily uneven, but the t's and y's
 * are both defined) or an EvenRepresentation (where all samples are eventy spaced 
 *
 * Both are a type of TimeDomainRepresentation. 
 * */ 


#include <vector> 
#include "TObject.h" 
#include "nurfana/Interpolation.h" 
class TGraph; 
class TH1; 

namespace nurfana
{

  class FrequencyRepresentation; 

  /** Virtual parent class for both Uneven and EvenReRepresentation */ 
  class TimeRepresentation : public TObject
  {

    public: 

      /** Returns the number of samples */ 
      virtual size_t N() const { return y_.size(); } 
      
      /** Returns the y-values of the samples */ 
      const double * y() const { return &y_[0]; } 
      const double  y(size_t i) const { return y_[i]; } 

      /** Resize to N. If longer than current size, new samples will all be zero */ 
      virtual void resize(size_t N) { y_.resize(N); } 

      /** Returns a modifiable pointer to the sample value array */
      double * updateY() { ; return &y_[0]; } 

      /** Returns the time at the given sample */ 
      virtual double t(size_t i) const { return t_[i]; }

      /** Returns the difference between a sample and the next sample. No range checking is performed, so will cause problems for i == N()-1. */ 
      virtual double dt(size_t i) const { return  t_[i+1] - t_[i]; }

      /** Returns the smallest sample greater than or equal to t.
       *
       * If start is non-zero, then we only consider the subsample starting at start. This is mainly useful if you
       * have a previous lower bound to reduce the search space of a binary search in the uneven case. 
       *  
       **/ 
      virtual size_t lower_bound(double t, size_t start = 0) const; 

      /** Returns an array of the sample times */ 
      virtual const double * t() const; 

      void setInterpolatorType(InterpolationType t, void * opt); 
      const Interpolator * interpolator() const { return interp_; }

    protected: 
      TimeRepresentation();  
      std::vector<double> y_; 
      mutable std::vector<double> t_; /**This is mutable because it may be cached */
      virtual void invalidate() { interp_->setInput(this); } 
      Interpolator * interp_; 
      ClassDef(TimeRepresentation,1); 
  }; 



  class EvenRepresentation; 
  class UnevenRepresentation : public TimeRepresentation
  {

    public: 
          UnevenRepresentation(size_t N, const double * t, const double * y); 
          UnevenRepresentation(const TGraph & g);
          UnevenRepresentation(const TH1 & h); 
          UnevenRepresentation (const EvenRepresentation & even); 
          virtual double * updateT() {  return &t_[0]; }

    protected:

      ClassDef(UnevenRepresentation,1); 
  };


  class EvenRepresentation : public TimeRepresentation
  {
    public: 

      EvenRepresentation(size_t N, const double * y, double dt, double t0 = 0); 
      EvenRepresentation(const std::vector<double> & y, double dt, double t0); 
      EvenRepresentation (const UnevenRepresentation & uneven); 
      EvenRepresentation (const FrequencyRepresentation & freq); 
      virtual void resize(size_t N) { invalidateT();  y_.resize(N); } 

      void setDT(double dt) { dt_ = dt; invalidateT(); } 
      void setT0(double t0) { t0_ = t0; invalidateT(); } 
      double dt(size_t i = 0) const {(void) i;  return dt_; } 
      double t0() const { return t0_; } 
      virtual const double * t() const;

    protected: 
      double t0_; 
      double dt_; 
      bool t_dirty_; 
      void invalidateT() { t_dirty_ = true; }
      ClassDef(EvenRepresentation,1); 
  }; 


}


#endif 
