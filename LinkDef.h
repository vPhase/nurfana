#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link off all namespaces;

//namespaces 
#pragma link C++ namespace nurfana+;
#pragma link C++ namespace nurfana::fft+;
#pragma link C++ namespace nurfana::angle+;
#pragma link C++ namespace nurfana::ice+;
#pragma link C++ namespace nurfana::ops+;

//Interpolation stuff
#pragma link C++ enum nurfana::InterpolationType; 
#pragma link C++ class nurfana::Interpolator+; 
#pragma link C++ class nurfana::Interpolatable+; 
#pragma link C++ class nurfana::GSLInterpolator+; 
#pragma link C++ class nurfana::GridInterpolator2D+; 
#pragma link C++ class nurfana::LinearInterpolator+; 

//Waveform stuff
#pragma link C++ class nurfana::TimeRepresentation+; 
#pragma link C++ class nurfana::EvenRepresentation+; 
#pragma link C++ class nurfana::UnevenRepresentation+; 
#pragma link C++ class nurfana::FrequencyRepresentation+; 
#pragma link C++ class nurfana::Waveform+; 

// Mappers
#pragma link C++ class nurfana::Mapper+; 
#pragma link C++ class nurfana::ElevationMapper+; 

// correlation and combining
#pragma link C++ class nurfana::Correlator+; 
#pragma link C++ class nurfana::Combiner+; 



//Ice Model stuff
#pragma link C++ class nurfana::ice::Model+; 
#pragma link C++ class nurfana::ice::DensityModel+; 
#pragma link C++ class nurfana::ice::RefractionModel+; 
#pragma link C++ class nurfana::ice::AttenuationModel+; 
#pragma link C++ class nurfana::ice::DensityDerivedRefractionModel+; 
#pragma link C++ class nurfana::ice::RefractionDerivedDensityModel+; 
#pragma link C++ class nurfana::ice::ExponentialRefractionModel+; 
#pragma link C++ class nurfana::ice::ExponentialDensityModel+; 
#pragma link C++ class nurfana::ice::ConstantAttenuationModel+; 


//Digitizer 
#pragma link C++ class nurfana::Digitizer; 

// Other stuff
#pragma link C++ class nurfana::Antenna+; 
#pragma link C++ class nurfana::Response+; 
#pragma link C++ class nurfana::Channel+; 

#endif

