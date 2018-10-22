#include "nurfana/Response.h" 
#include "TMath.h" 
#include "TRandom.h" 
#include "Math/Polynomial.h" 


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


  __attribute__((optimize("-ffast-math")))
  std::complex<double> AnalyticResponse::H(double f) const
  {

    std::complex<double> s(0,f * 2*TMath::Pi()) ; 
    if (init_with_ == ZPK) 
    {
      std::complex<double> val (g_,0); 
      //TODO, there is probably a more numerically stable way to calculate this
      for (auto z : z_) val *=(s-z);  
      for (auto p : p_) val /=(s-p);  
      return val; 
    }

    //horner's rule for the numerator
    int i = a_.size()-1; 
    std::complex<double> num = a_[i]; 
    while (i > 0) num = num*s+a_[i-1]; 

    //horner's rule for the denominator
    i = b_.size()-1; 
    std::complex<double> denom = b_[i]; 
    while (i > 0) denom = denom * s + b_[i-1]; 

    return num/denom; 
  }


  const double * AnalyticResponse::getACoeffs() const 
  {
    if (!a_.size()) zpk2ba(); 
    return &a_[0]; 
  }

  const double * AnalyticResponse::getBCoeffs() const 
  {
    if (!b_.size()) zpk2ba(); 
    return &b_[0]; 
  }

  double AnalyticResponse::getGain() const
  {
    if (g_ == 0) ba2zpk(); 
    return g_; 
  }

  //almost equivalent to matlab poly. (except indices are reversed so that the index matches the order)
  // 
  static void poly(const std::vector<std::complex<double>> & zeroes, std::vector<std::complex<double>>  & coeffs)
  {


    //Start out with (x - zero_0)
    coeffs[0] = -zeroes[0]; 
    coeffs[1] = std::complex<double>(1,0); 

    for (unsigned int i = 1; i < zeroes.size(); i++)
    {

      //Multiply  by each (x - zero_i)
      for (unsigned int j = i+1;;  j--)
      {
        coeffs[j] = (j > 0 ? coeffs[j-1] : 0) - (j < i+1 ? coeffs[j] * zeroes[i] : 0); 
        if (j == 0) break; 
      }
    }
  }

  void AnalyticResponse::zpk2ba() const
  {
    std::vector<std::complex<double> > bpoly(z_.size()+1); 
    std::vector<std::complex<double> > apoly(p_.size()+1); 

    poly(z_,bpoly); 
    poly(p_,apoly); 

    b_.resize(bpoly.size());
    a_.resize(apoly.size());

    for (unsigned i = 0; i < b_.size(); i++) 
    {
      b_[i] = std::real(g_ * bpoly[z_.size()-i]);
    }

    for (unsigned i = 0; i < a_.size(); i++) 
    {
      a_[i] = std::real(apoly[p_.size()-i]);
    }
  }


  void AnalyticResponse::ba2zpk() const
  {
    
    ROOT::Math::Polynomial num(b_.size()-1);
    ROOT::Math::Polynomial denom(a_.size()-1);
    num.SetParameters(&b_[0]); 
    denom.SetParameters(&a_[0]); 

    z_ = num.FindRoots(); 
    p_ = denom.FindRoots(); 

    //we can calculate the gain by evaluating the polynomials anywhere that's not a zero or pole... 
    std::complex<double> s(0,0); 
    while( std::find(z_.begin(), z_.end(),  s) != z_.end() || std::find(p_.begin(), p_.end(), s) != p_.end() ) 
    {
      s+= gRandom->Uniform(0,1); 
    }

    double poly_val = num(s.real())/denom(s.real()); 

    std::complex<double> zero_product = (s-z_[0]); 
    std::complex<double> pole_product = (s-p_[0]); 
    for (unsigned i = 1; i < z_.size(); i++) zero_product *= (s-z_[i]);
    for (unsigned i = 1; i < p_.size(); i++) pole_product *= (s-z_[i]);

    g_ = std::real(poly_val *pole_product/zero_product); 

  }







}

  ///
