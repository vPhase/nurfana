#include "nurfana/Interpolation2D.h" 


namespace nurfana
{

  GridInterpolator2D::GridInterpolator2D(int nx, int ny, const double * x, const double * y, const double * z, Type t) 
    : t_(t), x_(x,x+nx), y_(y,y+ny), z_(nx*ny), h_(0), hist_ready_(false), dirty_(true)
  {
    if (z) std::copy(z,z+nx*ny, z_.begin()); 

    initGSL(); 
  }


  GridInterpolator2D::GridInterpolator2D(int nx, double xmin, double xmax, int ny, double ymin, double ymax, const double * z, Type t ) 
    : t_(t), x_(nx), y_(ny), z_(nx*ny), h_(0), hist_ready_(false), dirty_(true) 
  {
    if (z) std::copy(z,z+nx*ny, z_.begin()); 
    double dx = (xmax - xmin)/(nx-1); 
    double dy = (ymax - ymin)/(ny-1); 
    for (int i = 0; i < nx; i++) x_[i] = xmin + dx * i; 
    for (int i = 0; i < ny; i++) y_[i] = ymin + dy * i; 
    initGSL(); 


  }

  GridInterpolator2D::GridInterpolator2D(const TH2 & hist, Type t ) 
    : t_(t), x_(hist.GetNbinsX()), y_(hist.GetNbinsY()), z_(hist.GetNbinsX()*hist.GetNbinsY()), hist_ready_(false), dirty_(true)
  {

    for (size_t i = 0; i < x_.size(); i++) x_[i] = hist.GetXaxis()->GetBinCenter(i+1); 
    for (size_t i = 0; i < y_.size(); i++) y_[i] = hist.GetYaxis()->GetBinCenter(i+1); 

    for (size_t i = 0; i < x_.size(); i++)
    {
      for (size_t j = 0; j < y_.size(); j++) 
      {
        z_[i + j * x_.size()] = hist.GetBinContent(i+1,j+1); 
      }
    }

    //we will exactly copy the binning from the histogram in this case... 
     h_ = new TH2D("grid","Grid Interpolator", 10,0,1,10,0,1); 
     h_->SetDirectory(0); 
     h_->SetName("grid"); 
     h_->SetTitle("Grid Interpolator"); 

     if (hist.GetXaxis()->GetXbins()->GetSize()) 
     {
       h_->SetBins(x_.size(), hist.GetXaxis()->GetXbins()->GetArray(), y_.size(), hist.GetYaxis()->GetXbins()->GetArray());

     }
     else
     {
       h_->SetBins(x_.size(), hist.GetXaxis()->GetXmin(), hist.GetXaxis()->GetXmax(), 
                               y_.size(), hist.GetYaxis()->GetXmin(), hist.GetYaxis()->GetXmax()); 

     }

     initGSL(); 
  }

  void GridInterpolator2D::initGSL() 
  {

    s_ = gsl_spline2d_alloc( t_ == Bilinear ? gsl_interp2d_bilinear : gsl_interp2d_bicubic , x_.size(), y_.size()); 
    ax_ = gsl_interp_accel_alloc(); 
    ay_ = gsl_interp_accel_alloc(); 
  }

  GridInterpolator2D::~GridInterpolator2D() 
  {

    gsl_spline2d_free(s_); 
    gsl_interp_accel_free(ax_); 
    gsl_interp_accel_free(ay_); 
    if (h_) delete h_; 
  }


  void GridInterpolator2D::set(int i, int j, double z) 
  {
    dirty_  = true; 
    hist_ready_ = false; 
    z_[i + j * x_.size()] = z; 
  }

  void GridInterpolator2D::setupInterp() const
  {
    if (!dirty_) return; 
    gsl_spline2d_init(s_, &x_[0], &y_[0], &z_[0], x_.size(), y_.size()); 
    dirty_ = false; 
  }

  double * GridInterpolator2D::evalMany(int N, const double * x, const double * y, double * answer) const
  {
    setupInterp(); 
    if (!answer) answer = new double[N]; 
    for (int i = 0; i < N; i++) 
    {
      answer[i] = outOfBounds(x[i],y[i]) ? 0 : gsl_spline2d_eval(s_, x[i],y[i], ax_, ay_); 
    }
    return answer; 
  }

  double * GridInterpolator2D::evalAxis(int N, double a, const double * b, Axis ca, double * answer) const
  {
    setupInterp(); 
    if (!answer) answer = new double[N]; 

    for (int i = 0; i < N; i++)
    {
      answer[i] = ca == XAxis ? ( outOfBounds(a,b[i]) ? 0 : gsl_spline2d_eval(s_, a,b[i], ax_, ay_) ):
                                ( outOfBounds(b[i],a) ? 0 :  gsl_spline2d_eval(s_,b[i],a,ax_,ay_) ); 
    }
    return answer; 
  }

  void GridInterpolator2D::evalHist(TH2 * h) const
  {
    setupInterp(); 

    for (int i = 1; i <= h->GetNbinsX(); i++)
    {
      for (int j = 1; j <= h->GetNbinsY(); j++)
      {
        double x = h->GetXaxis()->GetBinCenter(i); 
        double y = h->GetYaxis()->GetBinCenter(j); 

        h->SetBinContent(i,j,outOfBounds(x,y) ? 0 : gsl_spline2d_eval(s_, x,y,ax_, ay_)); 
      }
    }
  }

  TH2 * GridInterpolator2D::hist() 
  {
    if (hist_ready_) return h_; 

    std::vector<double> xbins(x_.size() +1); 

    if (!h_) 
    {
      // we want the grid points to be in the bin centers. 
      std::vector<double> xbins(x_.size() +1); 
      std::vector<double> ybins(y_.size() +1); 

      //for most of the points, the edges, will be halfway between the centers 
     
      for (size_t i = 1; i< x_.size(); i++) 
      {
        xbins[i] =( x_[i-1] + x_[i])/2; 
      }

      for (size_t i = 1; i< y_.size(); i++) 
      {
        ybins[i] =( y_[i-1] + y_[i])/2; 
      }

      //assuming the last bin spacing for first and last bins
      xbins[0] = x_[0] - (x_[1] - x_[0])/2; 
      ybins[0] = y_[0] - (y_[1] - y_[0])/2; 
      xbins[x_.size()] = x_[x_.size()-1] + (x_[x_.size()-1] - x_[x_.size()-2])/2; 
      ybins[y_.size()] = y_[y_.size()-1] + (y_[y_.size()-1] - y_[y_.size()-2])/2; 
      h_ = new TH2D("grid","Interpolator2D", x_.size(), &xbins[0], y_.size(), &ybins[0]); 
      h_->SetDirectory(0); 
    }

    for (size_t i = 0; i < x_.size(); i++) 
    {
      for (size_t j = 0; j < y_.size(); j++) 
      {
//        printf("%lu %lu %g %g %g\n",i,j,x_[i], y_[j], z_[x_.size() * j +i]); 
        h_->SetBinContent(i+1,j+1, z_[x_.size() * j + i]); 
      }
    }
    h_->SetStats(false); 
    h_->Fill(x_[0], y_[0], 0); 

    hist_ready_ = true; 
    return h_; 
  }


  bool GridInterpolator2D::outOfBounds(double x, double y) const
  {
    return x <= x_[0] || x >= x_[x_.size()-1] || y <= y_[0] || y >= y_[y_.size()-1]; 

  }

}







