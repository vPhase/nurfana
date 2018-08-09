#include "nurfana/SignalOps.h" 
#include "TGraph.h" 

double nurfana::ops::impulsivityMeasure(const Waveform *wf, TGraph * distance_cdf , int pt , bool use_envelope ) 
{

  const EvenRepresentation & g = use_envelope ? wf->envelope() : wf->even(); 

  if (pt < 0) 
  {
    unsigned u; 
    g.getPeak(&u,0,-1,true); 
    pt = u; 
  }

  int N = TMath::Max(pt+1,(int)  g.N()-pt+1); 

  if (distance_cdf) distance_cdf->Set(N); 


  double total = g.getSumV2(); 
  double sumv2 = 0; 


  double v= g[pt]; 
  sumv2+=v*v; 

  if (distance_cdf)
  {
    distance_cdf->GetX()[0] = 0; 
    distance_cdf->GetY()[0] = sumv2/total; 
  }

  double ysum = sumv2; 

  int i = 0; 
  while(++i < N) 
  {
    if (pt+i < (int) g.N())
    {
      v= g[pt+i]; 
      sumv2+=v*v; 
    }

    if (pt -i >= 0) 
    {
      v= g[pt-i]; 
      sumv2+=v*v; 
    }

    if (distance_cdf)
    {
      distance_cdf->GetY()[i] = sumv2 / total; 
      distance_cdf->GetX()[i] = i * g.dt(); 
    }

    ysum+= sumv2; 
  }


  return 2 * ysum / (N*total)-1; 
}
