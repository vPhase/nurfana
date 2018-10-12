#include "nurfana/TimeRepresentation.h" 
#include "nurfana/FrequencyRepresentation.h" 
#include "nurfana/FFT.h" 
#include "nurfana/Logging.h"  

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
    : TNamed("time", "Time Representation") 
  {
    interp_ = Interpolator::make(); 
  }

  /* Note that this does NOT copy t_ or y_because we don't want to do that in the even case necessarily */
  TimeRepresentation::TimeRepresentation(const TimeRepresentation &other) 
    : TNamed(other), TAttLine(other), TAttMarker(other), TAttFill(other)
  {
    interp_ = Interpolator::copy(*other.interp_); 
  }

  TimeRepresentation & TimeRepresentation::operator=(const TimeRepresentation &other) 
  {
    TNamed::operator=(other); 
    TAttLine::operator=(other); 
    TAttMarker::operator=(other); 
    TAttFill::operator=(other); 
    interp_ = Interpolator::copy(*other.interp_); 
    return *this; 
  }



  TimeRepresentation::TimeRepresentation(const FrequencyRepresentation &other) 
    : TNamed (other), TAttLine(other), TAttMarker(other), TAttFill(other)
  {
    interp_ = Interpolator::make(); 
  }

  TimeRepresentation & TimeRepresentation::operator=(const FrequencyRepresentation &other) 
  {
    TNamed::operator=(other); 
    TAttLine::operator=(other); 
    TAttMarker::operator=(other); 
    TAttFill::operator=(other); 
    return *this; 
  }


  TimeRepresentation::TimeRepresentation(TimeRepresentation && other) 
   : TNamed(other), TAttLine(other), TAttMarker(other), TAttFill(other)
  {
    interp_ = other.interp_; 
    other.interp_ = 0; 
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

  TGraph * TimeRepresentation::makeGraph(TGraph * g ) const
  {
    if (!g) g = new TGraph(N()); 
    else g->Set(N()); 

    g->SetName(GetName()); 
    g->SetTitle(GetTitle()); 
    TAttLine::Copy(*g); 
    TAttMarker::Copy(*g); 
    TAttFill::Copy(*g); 
    fillT(g->GetX()); 
    fillY(g->GetY()); 
    g->GetXaxis()->SetTitle("t"); 
 
    return g; 
  }

  void TimeRepresentation::Draw(Option_t  * opt ) 
  {
    TGraph * g =makeGraph(); 
    g->SetBit(kCanDelete); 
    g->Draw(opt); 
  }


  double TimeRepresentation::getPeak(unsigned * index, int start, int end, bool abs) const
  {
    if ( start < 0) start += N(); 
    if ( end < 0) start += N(); 

    double max = 0; 
    int imax = 0; 

    for (int i = start; i <= end; i++) 
    {
      double V = abs ? fabs(y(i)) : y(i); 
      if ( V > max) 
      {
        max = V; 
        imax = i; 
      }
    }

    if (index) *index = imax; 
    return max; 
  }

  double TimeRepresentation::getSumV2(int start , int end )  const
  {
    if ( start < 0) start += N(); 
    if ( end < 0) start += N(); 

    double V2 = 0; 
    for (int i = start; i <= end; i++) 
    {
      double V = y(i); 
      V2 += V*V; 
    }

    return V2; 
  }

  double TimeRepresentation::getRMS(int start, int end)  const
  {
    return sqrt(getSumV2(start,end)/ N()); 
  }


  double TimeRepresentation::getMean(int start, int end)  const
  {
    if ( start < 0) start += N(); 
    if ( end < 0) start += N(); 

    double sum = 0; 

    for (int i = start; i <= end; i++) 
    {
      sum += y(i); 
    }

    return sum/N(); 
  }


  TimeRepresentation & TimeRepresentation::operator*(double x)
  {
    for (unsigned i = 0; i < N(); i++) y_[i] *=x; 
    return *this; 
  }

  TimeRepresentation & TimeRepresentation::operator-(double x)
  {
    for (unsigned i = 0; i < N(); i++) y_[i] -=x; 
    return *this; 
  }

  TimeRepresentation & TimeRepresentation::operator+(double x)
  {
    for (unsigned i = 0; i < N(); i++) y_[i] +=x; 
    return *this; 
  }

  TimeRepresentation & TimeRepresentation::operator/(double x)
  {
    double inv = 1./x; 
    for (unsigned i = 0; i < N(); i++) y_[i] *=inv; 
    return (*this); 
  }



  /// Uneven ////

  UnevenRepresentation::UnevenRepresentation(size_t N, const double * t, const double * y, double dt) 
  :  nominal_dt_ (dt) 
  {
    y_.insert(y_.end(), y, y+N); 
    t_.insert(t_.end(), t, t+N); 
  }

  UnevenRepresentation::UnevenRepresentation(const TGraph & g) 
  {
    y_.insert(y_.end(),g.GetY(), g.GetY() + g.GetN()); 
    t_.insert(t_.end(),g.GetX(), g.GetX() + g.GetN()); 
    nominal_dt_ = 0; 
  }

  UnevenRepresentation::UnevenRepresentation(const EvenRepresentation & even) 
    : TimeRepresentation(even) 
  {
    y_.insert(y_.end(), even.y(), even.y()+even.N()); 
    t_.resize(N()); 
    even.fillT(&t_[0]); 
    nominal_dt_ = even.dt(); 
  }

  UnevenRepresentation & UnevenRepresentation::operator=(const EvenRepresentation & even) 
  {
    TimeRepresentation::operator=(even); 
    y_.insert(y_.end(), even.y(), even.y() + even.N()); 
    t_.resize(N()); 
    even.fillT(&t_[0]); 
    nominal_dt_ = even.dt(); 
    return *this; 
  }

  UnevenRepresentation::UnevenRepresentation(const UnevenRepresentation & other) 
    : TimeRepresentation(other) 
  {
    t_ = other.t_; 
    y_ = other.y_; 
    nominal_dt_ = other.nominal_dt_; 
  }

  UnevenRepresentation & UnevenRepresentation::operator=(const UnevenRepresentation & assign) 
  {
    TimeRepresentation::operator=(assign); 
    t_ = assign.t_; 
    y_ = assign.y_; 
    nominal_dt_ = assign.nominal_dt_; 
    return *this; 
  }

  UnevenRepresentation::UnevenRepresentation(UnevenRepresentation && other) 
    : TimeRepresentation(other) 
  {
    t_ = std::move(other.t_); 
    y_ = std::move(other.y_); 
    nominal_dt_ = other.nominal_dt_; 
  }

  bool UnevenRepresentation::amIReallyEven() const 
  {
    if (!t_.size()) return true; 

    double first_dt = t_[1] - t_[0]; 

    for (unsigned i = 2; i < t_.size(); i++) 
    {
      if (t_[i] - t_[i-1] != first_dt) return false; 
    }

    return true; 
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
    double first = u.N() ? u.t(0) : 0; 
    double last = u.N() ? u.t(u.N()-1) : 0; 
    t0_ = first; 
    dt_ = dt ?: u.nominalDT() ?: (last - first) / (u.N() - 1); 
    size_t n = u.N() ?  ceil((last-first)/dt_) : 0; 
    if (n) y_.resize(n); 
    else log::out(log::LOG_WARN, "Warning: Trying to make an EvenRepresentation out of a empty uneven representation!"); 
    if (u.amIReallyEven()  && (u.N() > 1 && dt == u.t(1) - u.t(0)))
    {
      //I can just copy... 
      std::copy(u.y(), u.y() + u.N(), y_.begin()); 
    }
    else
    {
      interp_->evalMany(n, t(), updateY()); 
    }
  }


  EvenRepresentation & EvenRepresentation::operator=(const UnevenRepresentation & u) 
  {
    TimeRepresentation::operator=(u); 
    t_dirty_ = true; 
    double first = u.N() ? u.t(0) : 0; 
    double last = u.N() ? u.t(u.N()-1) : 0; 
    t0_ = first; 
    dt_ = u.nominalDT() ?: (last - first) / (u.N() - 1); 
    size_t n = u.N() ?  ceil((last-first)/dt_) : 0; 

    if (n) y_.resize(n); 
    else log::out(log::LOG_WARN, "Warning: Trying to make an EvenRepresentation out of a empty uneven representation!"); 

    if (u.amIReallyEven()  && (u.N() > 1 && dt_ == u.t(1) - u.t(0)))
    {
      //I can just copy... 
      std::copy(u.y(), u.y() + u.N(), y_.begin()); 
    }
    else
    {
      interp_->evalMany(n, t(), updateY()); 
    }
 
    return *this; 
  }
    

  EvenRepresentation::EvenRepresentation(const FrequencyRepresentation & f) 
    : TimeRepresentation(f), t0_(f.t0()), dt_(1./(f.Nt() * f.df())), t_dirty_(true) 
  {
    y_.resize(f.Nt()); 
    fft::inverse(N(), f.Y(), &y_[0]); 
  }

  EvenRepresentation::EvenRepresentation(const EvenRepresentation & copy)
    : TimeRepresentation(copy) 
  {
    t0_ = copy.t0_; 
    dt_ = copy.dt_; 
    y_ = copy.y_; 
    t_dirty_ = true; 
  }

  EvenRepresentation & EvenRepresentation::operator=(const EvenRepresentation & copy)
  {
    TimeRepresentation::operator=(copy); 
    t0_ = copy.t0(); 
    dt_ = copy.dt(); 
    y_.insert(y_.begin(),copy.y(), copy.y()+copy.N()); 
    t_dirty_ = true; 
    return *this; 
  }


  EvenRepresentation & EvenRepresentation::operator=(const FrequencyRepresentation & f) 
  {
    TimeRepresentation::operator=(f); 
    t0_ = f.t0(); 
    dt_ = 1./(f.Nt() * f.df()); 
    t_dirty_ = true; 
    y_.resize(f.Nt()); 
    fft::inverse(N(), f.Y(), &y_[0]); 
    return *this; 
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

