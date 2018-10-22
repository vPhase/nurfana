#ifndef _NURFANA_MAPPER_H
#define _NURFANA_MAPPER_H


/* A mapper maps some parameter space (e.g. angle, position) to time delays.
 * This can then be used for correlations and things like that 
 *
 * */ 

#include "TVector3.h" 
#include "nurfana/Event.h" 
#include <vector> 

namespace nurfana
{

  class Event; 


  class Mapper
  {

    public: 

      unsigned ndim() const { return ndim_; } 
      unsigned nChannels() const { return ants_.size(); } 

      /* Set the event template for this mapper. The delays between all
       * antennas as well as what antennas can be used will be computed here
       * and cached for later use.  */ 
      virtual void setEventTemplate(const Event & event); 


      /** Returns the delay for the channel pair at this position */ 
      virtual double getDelay(int i, int j, const double * X)  const= 0; 

      /** Returns true if this pair of channels may be used together*/ 
      virtual bool   canUsePair(int i, int j)  const { (void) i; (void) j; return true;}

      /** Returns true if this antenna can be used for this part of phase space*/ 
      virtual bool   canUseAntenna(int i, const double * X)  const  { (void) i; (void) X;  return true; }

      /** destroys the mapper */
      virtual ~Mapper() { ;} 

    protected: 

      Mapper(unsigned ndim, const Event * event) : ndim_(ndim) { if (event) setEventTemplate(*event) ; }
      std::vector<const Antenna *> ants_; 
      unsigned ndim_; 

  }; 



  /** Assumes antennas are azimuthally symmetric (i.e. a string) */ 

  class ElevationMapper : public Mapper
  {
    public: 

      ElevationMapper (const Event * ev = 0, double n = 1.78) : Mapper(1,ev), n_(n) {;} 
      virtual double getDelay(int i, int j, const double * X)  const; 

    private: 
      double n_; 
  }; 


  class ElevationAzimuthMapper : public Mapper
  {

    public:
      ElevationAzimuthMapper(const Event * ev =0, double n = 1.78) : Mapper(2,ev), n_(n) {;} 
      virtual double getDelay(int i, int j, const double * X)  const; 
    private: 
      double n_; 

  }; 

  /* Assumes antennas are azimuthally symmetric (i.e. a string) */
  class IceRZMapper : public Mapper
  {
   



  };


  class IceRZPhiMapper : public Mapper
  {




  };




} 


#endif 
