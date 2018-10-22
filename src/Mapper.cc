#include "nurfana/Mapper.h" 
#include "nurfana/Event.h" 
#include "nurfana/Consts.h" 


namespace nurfana
{

  ///Mapper
  void Mapper::setEventTemplate(const Event & event) 
  {
    ants_.clear(); 
    for (unsigned i = 0; i < event.nChannels(); i++) 
    {
      ants_.push_back(event.channel(i)->antenna()); 
    }
  }

  ///Elevation Mapper
  double ElevationMapper::getDelay(int i, int j, const double * X) const
  {
    // do the dumbest possible thing for now! 
    double theta = *X * TMath::DegToRad(); 
    return (ants_[i]->position().Z()-ants_[j]->position().Z()) * n_/C*sin(theta); 
  }

  double ElevationAzimuthMapper::getDelay(int i, int j, const double * X) const
  {
    double phi = X[0] * TMath::DegToRad(); 
    double theta = X[1] * TMath::DegToRad(); 
    double cos_phi = cos(phi); 
    double sin_phi = sin(phi); 
    double cos_theta = cos(theta); 
    double sin_theta = sin(theta); 
    TVector3 dir ( cos_phi * cos_theta, sin_phi * cos_theta, sin_theta); 
    TVector3 diff( ants_[j]->position() - ants_[i]->position()); 

    return dir.Dot(diff) / (n_*C); 
  }


} 


