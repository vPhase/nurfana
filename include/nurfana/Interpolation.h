#ifndef _NURFANA_INTERPOLATION_H
#define _NURFANA_INTERPOLATION_H

/** 
 * nurfana interpolation routines
 *
 * The Interpolator is responsible for interpolation. 
 * You should have one interpolator for each waveform you want to interpolate, as it potentially has internal state. 
 *
 * One can either construct an interpolator directly, or use the make method. 
 *
 * */ 

#include <vector> 
#include "TMutex.h" 
#include <gsl/gsl_spline.h> 

namespace nurfana
{

  class TimeRepresentation; 

  enum InterpolationType
  {
    kInterpDefault, 
    kInterpLinear,
    kInterpGSL 
  }; 


  class Interpolator 
  {
    public: 
      /** Set the input of the interpolator. This theoretically can be expensive
       * depending on what the interpolator has to do */ 
      virtual void setInput(const TimeRepresentation* input)  { input_ = input; }

      /** Interpolate the input at value t */ 
      double eval(double t) const { double answer; evalMany(1,&t,&answer); return answer; }

      /** Interpolate the input at many values t, putting returning result into y. If y is not-NULL, y will be used as the output isntead of allocating new output. In both cases
       *  the result is also returned. If sorted is true, it can be assumed that the input is sorted */ 

      virtual double * evalMany(size_t N, const double * t, double *y = NULL, bool sorted = true) const = 0;

      /** Takes the times from a TimeRepresentation and fills the values */ 
      void eval(TimeRepresentation * out) const; 

      virtual ~Interpolator() { ;}

      /** Create an interpolator based on an interpolator type, and, if applicable, the interpolator options. 
       *  Since each interpolator with options will have incompatible types, a void * is passed that will
       *  be converted to the appropriate type. If kInterpDefault is passed, opt is ignored and instead
       *  the option specified with setDefaultInterpolator is used instead. 
       */

      static Interpolator * make(InterpolationType t = kInterpDefault, void * opt = NULL ); 

      /** Change the default interpolator. If opt is NULL, the default argument (if any ) will be used for an interpolator. 
       **/ 
      static void setDefaultInterpolator(InterpolationType t, void * opt = NULL); 

    protected: 
      const TimeRepresentation * input_; 
  }; 



  /** Linear interpolator. */ 
  class LinearInterpolator : public Interpolator
  {

    public: 
      virtual double * evalMany(size_t N, const double * t, double *y = 0, bool sorted = true) const ;
  };

  /** Interpolator based on GSL classes */ 
  class GSLInterpolator : public Interpolator
  {
    public: 
      GSLInterpolator(const gsl_interp_type * type = gsl_interp_akima); 
      virtual double * evalMany(size_t N, const double * t, double *y = 0, bool sorted = true) const ;
      virtual void setInput(const TimeRepresentation* input);
      virtual ~GSLInterpolator() ; 
    protected: 
      mutable gsl_spline * gsl_s_; 
      const gsl_interp_type * gsl_t_; 
      mutable gsl_interp_accel * gsl_a_; 
      mutable TMutex m_; 
  }; 



}

#endif
