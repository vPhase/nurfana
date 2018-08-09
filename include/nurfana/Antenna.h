#ifndef _NURFANA_ANTENNA_H
#define _NURFANA_ANTENNA_H
#include "TVector3.h" 
#include <complex>
#include <cmath> 

namespace nurfana
{
  /** The base antenna class.
   *
   */ 
  class Antenna
  {

    public: 

      /** The stupidest case */ 
      Antenna(double x, double y, double z, int type)
        : position_(x,y,z),  
          orientation_(0,0,1), 
          rot_(0), type_(type) { ; } 
          


      /** Slightly smarter case */ 

      Antenna(const TVector3 & pos, const TVector3 & orientation, double rot) 
        : position_(pos), orientation_(orientation.Unit()), rot_(rot) 
      {
      }


      virtual std::complex<double> H(double f, const TVector3 & dir) const ; 
      virtual double G(double f, const TVector3 & dir) { return std::abs(H(f,dir)); }
      virtual double ph(double f, const TVector3 & dir) { return std::arg(H(f,dir)); }


      int getType() const { return type_; }
      const TVector3 & position() const { return position_; } 

      virtual ~Antenna() { ; } 


    protected: 
      TVector3 position_; 
      TVector3 orientation_; 
      double rot_; 
      virtual std::complex<double> localH( double f, double local_el, double local_phi = 0) const { (void) f; (void) local_phi; return cos(local_el); }
      int type_; 


     ClassDef(Antenna,1); 

  };


}

#endif

