#include "nurfana/IceModel.h" 
#include "nurfana_private.h" 
#include "TGraph.h" 
#include "TAxis.h" 


namespace nurfana
{
  namespace ice
  {

    static ExponentialDensityModel default_dens; 
    static DensityDerivedRefractionModel default_refr(default_dens); 
    static ConstantAttenuationModel default_atten; 

    const DensityModel & DensityModel::getDefault() { return default_dens; } 
    const RefractionModel & RefractionModel::getDefault() { return default_refr; } 
    const AttenuationModel & AttenuationModel::getDefault() { return default_atten; } 




    void Model::Draw(Option_t * t) 
    {

      enum {RHO, REF, ATTF, ATTZ} what = RHO; 
      double minz = 0; 
      double maxz = 3000; 
      double minf = 0; 
      double maxf = 1; 
      int N = 100; 
      double attf = 0.3; 
      double attz = 100; 

      TString opt = t; 

      if (parse_opt(opt,"ATTF", &attf)) what = ATTF;  
      if (parse_opt(opt,"ATTZ", &attz)) what = ATTZ; 
      if (consume(opt,"REF")) what = REF; 
      if (consume(opt,"RHO")) what = RHO; 

      parse_opt(opt,"MINZ", &minz); 
      parse_opt(opt,"MAXZ", &maxz); 
      parse_opt(opt,"MINF", &minf); 
      parse_opt(opt,"MAXF", &maxf); 
      parse_opt(opt,"NPTS", &N); 

      TGraph * g = new TGraph(N); 
      TString str; 

      if (what == ATTF) 
      {
        for (int i = 0; i < N; i++) 
        {
          double z =  (maxz-minz)/(N-1) * i; 
          g->GetX()[i] =z; 
          g->GetY()[i] = attenuation_.attenuation(z,attf); 
        }
        g->GetXaxis()->SetTitle("Depth (m)"); 
        g->GetYaxis()->SetTitle("Attenuation (m) "); 
        str.Form("Attenuation vs. Depth (f=%g GHz)", attf); 
        g->SetTitle(str.Data()); 
      }

      if (what == ATTZ) 
      {
        for (int i = 0; i < N; i++) 
        {
          double f = (maxf - minf) / (N-1) * i; 
          g->GetX()[i] = f; 
          g->GetY()[i] = attenuation_.attenuation(attz,f); 
        }
        g->GetXaxis()->SetTitle("Frequency (GHz)"); 
        g->GetYaxis()->SetTitle("Attenuation (m) "); 
        str.Form("Attenuation vs. Frequency (z=%g m)", attz); 
        g->SetTitle(str.Data()); 
      }

      if (what == REF) 
      {
        for (int i = 0; i < N; i++) 
        {
          double z =  (maxz-minz)/(N-1) * i; 
          g->GetX()[i] =z; 
          g->GetY()[i] = refraction_.n(z); 
        }
       
        g->GetXaxis()->SetTitle("Depth (m)"); 
        g->GetYaxis()->SetTitle("Refractive Index"); 
        str.Form("Refractive Index vs. Depth"); 
        g->SetTitle(str.Data()); 
      }

      if (what == RHO) 
      {
        for (int i = 0; i < N; i++) 
        {
          double z =  (maxz-minz)/(N-1) * i; 
          g->GetX()[i] =z; 
          g->GetY()[i] = density_.density(z); 
        }
       
        g->GetXaxis()->SetTitle("Depth (m)"); 
        g->GetYaxis()->SetTitle("Density (g/cm^3)"); 
        str.Form("Density vs. Depth"); 
        g->SetTitle(str.Data()); 
      }

      g->SetBit(kCanDelete); 
      g->Draw(opt); 
    }
  }

}

