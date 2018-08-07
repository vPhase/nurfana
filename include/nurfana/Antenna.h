#ifndef _NURFANA_ANTENNA_H
#define _NURFANA_ANTENNA_H
#include "TVector3.h" 
#include <complex>

namespace nurfana
{
  /** The base antenna class.
   *
   */ 
  class Antenna
  {

    public: 

      /** The stupidest case */ 
      Antenna(double x, double y, double z)
        : _position(x,y,z),  
          _orientation(0,0,1), 
          _rot(0) { ; } 

      /** Slightly smarter case */ 

      Antenna(const TVector3 & pos, const TVector3 & orientation, double rot) 
        : _position(pos), _orientation(orientation.Unit()), rot(rot) 
      {
      }


      virtual std::complex<double> H(double f, const TVector3 & dir) const ; 
      virtual double G(double f, const TVector3 & dir) { std::abs(H(f,dir)); }
      virtual double ph(double f, const TVector3 & dir) { std::ang(H(f,dir)); }


      const TVector & position() const { return _position; } 



    protected: 
      TVector3 _position; 
      TVector3 _orientation; 
      double _rot; 
      virtual std::complex<double> localH( double f, double local_el, double local_phi = 0) const { return cos(local_el); }
      virtual double localGain(double f, double theta, double phi = 0) { std::abs(H(f,theta_el,phi)); }
      virtual double localPhase(double f, double theta, double phi = 0) { std::ang(H(f,theta_el,phi)); }


     ClassDef(Antenna,1); 

  };


}

#endif

