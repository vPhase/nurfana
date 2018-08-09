#ifndef _NURFANA_MAPPER_H
#define _NURFANA_MAPPER_H


/* A mapper maps some parameter space (e.g. angle, position) to time delays */ 

#include "TVector3.h" 
#include <vector> 

namespace nurfana
{

  class Event; 

  class Mapper
  {

    public: 

      virtual int nDim() const { return nDim_; }

      /* Set the event template for this mapper. The delays
       * between all antennas will be computed here and cached
       * for later use.  */ 
      virtual void setEventTemplate(const Event & event); 

      virtual double getDelay(int i, int j, const double * X)  const= 0; 

      virtual ~Mapper() { ;} 
    protected: 
      Mapper(int ndim, const Event * ev) : nDim_(ndim){ if (ev) setEventTemplate(*ev); } 
      int nDim_; 
      std::vector<TVector3> positions_; 

  }; 


  class ElevationMapper : public Mapper
  {
    public: 

      ElevationMapper (const Event * ev, double n = 1.78) : Mapper(1,ev), n_(n) {;} 
      virtual double getDelay(int i, int j, const double * X) const; 

    private: 
      double n_; 
  }; 

} 


#endif 
