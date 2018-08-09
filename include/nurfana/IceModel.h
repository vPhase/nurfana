#ifndef _NURFANA_ICE_MODEL_HH 
#define _NURFANA_ICE_MODEL_HH 

/** Define ice models with a forest of classes. 
 *
 *  Cosmin Deaconu <cozzyd@kicp.uchicago.edu> 
 *
 **/ 


#include "TNamed.h" 
#include <cmath> 
#include <vector> 

namespace nurfana
{
  namespace ice
  {
    /* To allow fully general composition with possible
     * internal state, each component of the ice model 
     * is its own abstract class. This kind of sucks, but 
     * oh well. 
     * */ 

    class DensityModel
    {
      public:
        virtual double density(double z_m) const = 0; 
        static const DensityModel & getDefault(); 
        virtual int getDepthsWithDensity(double rho, std::vector<double> & depths) const =0; 
        virtual ~DensityModel() { ; } 
    };

    class RefractionModel
    {
      public:
        virtual double n(double z_m) const = 0; 
        static const RefractionModel & getDefault(); 
        virtual int getDepthsWithN(double n, std::vector<double> & depths) const = 0;
        virtual ~RefractionModel() { ; } 
    };

    class AttenuationModel
    {
      public:
        virtual double attenuation(double z, double f_GHz = 0.3) const = 0; 
        static const AttenuationModel & getDefault(); 
        virtual ~AttenuationModel() { ; } 
    };


    class Model : public TNamed
    {

      public: 
        Model( const char * id = "default" , 
                  const DensityModel & density = DensityModel::getDefault(), 
                  const RefractionModel & refraction = RefractionModel::getDefault(), 
                  const AttenuationModel & attenuation = AttenuationModel::getDefault() , 
                  double iceDepth = 2800
                  ) 
          : TNamed(id,id), density_(density), refraction_(refraction), attenuation_(attenuation), iceDepth_(iceDepth) {}

        double density(double z_m) const { return density_.density(z_m); } 
        double attenuation(double z_m, double f_GHz = 0.3) const { return attenuation_.attenuation(z_m,f_GHz); } 
        double n(double z_m) const { return refraction_.n(z_m); } 

        double iceDepth() const { return iceDepth_; } 
        const DensityModel & densityModel() const { return density_ ; }
        const RefractionModel & refractionModel() const { return refraction_ ; }
        const AttenuationModel & attenuationModel() const { return attenuation_ ; }

        /** Options:   
         * RHO  -> density vs depth
         * ATTF=0.3  -> attenuation vs. depth at a frequency
         * ATTZ=100  -> attenuation vs frequency at a depth
         * REF   -> refractive index vs depth
         * MINZ=0 
         * MAXZ=3000 
         * MINF=0
         * MAXF=1 
         * NPTS=100  -> num points
         *
         * anything else is passed to a TGraph 
         *
         */
        void Draw(Option_t * opt = "RHO");  

      private: 
        const DensityModel & density_; 
        const RefractionModel & refraction_; 
        const AttenuationModel & attenuation_; 
        double iceDepth_; 
        ClassDef(Model,1); 
    }; 


    class DensityDerivedRefractionModel : public RefractionModel
    {
      public: 

        DensityDerivedRefractionModel (const DensityModel & m = DensityModel::getDefault()) 
          : m_(m) { } 

        double n(double z_m) const { return m_.density(z_m) * 0.845 + 1; } 
        virtual int getDepthsWithN(double n, std::vector<double> & depths) const
        {
          return m_.getDepthsWithDensity( (n-1)/0.845, depths); 
        }
    private: 
        const  DensityModel & m_; 
    }; 

    class RefractionDerivedDensityModel : public DensityModel
    {
      public: 
        RefractionDerivedDensityModel (const RefractionModel & m = RefractionModel::getDefault()) 
          : m_(m) { } 

        double n(double z_m) const { return (m_.n(z_m)-1) / 0.845; } 
        virtual int getDepthsWithDensity(double rho, std::vector<double> & depths) const
        {
          return m_.getDepthsWithN( rho * 0.845 + 1, depths); 
        }

    private: 
        const RefractionModel & m_; 
    }; 



    class ExponentialRefractionModel: public RefractionModel
    {
      public:

        /** Creates an index of refraction model with n = a0 + a1(1 - exp(-b1 z) */ 
        ExponentialRefractionModel(double a0 = 1.325, double a1 = 0.463, double b1 = 0.014) 
         : a0_(a0), a1_(a1), b1_(b1) { }

        virtual double n(double z_m) const { return a0_ + a1_ * (1-exp(-b1_*z_m)); } 
        virtual int getDepthsWithN(double N, std::vector<double> & depths) const ; 

 
      private: 
        double a0_; 
        double a1_; 
        double b1_; 
    }; 

    class ExponentialDensityModel : public DensityModel 
    {
      public: 
        /** Creates a density model with p= a0 + a1(1 - exp(-b1 z) */ 
        ExponentialDensityModel(double a0 = 0.37415, double a1 = 0.562385, double b1 = 0.01535) 
         : a0_(a0), a1_(a1), b1_(b1) { }

        virtual double density(double z_m) const { return a0_ + a1_ * (1-exp(-b1_*z_m)); } 
        virtual int getDepthsWithDensity(double rho, std::vector<double> & depths) const;
      private: 
        double a0_; 
        double a1_; 
        double b1_; 

    }; 



    class ConstantAttenuationModel : public AttenuationModel
    {
      public: 
        ConstantAttenuationModel(double l = 1500) : l_(l) {} 
        virtual double attenuation(double z_m __attribute__((unused)), double f __attribute__((unused))) const { return l_; } 
      private: 
        double l_; 
    }; 
  }
}


#endif
