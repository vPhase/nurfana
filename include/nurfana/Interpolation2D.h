#ifndef _NURFANA_INTERPOLATION_2D_H
#define _NURFANA_INTERPOLATION_2D_H

#include "TH2.h" 
#include <vector>
#include "gsl/gsl_spline2d.h" 
#include "gsl/gsl_interp2d.h" 

namespace nurfana
{

  /** Interpolates on an x-y grid
   *
   * It needn't be even, but it must be regular
   *
   * NOT THREAD SAFE (you must lock access to eval ) 
   *
   **/ 
  class GridInterpolator2D 
  {
    public: 
      enum Type
      {
        Bilinear, 
        Bicubic
      }; 
      enum Axis
      {
        XAxis,
        YAxis
      }; 

      GridInterpolator2D(int nx, int ny, const double * xvals, const double * yvals, const double * z = 0, Type t = Bicubic); 
      GridInterpolator2D(int nx, double xmin, double xmax, int ny, double ymin, double ymax, const double * z = 0, Type t= Bicubic); 
      GridInterpolator2D(const TH2 & hist, Type t = Bicubic ); 

      virtual ~GridInterpolator2D(); 


      /** This is zero indexed, unlike the histogram */ 
      void set(int xindex, int yindex, double z); 

      double * evalMany(int N, const double * x, const double * y, double * answer = 0) const; 
      double eval(double x, double y) const { double answer=0; evalMany(1, &x,&y,&answer); return answer; } 
      double * evalAxis(int N, double a, const double * b, Axis constant_axis = XAxis, double * answer = 0) const; 

      void evalHist(TH2 * h) const; 


      TH2 * hist(); 


    private: 
       void initGSL(); 
       bool outOfBounds(double x, double y) const; 
       void setupInterp() const; 
       Type t_; 
       mutable gsl_spline2d *s_; 
       mutable gsl_interp_accel *ax_; 
       mutable gsl_interp_accel *ay_; 
       std::vector<double> x_; 
       std::vector<double> y_; 
       std::vector<double> z_; 
       TH2 * h_; 
       bool hist_ready_;
       mutable bool dirty_; 


  }; 
}


#endif
