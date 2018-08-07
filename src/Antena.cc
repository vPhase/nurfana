#include "Antenna.h" 



namespace nurfana
{
  std::complex<double> Antenna::H(double f, const TVector3 & dir) const
  {
    TVector3 v(dir); 
    v.RotateUz(_orientation); 
    return localH(f, v.Theta(), c.Phi()-_rot); 
  }

}
