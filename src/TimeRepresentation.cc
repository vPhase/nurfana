#include "nurfana/TimeRepresentation.h" 
#include "nurfana/FrequencyRepresentation.h" 
#include "nurfana/FFT.h" 

#include "TGraph.h" 
#include "TH1.h" 
#include <algorithm> 

namespace nurfana
{

  size_t TimeRepresentation::lower_bound(double t, size_t start) const 
  {
    return std::lower_bound(t_.begin()+start, t_.end(), t) - (t_.begin() + start); 
  }


  void TimeRepresentation::setInterpolatorType(InterpolationType t, void * opt) 
  {
    delete interp_; 
    interp_ = Interpolator::make(t,opt); 
  }


  TimeRepresentation::TimeRepresentation() 
  {
    interp_ = Interpolator::make(); 
  }

  TimeRepresentation::TimeRepresentation(const TimeRepresentation &other) 
    : TObject(), TAttLine(other), TAttMarker(other), TAttFill(other) 
  {
    interp_ = Interpolator::make(); 
  }

  TimeRepresentation::TimeRepresentation(const FrequencyRepresentation &other) 
    : TAttLine(other), TAttMarker(other), TAttFill(other) 
  {
    interp_ = Interpolator::make(); 
  }



  TimeRepresentation::~TimeRepresentation() 
  {
    delete interp_; 
  }

  void TimeRepresentation::fillY(double * vals) const 
  {
    std::copy(y_.begin(), y_.end(), vals); 
  }

  void TimeRepresentation::fillT(double * vals) const 
  {
    std::copy(t(),t() + N(), vals); 
  }


  void TimeRepresentation::Draw(Option_t  * opt ) 
  {
    TGraph * g = new TGraph(N()); 
    g->SetBit(kCanDelete); 
    g->GetXaxis()->SetTitle("t"); 
    TAttLine::Copy(*g); 
    TAttMarker::Copy(*g); 
    TAttFill::Copy(*g); 
    fillT(g->GetX()); 
    fillY(g->GetY()); 
    g->Draw(opt); 
  }


  /// Uneven ////

  UnevenRepresentation::UnevenRepresentation(size_t N, const double * t, const double * y) 
  {
    y_.insert(y_.end(), y, y+N); 
    t_.insert(t_.end(), t, t+N); 
  }

  UnevenRepresentation::UnevenRepresentation(const TGraph & g) 
  {
    y_.insert(y_.end(),g.GetY(), g.GetY() + g.GetN()); 
    t_.insert(t_.end(),g.GetX(), g.GetX() + g.GetN()); 
  }

  UnevenRepresentation::UnevenRepresentation(const EvenRepresentation & even) 
    : TimeRepresentation(even) 
  {
    y_.insert(y_.end(), even.y(), even.y()+even.N()); 
    t_.resize(N()); 
    even.fillT(&t_[0]); 
  }



  /// Even ///

  EvenRepresentation::EvenRepresentation(size_t N, const double *y, double dt, double t0) 
   : t0_(t0), dt_(dt), t_dirty_(true)
  {
    y_.insert(y_.end(), y, y + N); 
  }

  EvenRepresentation::EvenRepresentation(const UnevenRepresentation & u, double dt) 
    : TimeRepresentation(u), t_dirty_(true) 
  {
    double first = u.t(0); 
    double last = u.t(u.N()-1); 
    t0_ = first; 
    dt_ = dt ?: (last - first) / (u.N() - 1); 
    size_t n = ceil((last-first)/dt_); 
    y_.resize(n); 
    interp_->evalMany(n, t(), updateY()); 

  }

  EvenRepresentation::EvenRepresentation(const FrequencyRepresentation & f) 
    :  t0_(f.t0()), dt_(1./(f.Nt() * f.df())), t_dirty_(true) 
  {
    y_.resize(f.Nt()); 
    fft::inverse(N(), f.Y(), &y_[0]); 
  }

  const double * EvenRepresentation::t() const 
  {
    if (!t_dirty_) return &t_[0]; 
    TLockGuard l(&m_); 
    t_.resize(N()); 
    fillT(&t_[0]); 
    t_dirty_ = false; 
    return &t_[0]; 
  }

  void EvenRepresentation::fillT(double * vals) const 
  {
    for (size_t i = 0; i < N(); i++) vals[i] = t0_ + dt_ * i; 
  }

}

