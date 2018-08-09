#ifndef _NURFANA_CORRELATOR_H
#define _NURFANA_CORRELATOR_H

#include "nurfana/Mapper.h" 
#include "nurfana/Event.h" 

namespace nurfana
{

  class Correlator
  {

    public: 

      Correlator( const Mapper & mapper, unsigned *N, const double *xmin, const double *xmax); 
      Correlator( const Mapper & mapper, unsigned N, double xmin, double xmax) 
      void correlate(const Event &); 
      const TH1 * getHist() const { return h_; }
    private: 
      TH1 * h_; 
      const Mapper * m_;
  }; 
}


#endif
