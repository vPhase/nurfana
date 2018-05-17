
#include "nurfana/Digitizer.h" 

static nurfana::Digitizer the_IRS2 ("IRS2", 3.2, false); 
static nurfana::Digitizer the_PA ("ADC07D1520", 1.5, true); 
 
namespace nurfana
{


 const Digitizer & Digitizer::IRS2() { return the_IRS2; } 
  const Digitizer & Digitizer::PhasedArray() { return the_PA; } 
}
