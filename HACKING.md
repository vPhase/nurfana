# Conventions and such 

## language and environment
  - nurfana is written in c++11. GNU extensions are ok
  - It should compile with gcc 4.8 and later. 

## Dependencies
  - All external dependencies (other than other collaboration code, of course) should be relatively common. Uncommon third-party libraries should be internally packaged (git submodule preferred) 
  - Current third party dependencies are: 
     - ROOT 6+ 
     - GSL 2+ 
     - FFTW3 
  - Optional other collaboration dependencies are: 
    - AraRoot (really just AraEvent) 
    - nuphaseroot 


## Build system 
  - A complicated custom Makefile. May someday move to cmake or some other fancy system if it's needed. 
  - Build customization is via m.config , which requires users to conform to Makefile syntax 

## Naming 
  - Variables are generally underscore separated.
  - Class instance variables end in an underscore.  Yeah, it's ugly, I know. For public members (and structs) this is not necessary. 
  - Usually prefer camel case starting with lowercase for methods. This helps distinguish from ROOT methods. 
  - Class and Struct names should be camelcase capitalized. 

## Files and paths
  - All public headers should be under include/nurfana, so that everthying public gets included via nurfana/Foo.h 
  - private headers can live inside src/ 



## Units
  - Lengths should be in meters
  - Times should be in ns 
  - Therefore, c = 0.3 
  - Frequencies in GHz 
  - Densities in g/cm^3
  - Voltages in mV 
  - E-field in mV/m 
  - 


