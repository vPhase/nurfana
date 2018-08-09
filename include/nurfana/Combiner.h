#ifndef _NURFANA_COMBINER_H
#define _NURFANA_COMBINER_H


namespace nurfana
{
  class Mapper; 

  class Combiner  
  {

    public: 

      Combiner (const Mapper & mapper) : mapper_(mapper); 
      Waveform * combine(const Event & event, const double * X); 
      Waveform * combine(const Event & event, double x, double y =0, double z =0) 
      {
        double X[3] = {x,y,z}; 
        combine(event,X);
      }

    private: 
      const Mapper & mapper_; 

  }; 




}
