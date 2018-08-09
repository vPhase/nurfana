#include "nurfana/Response.h" 


namespace nurfana
{

  //////////// Response
  void Response::H(FrequencyRepresentation & freq) const
  {
    for (unsigned i = 0; i < freq.Nf(); i++) 
    {
      freq.updateY()[i] = H(freq.f(i)); 
    }
  }


  std::complex<double> *  Response::HVec(size_t N, const double * f, std::complex<double> * out )  
  {
    if (!out) out = new std::complex<double>[N]; 

    for (unsigned i = 0; i < N; i++) 
    {
      out[i] = H(f[i]); 
    }

    return out; 
  }



  /////////// CompositeResponse
  
  std::complex<double> CompositeResponse::H (double f) const 
  {
    std::complex<double> val = responses_[0]->H(f); 
    for (unsigned i = 1; i < responses_.size(); i++) val *= responses_[i]->H(f); 
    return val; 
  }


  ///////// Analytic Response




}

