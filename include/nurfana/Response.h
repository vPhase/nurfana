#ifndef _NURFANA_RESPONSE_H
#define _NURFANA_RESPONSE_H

#include <vector> 
#include "FrequencyRepresentation.h" 


/** Defines the system response of something. There are a lot of ways this can
 * be expressed, but we use a frequency-based basis for the most part. 
 *
 * Note that this is meant for the response of a signal... the antenna has its own response!
 *   
 */ 
namespace nurfana {


  class Response
  {

    public: 

      /* Returns the complex response at a frequency */ 
      virtual std::complex<double> H( double f) const  { (void) f;return 1; } 

      virtual double G( double f) const  { return std::abs(H(f)); }
      virtual double ph( double f) const  { return std::arg(H(f)); }

      /* Fills the frequency representation with the response */ 
      virtual void H(FrequencyRepresentation & freq) const;  

      /* Get the response at arbitrary places */ 
      virtual std::complex<double> *  HVec(size_t N, const double * f, std::complex<double> * out = 0);  

      /** Returns the real part of the frequency response at a frequency */ 
      virtual double Hre(double f) const { return std::real(H(f)); }
      virtual double Him(double f) const { return std::imag(H(f)); }
      
      virtual ~Response() { ; } 

  }; 


  class CompositeResponse : public Response
  {

      public: 

        CompositeResponse(const Response * r) {addResponse(r); } 
        void addResponse(const Response * r) { responses_.push_back(r);}

        virtual std::complex<double> H( double f) const ;  

    protected: 
      std::vector<const Response *> responses_; 
  }; 

  /** An analytic response function from 
   * poles and zeroes (in the s plane); 
   *  This allows exact modeling of e.g. analog filters
   * 
  */ 
  class AnalyticResponse : public Response
  {
    public: 
//      AnalyticResponse(unsigned npoles, const std::complex<double> * poles,
 //                      unsigned nzeroes = 0, const std::complex<double> * zeroes = 0, double gain = 1 ) { ; } 

      std::complex<double> H(double f) const { (void) f;  return 0; } 
     private: 
      int npoles; 
      int nzeroes; 
      std::vector<std::complex<double> > poles; 
      std::vector<std::complex<double> > zeroes; 
      double gain; 

  }; 


  /** This interpolates a frequency domain response.*/
  class InterpolatingResponse  : public Response
  {
    public: 

      /* Here we start from a frequency representation with evenly spaced frequencies */ 
      InterpolatingResponse(const FrequencyRepresentation & f) 
        : _f(f) {;} 
      std::complex<double> H(double f) const; 
      
      enum TDPhaseOption
      {
        KEEP, 
        MAX, 
        MIN,
        MAXENV
      }; 

      /* Here we start from a time-domain (an impulse response) */ 
//      InterpolatingResponse(const TimeRepresentation & t, TDPhaseOption phase_option = KEEP); 

    private: 
      FrequencyRepresentation _f; 


  }; 


}


#endif 
