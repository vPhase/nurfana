#ifndef _NURFANA_RAYTRACING_H
#define _NURFANA_RAYTRACING_H


#include "nurfana/Interpolation2D.h" 
#include <vector> 

class TGraph; 


namespace nurfana
{

  class IceModel; 


  namespace raytracer 
  {
      //Options for a raytracer 
      struct Options
      {

          double Rsurface = 6356752.3 + 2836; 
          bool flatEarth() const { return Rsurface <= 0 || Rsurface >= DBL_MAX; } 
          std::vector<double> atten_fs = {0.3 }; 
          const Options & defaultOptions(); 
      }; 


      /**  
       *  Rays will turn either at the surface or at a specific index of refraction. 
       *
       *  For non-monotonic firns, there can be very complicated structure. 
       *
       *  We can uniquely identify each solution by 
       *  the number of turns, and the number of skipped layers at for each turn. 
       *
       *  Note that in most cases you can use turnsWithNoSkip(); 
       *
       **/ 
      class Turns
      {
        public: 
          Turns(unsigned num_turns = 0, 
                const unsigned * num_skips = 0) 
          :    nturns_(num_turns), nskips_(0)
          {
            if (num_skips) nskips_ = new std::vector<unsigned>( num_skips, num_skips + num_turns); 
          }

          ~Turns() { if (nskips_) delete nskips_; } 

          unsigned getNTurns() const { return nturns_; } 
          unsigned getNSkips(unsigned i) const { return nskips_ ? nskips_[i] : 0; } 

          static const Turns & turnsWithNoSkip(unsigned i); 

        private: 
          unsigned nturns_; 
          std::vector<unsigned> * nskips_ = 0; 

      }; 
      
    
      //A raytracer solution 
      struct Solution 
      {
          double z0;  //start depth
          double z1;  //end depth
          double theta0; //start zenith angle
          double theta1; //end zenith angle
          double t;  // time 
          double s;  // total distance
          double d;  // surface distance
          double phi; // angular distance 
          double alpha; // the ray parameter 
          Turns turns; 
          std::vector<double> turning_points; 
          std::vector<std::pair<double,double> > attenuation_v_F; 
          Options opt; 
      }; 



    /** An spherically symmetric ray tracer (you can force a flat Earth too, if you want, by setting Rsurface <=0 ))  
     *  using numerical integration with the cubature library. 
     *
     *   The assumption is that we have a spherically symmetric index of refraction n(z), where z is the depth below the spherical surface R; 
     *
     *    We fix z0, since usually you care about raytracing to a fixed point. 
     *
     *    Once you've calculated enough nearby points, this can start interpolating if you'd like. 
     *
     **/ 
    class IntegratingRaytracer  
    {

      public: 
         
                IntegratingRaytracer(const IceModel & icemodel, double z0, const Options & opt = Options::defaultOptions()) 
                             

          /* Solve the raytracing problem from z0 to z, at distance d, with nturns turns.
           *
           * Returns NULL if no solution. If solution is passed it will be filled if there is a solution)
           *
           * */ 
          Solution * solve(double z, double d, const Turns & = Turns::turnsWithNoSkip(0), Solution * solution = 0); 

          /** Advanced helper.
           *
           * Not really for public use
           * but it's here anyway
           *
           * But I won't bother documenting. 
           **/ 
          int computeRay(double z, double alpha, 
                         double * t, double * d,
                         double * s, double * A, 
                         const Turns * turns = 0, 
                         const double * turning_points = 0,
                         double * save_turns = 0 
                         ); 



          /** This will find the ith turning point for the current ray  from the current depth. For a 
           * complicated firn model (with layers) there may be lots of potential turning points, 
           * in which case you can skip some using the i parameter. 
           *
           * If no turning point can be found, the surface or bedrock  will be returned (i.e. the surface).
           *
           **/
          double findTurningPoint(double alpha,  double z, bool up, int i = 0) ; 



          double computeAlpha(double d, double z, unsigned nturns = 0); //returns negative if no solution 


      private: 

          //This caches solutions.
          struct Cache
          {
            std::vector<double> turning_points; 
            

          };

          const IceModel & ice_; 
          double z0_; 
          Options opt_; 

          //used for computing turning points 
          std::vector<double> depths_; 

    }; 
  }
}



#endif

