#include "nurfana/FFT.h" 
#include <fftw3.h> 
#include "TMutex.h" 
#include <map> 


static const char * wisdom = nullptr; 
static TMutex fftw_lock; 

static const int NALIGN = 8; 

struct fft_setup
{
  fft_setup(size_t n) 
  {

    N  = n;
    mem = fftw_malloc(sizeof(double) * (N + N % NALIGN) + (N/2 + 1) * sizeof(fftw_complex)); 
    y = (double *) mem; 
    Y = (fftw_complex *) (y + N + N % NALIGN); 

    TLockGuard l(&fftw_lock); 
    forward = fftw_plan_dft_r2c_1d(N, y, Y,FFTW_MEASURE | FFTW_DESTROY_INPUT); 
    inverse = fftw_plan_dft_c2r_1d(N, Y, y,FFTW_MEASURE | FFTW_DESTROY_INPUT); 
  }

  size_t N; 
  fftw_plan forward; 
  fftw_plan inverse; 
  void * mem; 
  double * y; 
  fftw_complex * Y; 
}; 

static thread_local std::map<size_t, fft_setup * > setups; 

__attribute__((destructor)) 
static void on_exit() 
{
  if (wisdom) fftw_export_wisdom_to_filename(wisdom); 
}

static fft_setup & setup(size_t N) 
{
  if (setups.count(N)) return *setups[N]; 
  setups[N] = new fft_setup(N); 
  return *setups[N]; 
}

namespace nurfana
{
  namespace fft
  {
    void setWisdomFile(const char * f) 
    { 
      wisdom = f; 
      fftw_import_wisdom_from_filename(f); 
    }

    int forward(size_t N, const double * y, std::complex<double> *Y) 
    {
      fft_setup & s = setup(N); 
      memcpy(s.y, y, N * sizeof(double)); 
      fftw_execute(s.forward); 
      memcpy(Y, s.Y, (N/2 + 1) * sizeof(fftw_complex)); 
    }

    int inverse(size_t N, const std::complex<double> * Y, double * y) 
    {
      fft_setup & s = setup(N); 
      memcpy(s.Y, Y, (N/2 + 1) * sizeof(fftw_complex)); 
      fftw_execute(s.inverse); 
      memcpy(y, s.y, N * sizeof(double)); 
    }
  }
}

