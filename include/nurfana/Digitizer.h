#ifndef _NURFANA_DIGITIZER_H 
#define _NURFANA_DIGITIZER_H 

/** This class encodes information
 * about different digitizers */ 


namespace nurfana
{

  class Digitizer  
  {
    public: 

      Digitizer(const char * name, double rate, bool even ) 
        : name_(name), rate_(rate), dt_(1./rate), even_(even) 
      {

      } 

      const Digitizer & IRS2(); 
      const Digitizer & PhasedArray(); 

      const char * getName() { return name_; }
      double getSampleRate() const { return rate_; } 
      double getNominalDt() const { return dt_; } 
      bool  isEvenlySampled() const { return even_; } 

    private: 
      const char * name_; 
      double rate_; 
      double dt_; 
      bool even_; 
  }; 


}

#endif 
