#include "nurfana/Raytracing.h" 
#include "nurfana/IceModel.h" 
#include "cubature/cubature.h" 
#include "TGraph.h" 


namespace nurfana
{

  namespace raytracer
  {

  struct integrand_aux_t 
  {
    const ice::Model * m; 
    double alpha2; 
    bool flat; 
    bool just_d_d; 
    double R; 
    const std::vector<double> & atten_fs; 
  }; 

  static int integrand(unsigned ndim, size_t npt, const double *x, void * aux, 
                       unsigned fdim, double *f) 
  {
    assert(ndim == 1); //integrating only over z
    integrand_aux_t * a= (integrand_aux_t *) aux; 
    int natt = a->atten_fs.size();
    assert(fdim == 1 || fdim == 3 + natt); //we calculate time, surface distance, chord length, and attenuation, or just ditsance

    for (int ipt = 0; ipt < npt; ipt++) 
    {
      double z = x[ipt]; 
      double n = a->m->n(z); 
      double n2 = n*n; 

      double  d_d, d_s;

      if (flat) 
      {
        double denom2 = n2-a->alpha2; 
        double denominv = pow(denom2,-0.5); 
        double d_d =denom_inv; 
        double d_s = fdim > 1 ? n * denom_inv : 0; 
      }
      else
      {

        double one_plus_2z_over_R = 2*z/a->R +1; 
        double denom2 = n2-a->alpha2 * one_plus_2z_over_R; 
        if (denom2 < 0) return 1; 
        double denominv = pow(denom2,-0.5); 
        double d_d = one_plus_2z_over_R * denominv; 
        double d_s = fdim > 1 ? n * denominv : 0; 
      }

      f[ipt] = fdim > 1 ?  n*d_s : d_d; ; 
      if (fdim > 1) 
      {
        f[npts+ipt] = d_d; 
        f[2*npts+ipt] = d_s; 

        for (int iat = 0; iat < natt; iatt++) 
          f[(3 + iat)*npts+ipt] = d_s / a->m->attenuation(z, a->atten_fs[iat]);
      }
    }
    
    return 0; 
  }



  int IntegratingRaytracer::computeRay(double z, double alpha, double *t, double *d, double *s, double * A, unsigned nturns, const double * turns, double * save_turns) 
  {
    std::vector<double> lower_limits(nturns+1); 
    std::vector<double> upper_limits(nturns+1); 

    lower_limits[0] = z0; 
    upper_limits[nturns] = z; 

    //figure out where the turns are if they're not provided
    if (!turns)  
    {
      bool up = z < z0; 
      double next_z = z0; 
      for (int iturn = 0; i turn < nturns; i++) 
      {
        next_z =  findTurningPoint(alpha, next_t, up); 
        if (save_turns) save_turns[iturn] = next_z; 
        upper_limits[iturn] = next_z
        lower_limits[iturn+1] = next_z; 
        up = != up; 
      }
    } 
    else
    {
      for (int iturn = 0; iturn < nturns; i++) 
      {
        upper_limits[iturn] = turns[iturn]; 
        lower_limits[iturn+1] = turns[iturn]; 
      }
    }
      
    struct integrand_aux_t aux; 
    aux.m = &ice_; 
    aux.alpha2 = alpha*alpha; 
    aux.R = opt_.R; 
    aux.atten_fs = opt.atten_fs;
    double ans_t =0; 
    double ans_d=0; 
    double ans_s = 0; 
    std::vector<double> ans_A(A ? opt.atten_fs.size() : 0); 

    bool only_d  = d && !t && !s && !A ; 

    for (int iturn = 0; iturns < nturns+1; iturn++) 
    {
      double sign = lower_limits[iturn] < upper_limits[iturn] ? -1 : 1; 
      std::vector<double> ans(only_d ? 1 : 3 + ans_A.size()); 
      
      std::vector<double> err(only_d ? 1 : 3 + ans_A.size()); 
      int ok = pcubature_v(only_d ? 1 : 3 + ans_A.size(), integrand, &aux, 1, &lower_limits[iturn], &upper_limits[iturn], 0, 1e-2,0, ERROR_INDIVIDUAL, &ans[0],err); 

      if (!ok) return ok; 

      if (only_d) 
      {
        ans_d += ans[0] * sign * alpha; 
      }
      else
      {
        ans_t += ans[0]*sign * (1./0.3); 
        ans_d += ans[1]*sign *alpha; 
        ans_s += ans[2]*sign; 
        for (size_t i = 0; i < ans_A.size(); i++) 
          ans_A[i] += ans[3+i]*sign; 
      }
    }

    if (t) *t = ans_t; 
    if (d) *d = ans_d; 
    if (s) *s = ans_s; 
    if (A) memcpy(A,&ans_A[0], sizeof(double) * ans_A.size()); 
    return 0;
  }

  struct gsl_root_aux
  {
    ice::Model * m; 
    IntegratingRaytracer::Options *opt; 
    double alpha;

  };

  double IntegratingRaytracer::findTurningPoint(double alpha, double z, bool up, int i = 0) 
  {
    depths_.clear(); 
    depths.push_back(0); 
    int nsolns = ice_.refractionMode()->getDepthsWithN(alpha, depths); 
    depths.push_back(m_.iceDepth()); 

    std::sort(depths.begin(), depths.end()); 
    size_t lb = std::lower_bound(depths.begin(), depths.end(), z); 

    //here we just have alpha = n sin theta, so that's good enough
    if (opt_.flatEarth())
    {
      if (!up)
        return depths[TMath::Max(0,lb-1-i)]
      else
        return depths[TMath::Min(lb+i, depths.size()-1)];
    }


    //otherwise, this gets a bit trickier. We'll use the flat Earth solution as a guess
    //and use a root-finder to get the real answer. 
    /
    gsl_function F; 

    F.function = [](double z, void * p)
    {
      gsl_root_aux * aux = (gsl_root_aux*) p; 
      return (1 - z / aux->opt.Rsurface) * aux.ice->refraction(z) - aux->alpha; 
    }

    gsl_root_aux aux{.m=&m_, .opt=&opt_, .alpha = alpha}; 
    F.aux = &aux; 
    j

    double val = F.function(z); 

    


    
  }

    double IntegratingRaytracer::computeAlpha(double d, double z, unsigned nturns, const double * turning_points) 
    {
      //so we know how to solve the for d given alpha


    }



    Options defaultOpts; 
    const Options & Options::defaultOptions() { return defaultOpts; } 




  }
}


