#include "nurfana/Antenna.h" 



namespace nurfana
{
  std::complex<double> Antenna::H(double f, const TVector3 & dir) const
  {
    TVector3 v(dir); 
    v.RotateUz(orientation_); 
    return localH(f, v.Theta(), v.Phi()-rot_); 
  }

}
