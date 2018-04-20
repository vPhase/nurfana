#include "Math/Interpolator.h" 
#include "TMutex.h" 

#include "nurfana/Interpolation.h" 
#include "nurfana/TimeRepresentation.h" 
#include <algorithm> 

static TMutex setter; 

namespace nurfana 
{

  static InterpolationType default_interpolation; 
  static void * default_opt; 

  Interpolator * Interpolator::make(InterpolationType t, void * opt) 
  {

    switch(t)
    {
      case kInterpDefault: 
        t = default_interpolation;
        opt = default_opt; 
      case kInterpGSL: 
        return new GSLInterpolator;
      case kInterpLinear:
        return new LinearInterpolator;
      default: 
        return make(kInterpDefault,opt); 
    }
  }

  void setDefaultInterpolator(InterpolationType t, void * opt)
  {
    TLockGuard l(&setter); //make sure these are set atomically 
    default_interpolation = t; 
    default_opt = opt; 
  }


  void Interpolator::eval(TimeRepresentation * out) const
  { 
    evalMany (out->N(), out->t(), out->updateY(),true);
  } 

  double * LinearInterpolator::evalMany(size_t N, const double * t, double * y, bool sorted) const
  {
    if (!y) y = new double[N]; 


    size_t last_bound = 0; 
    for (size_t i = 0; i < N; i++) 
    {
      size_t lower_bound = input_->lower_bound(t[i],last_bound); 

      if (lower_bound == 0) 
      {
        y[i] = input_->y(0); 
      }
      else if (lower_bound == input_->N()) 
      {
        y[i] = input_->y(input_->N()-1);
      }
      else
      {
        double t0 = t[i] - input_->t(lower_bound-1); 
        y[i] = input_->y(lower_bound-1) + t0 * (input_->y(lower_bound) - input_->y(lower_bound-1)) / input_->dt(lower_bound-1); 
      }

      if (sorted) last_bound = lower_bound; 
    }

    return y; 
  }

  GSLInterpolator::GSLInterpolator(const gsl_interp_type * type)
  {
    if (!type) type = gsl_interp_akima; 
    gsl_t_ = type; 
    gsl_s_  = NULL;
    gsl_a_  = NULL;

  }

  GSLInterpolator::~GSLInterpolator() 
  {
    if (gsl_s_) 
    {
      gsl_spline_free(gsl_s_); 
      gsl_interp_accel_free(gsl_a_); 
    }
  }

  double * GSLInterpolator::evalMany(size_t N, const double * t, double *y, bool sorted) const
  {
    if (!y) y = new double[N]; 

    double *t_sorted = 0; 

    if (!sorted) 
    {
      t_sorted = new double[N]; 
      std::partial_sort_copy(t,t+N, t_sorted, t_sorted+N); 
      t = t_sorted; 
    }

    TLockGuard lock(&m_); //probably the accelerator is not threadsafe 

    //initialize the spline if it's not initialized already 
    if (!gsl_s_) 
    {
      if (input_->N() < gsl_interp_type_min_size(gsl_t_))
      {
         memset(y,0,N*sizeof(double)); 
         return y; 
      }

      if (!gsl_a_) gsl_a_ = gsl_interp_accel_alloc(); 
      else gsl_interp_accel_reset(gsl_a_); 

      gsl_s_ = gsl_spline_alloc(gsl_t_, input_->N()); 
      gsl_spline_init(gsl_s_, input_->t(), input_->y(), input_->N()); 
    }

    for (size_t i = 0; i < N; i++)
    {
      y[i] = gsl_spline_eval(gsl_s_, t[i], gsl_a_); 
    }

    if (t_sorted) delete [] t_sorted; 

    return y; 
  }

  void GSLInterpolator::setInput(const TimeRepresentation * in)
  {
    Interpolator::setInput(in); 

    if (input_->N() < gsl_interp_type_min_size(gsl_t_))
    {
        fprintf(stderr,"WARNING: Input smaller than minimum size interp type, %u. Will zero output.\n",
                gsl_interp_type_min_size(gsl_t_)); 
    }
 
    if (gsl_s_) 
    {
      gsl_spline_free(gsl_s_); 
    }
    gsl_s_ = 0; 
  }
}




