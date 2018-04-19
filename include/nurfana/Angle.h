#ifndef _NURFANA_ANGLE_H 
#define _NURFANA_ANGLE_H

/** Utility methods for dealing with angles and other peroidic thigns */
#include "TMath.h" 

namespace nurfana
{
  namespace angle 
  {

    /* Faster than libmath floor , needed for wrap */ 
    inline int fast_floor(double val) 
    { 
      return  (int) val - (val < (int) val); 
    } 

    inline double wrap(double val, double period, double center) 
    {
      return val - period * fast_floor((val-center+period/2)/period); 
    }

    inline double wrap(double val, double period = 2*TMath::Pi())
    {
      return wrap(val,period, period/2); 
    }

    template <typename T> 
    void unwrap(size_t N, T * vals, T period = 2 *TMath::Pi())
    {
      T adjust = 0; 
      for (size_t i = 1; i < N; i++) 
      {
        double extra = vals[i] - vals[i-1] + adjust; 
        if (extra  > period/2) adjust -= period; 
        else if (extra < -period/2) adjust += period; 
        vals[i] += adjust; 
      }
    }
  }
}


#endif 

