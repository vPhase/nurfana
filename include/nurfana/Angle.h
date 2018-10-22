#ifndef _NURFANA_ANGLE_H 
#define _NURFANA_ANGLE_H

/** 
 *  Utility methods for dealing with angles and other peroidic thigns 
 *  
 *  Cosmin Deaconu <cozzyd@kicp.uchicago.edu> 
 *
 **/

/****************************************************************************\
*                                                                            *
*                                                                            *
*    nurfana is free software: you can redistribute it and/or modify         *
*    it under the terms of the GNU General Public License as published by    *
*    the Free Software Foundation, either version 3 of the License, or       *
*    (at your option) any later version.                                     *
*                                                                            *
*    nurfana is distributed in the hope that it will be useful,              *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
*    GNU General Public License for more details.                            *
*                                                                            *
*    You should have received a copy of the GNU General Public License       *
*    along with nurfana.  If not, see <https://www.gnu.org/licenses/>.       *
*                                                                            *
\****************************************************************************/

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

