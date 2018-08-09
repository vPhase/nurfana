#ifndef _NURFANA_TIME_REPRESENTATION_H 
#define _NURFANA_TIME_REPRESENTATION_H 


/**  Waveforms in the time domain may either be represented by an
 * UnevenRepresentation (which is not necessarily uneven, but the t's and y's
 * are both defined) or an EvenRepresentation (where all samples are eventy spaced 
 *
 * Both are a type of TimeDomainRepresentation. 
 * */ 


#include <vector> 
#include "TNamed.h" 
#include "nurfana/Interpolation.h" 
#include "TAttFill.h" 
#include "TAttLine.h" 
#include "TAttMarker.h" 
#include "TMutex.h" 
class TGraph; 

namespace nurfana
{

  class FrequencyRepresentation; 

  /** Virtual parent class for both Uneven and EvenReRepresentation */ 
  class TimeRepresentation : public TNamed, public TAttLine, public TAttMarker, public TAttFill, public Interpolatable
  {

    public: 

      /** Returns the number of samples */ 
      virtual size_t N() const { return y_.size(); } 
      
      /** Returns the y-values of the samples */ 
      virtual const double * y() const { return &y_[0]; } 
      virtual double y(size_t i) const { return y_[i]; } 
      void fillY(double * vals) const; 
      double operator[](size_t i) const { return y(i); } 
      

      /** Resize to N. If longer than current size, new samples will all be zero */ 
      virtual void resize(size_t N) { y_.resize(N); } 

      /** Returns a modifiable pointer to the sample value array */
      double * updateY() { ; return &y_[0]; } 

      /** Returns the time at the given sample */ 
      virtual double t(size_t i) const { return t_[i]; }

      /** Returns an array of the sample times */ 
      virtual const double * t() const { return &t_[0];}; 

      virtual void fillT(double *t) const; 

      /** Returns the difference between a sample and the next sample. No range checking is performed, so will cause problems for i == N()-1. */ 
      virtual double dt(size_t i) const { return  t_[i+1] - t_[i]; }

      /** Returns the smallest sample greater than or equal to t.
       *
       * If start is non-zero, then we only consider the subsample starting at start. This is mainly useful if you
       * have a previous lower bound to reduce the search space of a binary search in the uneven case. 
       *  
       **/ 
      virtual size_t lower_bound(double t, size_t start = 0) const; 


      void setInterpolatorType(InterpolationType t, void * opt); 
      const Interpolator * interpolator() const { return interp_; }

      virtual void Draw(Option_t * opt = "") ;

      /** Utility methods */ 

      double getPeak(unsigned * index = 0, int start = 0, int end = -1, bool use_abs = false) const; 
      double getSumV2(int start = 0, int end = -1) const; 
      double getMean(int start = 0, int end = -1) const; 
      double getRMS(int start = 0, int end = -1) const; 


      TimeRepresentation & operator *(double x); 
      TimeRepresentation & operator /(double x); 
      TimeRepresentation & operator -(double x); 
      TimeRepresentation & operator +(double x); 

      TGraph * makeGraph(TGraph * useme = 0) const; 


    protected: 
      TimeRepresentation();  
      TimeRepresentation(const TimeRepresentation & other);  
      TimeRepresentation & operator=(const TimeRepresentation & other);  
      TimeRepresentation(TimeRepresentation && other);  
      TimeRepresentation(const FrequencyRepresentation & other);  
      TimeRepresentation & operator=(const FrequencyRepresentation & other);  
      virtual ~TimeRepresentation(); 
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
          UnevenRepresentation(size_t N, const double * t, const double * y, double nominal_dt); 
          UnevenRepresentation(const TGraph & g);
          UnevenRepresentation(const EvenRepresentation & even); 

          UnevenRepresentation(const UnevenRepresentation & copy); 
          UnevenRepresentation()  {;}
          UnevenRepresentation & operator=(const UnevenRepresentation & assign); 
          UnevenRepresentation & operator=(const EvenRepresentation & assign); 

          UnevenRepresentation(UnevenRepresentation && move); 

          virtual double * updateT() {  return &t_[0]; }
          bool amIReallyEven() const; 
          double nominalDT() const { return nominal_dt_; }

    protected:

          double nominal_dt_; 
      ClassDef(UnevenRepresentation,1); 
  };


  class EvenRepresentation : public TimeRepresentation
  {
    public: 

      EvenRepresentation(size_t N, const double * y, double dt, double t0 = 0); 
      EvenRepresentation (const UnevenRepresentation & uneven, double dt = 0); 
      EvenRepresentation (const FrequencyRepresentation & freq); 
      EvenRepresentation(): t0_(0), dt_(1) { ; } 
      EvenRepresentation (const EvenRepresentation & copy); 

      EvenRepresentation & operator=(const UnevenRepresentation & assign); 
      EvenRepresentation & operator=(const FrequencyRepresentation & assign); 
      EvenRepresentation & operator=(const EvenRepresentation & assign); 

      virtual void resize(size_t N) { invalidateT();  y_.resize(N); } 
      virtual void pad(size_t n) { resize((1+n)*N()); } 

      virtual size_t lower_bound(double t, size_t start = 0) const { (void) start; return (t-t0())/dt(); }; 

      void setDT(double dt) { dt_ = dt; invalidateT(); } 
      void setT0(double t0) { t0_ = t0; invalidateT(); } 

      double dt(size_t i = 0) const {(void) i;  return dt_; } 
      double t0() const { return t0_; } 

      virtual const double * t() const;
      virtual double t(size_t i) const { return dt_ * i + t0_; }
      virtual void fillT(double *t) const; 

    protected: 
      double t0_; 
      double dt_; 
      volatile mutable bool t_dirty_; 
      void invalidateT() { t_dirty_ = true; }
      mutable TMutex m_; 
      ClassDef(EvenRepresentation,1); 
  }; 


}


#endif 
