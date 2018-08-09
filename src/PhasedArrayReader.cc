#include "nurfana/PhasedArray.h" 

#include "TGraph.h" 
#include "TMutex.h" 
#include <map> 




namespace nurfana
{
#ifdef HAVE_NUPHASE

  //These will get more complicated in the future, probably 
  static const Response * getPhasedArrayResponse(double depth)
  {
    static Response stupid; 
    (void) depth; 
    return &stupid; 
  }


  //These will get more complicated in the future, probably 
  static const Antenna * getPhasedArrayAntenna(double depth)
  {

    static std::map<double, Antenna*> antennas; 
    static TMutex m; 

    if (!antennas.count(depth))
    {
      TLockGuard l(&m); 
      antennas[depth] = new Antenna(0,0,-depth,0); 
    }

    return antennas[depth]; 
  }

  void PhasedArrayReader::setupChain(const char * dir, int nruns, const int * runs) 
  {
    npEv_ = 0; 
    npHd_ = 0; 

    ch_ev_.SetName("event"); 
    ch_hd_.SetName("header"); 
    

    TString fi; 
    for (int i = 0; i < nruns; i++)
    {
      fi.Form("%s/run%d/event.root", dir, runs[i]); 
      ch_ev_.Add(fi.Data()); 
      fi.Form("%s/run%d/header.filtered.root", dir, runs[i]); 

      //does filtered file exist? 
      if (!access(fi.Data(),F_OK))
      {
        ch_hd_.Add(fi.Data()); 
      }
      else //try just normal file
      {
        fi.Form("%s/run%d/header.root", dir, runs[i]); 
        ch_hd_.Add(fi.Data()); 
      }
    }

    ch_ev_.SetBranchAddress("event",&npEv_); 
    ch_hd_.SetBranchAddress("head",&npHd_); 


  }


  int PhasedArrayReader::loadEvent(int i) 
  {
    if (!ch_ev_.GetEntry(i)) return 1; 

    channels().clear(); 

    //just hardcode stuff for now... 
    // this will have to get more complicated in the future 
    
    static nuphase::CalibrationInfo ci; 

    bool old_config = (npHd_->event_number >> 32) < 400; 
    npEv_->setCalibrationInfo(ci); 
     
    //master
    double depth = 196; 
    for (int i = 0; i < 8; i++) 
    {
      //there are way too many copies here. Will need to fix this... 
      TGraph * g = npEv_->getGraph(i); 
      EvenRepresentation even(g->GetN(), g->GetY(), g->GetX()[1] - g->GetX()[0], g->GetX()[0]); 
      delete g; 
      Waveform * wf = new Waveform(even); 
      if (i >= 6 && !old_config) depth+=1; 
      TString name; name.Form("NUPHASE_CH%d", i); 
      channels().emplace_back(name.Data(), wf, getPhasedArrayAntenna(depth), getPhasedArrayResponse(depth), &Digitizer::PhasedArray()); 
      depth+=1; 
    }

    
    //TODO, hpol channels, etc. 
    
    return 0; 

  }

  int PhasedArrayReader::loadMeta(int i) 
  {
    if (!ch_hd_.GetEntry(i)) return 1; 


    meta().trigger_time = npHd_->corrected_trigger_time; 
    meta().trig_type =  (Event::Header::trigger_type) npHd_->trigger_type; 

    return 0; 
  }






#else
  void PhasedArray::setupChain(const char * dir, int nruns, const int * runs) 
  {
    (void) dir; 
    (void) nruns; 
    (void) runs; 
    fprintf(stderr,"Not compiled with Phased Array support\n"); 
  }

  int PhasedArray::loadEvent(int i) 
  {
    (void) i; 
    return -1; 
  }

  int PhasedArray::loadMeta(int i) 
  {
    (void) i; 
    return -1; 
  }
#endif



}


