#ifndef _NURFANA_ANTENNA_H
#define _NURFANA_ANTENNA_H

/* Antenna descriptions
 *
 * Cosmin Deaconu <cozzyd@kicp.uchicago.edu>
 *
 *
 * */ 


#include "TVector3.h" 
#include <complex>
#include <cmath> 





namespace nurfana
{
  class FrequencyRepresentation; 

  /** The base antenna class.
   *
   *
   */ 
  class Antenna
  {

    public: 

      //This might eventually become more complicated than an enum 
      enum Polarization
      {
        HPOL = 1, 
        VPOL = 2,
        LCP = 4 , 
        RCP = 8
      };


      /** The stupidest case */ 
      Antenna(double x, double y, double z, Polarization pol)
        : position_(x,y,z),  
          orientation_(0,0,1), 
          rot_(0), pol_(pol) { ; } 
          


      /** Slightly smarter case */ 

      Antenna(const TVector3 & pos, const TVector3 & orientation, double rot, Polarization pol) 
        : position_(pos), orientation_(orientation.Unit()), rot_(rot), pol_(pol)
      {
      }


      /** The response in this direction. The default implementation transforms to the local frame
       *  and calls localH*/ 
      virtual std::complex<double> H(double f, const TVector3 & dir) const ; 

      /** fills the frequency representation with a response */ 
      void H(FrequencyRepresentation & f, const TVector3 & dir) const ; 

      /** The realized gain (including impedance mismatch losses) in this direction */ 
      virtual double G(double f, const TVector3 & dir) { return std::abs(H(f,dir)); }

      virtual double ph(double f, const TVector3 & dir) { return std::arg(H(f,dir)); }


      Polarization getPol() const { return pol_; }
      const TVector3 & position() const { return position_; } 
      const TVector3 & orientation() const { return orientation_; } 

      virtual ~Antenna() { ; } 


    protected: 
      TVector3 position_; 
      TVector3 orientation_; 
      double rot_; 
      virtual std::complex<double> localH( double f, double local_el, double local_phi = 0) const { (void) f; (void) local_phi; return cos(local_el); }
      Polarization pol_; 

     ClassDef(Antenna,1); 
  };


}

#endif

