#include "TMath.h" 
#include "TGraph.h" 
#include "TAxis.h" 

#include "nurfana/FrequencyRepresentation.h" 
#include "nurfana/TimeRepresentation.h" 
#include "nurfana_private.h" 
#include "nurfana/FFT.h" 
#include "nurfana/Angle.h" 


namespace nurfana
{


  FrequencyRepresentation::FrequencyRepresentation(size_t N, 
                                                   const std::complex<double> * Y, 
                                                   double df, double t0) 
    : Nt_(N), t0_(t0), df_(df), Y_(Y, Y + N/2+1)
  {

    invalidate(); 

  }

  FrequencyRepresentation::FrequencyRepresentation(const EvenRepresentation & even) 
    : TAttFill(even), TAttLine(even), TAttMarker(even), Nt_(even.N()), t0_(even.t0()), df_( 1./(even.N() * even.dt())), Y_(even.N()/2+1)
  {
     fft::forward(Nt_, even.y(), &Y_[0]); 
     invalidate(); 
  }


  void FrequencyRepresentation::setNt(size_t N) 
  {
    Nt_ = N; 
    Y_.resize(N/2+1, std::complex<double>(0,0)); 
    invalidate(); 
  }


  void FrequencyRepresentation::f(double * dest) const 
  {
    for (size_t i = 0; i < Nf(); i++) dest[i] = f(i); 
  }


  double FrequencyRepresentation::mag(size_t i, bool dB) const
  {
   return dB ? 10 * TMath::Log10(std::abs(Y(i))) : std::abs(Y(i)); 
  }

  void FrequencyRepresentation::mag(double * dest, bool dB) const
  {
    for (size_t i = 0; i < Nf(); i++) dest[i] = mag(i,dB); 
  }

  double FrequencyRepresentation::norm(size_t i, bool dB) const
  {
   return dB ? 20 * TMath::Log10(std::norm(Y(i))) : std::norm(Y(i)); 
  }

  void FrequencyRepresentation::norm(double * dest, bool dB) const
  {
    for (size_t i = 0; i < Nf(); i++) dest[i] = norm(i,dB); 
  }

  double FrequencyRepresentation::phase(size_t i, bool unwrapped) const
  {
    if (!unwrapped) 
    {
      return std::arg(Y(i)); 
    }
    calc_unwrapped(); 
    return unwrapped_phase_[i]; 
  }


  void FrequencyRepresentation::phase(double * dest, bool unwrapped) const
  {

    for (size_t i = 0; i < Nf(); i++) dest[i] = phase(i,unwrapped); 
  }

  double FrequencyRepresentation::groupDelay(size_t i) const 
  {
    calc_delay(); 
    return group_delay_[i]; 
  }

  void FrequencyRepresentation::groupDelay(double * dest) const
  {
    for (size_t i = 0; i < Nf(); i++) dest[i] = groupDelay(i); 
  }

  void FrequencyRepresentation::calc_unwrapped() const
  {
    if (!unwrapped_invalid_) return; 
    TLockGuard l(&m_unwrap_); 
    unwrapped_phase_.resize(Nf()); 
    phase(&unwrapped_phase_[0], false); 
    angle::unwrap(Nf(), &unwrapped_phase_[0]); 
    unwrapped_invalid_ = false; 
  }

  void FrequencyRepresentation::calc_delay() const
  {
    if (!delay_invalid_) return; 
    TLockGuard l(&m_delay_); 
    group_delay_.resize(Nf()); 
    phase(&group_delay_[0],true); 
    double last = group_delay_[0]; 
    double dw = df() * 2 * TMath::Pi(); 
    for (size_t i = 1; i < Nf(); i++) 
    {
      double current = group_delay_[i]; 
      group_delay_[i-1] = (last-current)/dw; 
      last = current; 
    }
    group_delay_[Nf()-1] = 0; 
    delay_invalid_ = true; 
  } 

  void FrequencyRepresentation::Draw(Option_t * option)
  {
    TGraph * g = new TGraph(Nf()); 
    TAttLine::Copy(*g); 
    TAttMarker::Copy(*g); 
    TAttFill::Copy(*g); 
    f(g->GetX()); 
    g->GetXaxis()->SetTitle("Frequency"); 
    g->SetBit(kCanDelete); 
    std::string opt(option);

    if (consume(opt,"GROUPDELAY"))
    {
      groupDelay(g->GetY()); 
      g->GetYaxis()->SetTitle("Group Delay"); 
    }
    else if (consume(opt,"PHASE"))
    {
      phase(g->GetY(), consume(opt,"UNWRAP")); 
      g->GetYaxis()->SetTitle("Phase"); 
    }
    else if (consume(opt,"MAG"))
    {
      bool dB = consume(opt,"DB");
      mag(g->GetY(), dB); 
      g->GetYaxis()->SetTitle(dB ? "Magnitude (dB)" : "Magnitude");
    }
    else if (consume(opt,"NORM") || consume(opt,"POWER"))
    {
      bool dB = consume(opt,"DB");
      norm(g->GetY(), dB); 
      g->GetYaxis()->SetTitle(dB ? "Power (dB)" : "Power");
    }

    g->Draw(option); 
  }


}
