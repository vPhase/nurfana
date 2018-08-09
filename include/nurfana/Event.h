#ifndef _NURFANA_EVENT 
#define _NURFANA_EVENT 

/** A class containing what is necessary to analyze an event */ 

#include "nurfana/Channel.h" 

namespace nurfana
{

  class Reader; 
  class Event
  {
    friend Reader; 

    public: 

      class Header
      {
        public: 

          //These are fields that are global to anything
          double trigger_time; 
          enum trigger_type { UNKNOWN, RF, SOFT, CALIB } trig_type; 

          //TODO: add some capability to get detector-specific information 

         
      }; 


      unsigned nChannels() const { return channels_.size(); } 
      Channel * channel(int i) { return &channels_[i] ; }
      const Channel * channel(int i) const { return &channels_[i]; }

    private: 
      Event() {;} 
      std::vector<Channel> channels_; 
      Header meta_; 
  };

  /** A reader knows how to make events */ 
  class Reader
  {

    public: 
      virtual int nEvents() const = 0; 

      int next() { return get(current_entry_+1); }
      int previous() { return get(current_entry_-1); }
      int first() { return get(0); }
      int last() { return get(nEvents()-1); }

      int get(int i) {
        if (i < 0 || i > nEvents()) return current_entry_;
        current_entry_ = i; 
        return current_entry_; 
      }

     virtual Event::Header & header(bool force = false) { if (force || loaded_meta_ != current_entry_) loadMeta(current_entry_); return ev_.meta_; }

     virtual Event & event(bool force = false) 
      {
        header(force); 
        if (force || loaded_event_ != current_entry_) loadEvent(current_entry_); 
        return ev_;
      }


    protected:
      Reader () 
      {
        current_entry_ = 0; 
        loaded_event_ = -1; 
        loaded_meta_ = -1; 

      }

      virtual int loadEvent(int i) = 0; 
      virtual int loadMeta(int i) = 0; 

      std::vector<Channel> & channels() { return ev_.channels_; } 
      Event::Header & meta() { return ev_.meta_; } 

      int current_entry_; 
      int loaded_event_; 
      int loaded_meta_ ;
      Event ev_; 
  }; 


} 

#endif

