#include "nurfana/Antenna.h" 
#include "nurfana/FrequencyRepresentation.h" 




namespace nurfana
{

  void Antenna::H(FrequencyRepresentation & freq, const TVector3 & dir) const
  {
    for (unsigned i = 0; i < freq.Nf(); i++) 
    {
      freq.updateY()[i] = H(freq.f(i),dir); 
    }
  }

  std::complex<double> Antenna::H(double f, const TVector3 & dir) const
  {
    TVector3 v(dir); 
    v.RotateUz(orientation_); 
    return localH(f, v.Theta(), v.Phi()-rot_); 
  }

}
