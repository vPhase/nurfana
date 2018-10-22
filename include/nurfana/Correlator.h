#ifndef _NURFANA_CORRELATOR_H
#define _NURFANA_CORRELATOR_H

#include "nurfana/Mapper.h" 
#include "nurfana/Event.h" 
#include <vector> 

namespace nurfana
{

  class Correlator
  {

    public: 

      /*A correlator range corresponds to the range of one of the axes of a mapper */ 
      struct Range
      {
        unsigned num_steps;
        double min; 
        double max; 
        const char * name; 
        bool periodic; 

        Range(unsigned num_steps, double min, double max, const char * name = "",  bool periodic = false) 
         : num_steps(num_steps), 
           min(min), 
           max(max), 
           name(name),
           periodic(periodic) 
        {;} 
      }; 

      Correlator( const Mapper & mapper, int nranges, const Range * ranges); 

      /** Sets the channels used in the correlation. 
       * If they are not set, all of them will be used */ 
      void setChannels (int nchan, const int * channels) { c_.assign(channels, channels+nchan) ; }
      std::vector<int>  & channels() { return c_; } //so you can use assign or whatever
      void correlate(const Event & ev); 
      const TH1 * getHist() const { return h_; }
    private: 
      TH1 * h_; 
      std::vector<Range> r_; 
      std::vector<int> c_; 
      const Mapper * m_;
  }; 
}


#endif
