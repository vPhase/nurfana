#include "nurfana/Mapper.h" 
#include "nurfana/Event.h" 
#include "nurfana/Consts.h" 


namespace nurfana
{

  void Mapper::setEventTemplate(const Event & event) 
  {
    positions_.clear(); 
    for (unsigned i = 0; i < event.nChannels(); i++) 
    {
      positions_.emplace_back(event.channel(i)->antenna()->position()); 
    }
  }

  double ElevationMapper::getDelay(int i, int j, const double * X) const
  {
    // do the dumbest possible thing for now! 
    double theta = *X * TMath::DegToRad(); 
    return (positions_[i].Z()-positions_[j].Z()) * n_/C*sin(theta); 
  }

} 


