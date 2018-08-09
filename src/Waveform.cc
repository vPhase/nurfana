#include "nurfana/Waveform.h" 
#include "nurfana/SignalOps.h" 
#include <assert.h> 
#include "nurfana_private.h" 


#define ZERO() hilbert_ = 0; envelope_ = 0;
#define SET_UNEVEN() uneven_dirty_ = false; even_dirty_=true; freq_dirty_=true; hilbert_dirty_=true; envelope_dirty_ = true; 
#define SET_EVEN() uneven_dirty_ = true; even_dirty_=false; freq_dirty_=true; hilbert_dirty_=true; envelope_dirty_ = true; 
#define SET_FREQ() uneven_dirty_ = true; even_dirty_=true; freq_dirty_=false; hilbert_dirty_=true; envelope_dirty_ = true; 


namespace nurfana
{


  //Uneven Constructors
  Waveform::Waveform(const TGraph & g) 
    : uneven_(g) 
  {
    SET_UNEVEN(); 
    ZERO(); 
  }

  Waveform::Waveform(const UnevenRepresentation & uneven) 
    : uneven_(uneven) 
  {
    SET_UNEVEN(); 
    ZERO(); 
  }

  Waveform::Waveform(UnevenRepresentation && uneven) 
    :uneven_(uneven) 
  {
    SET_UNEVEN(); 
    ZERO(); 
  }

  //Even constructors 

  Waveform::Waveform(const EvenRepresentation & even)
    : even_(even)
  {
    SET_EVEN(); 
    ZERO(); 
  }

  Waveform::Waveform(EvenRepresentation && even)
    : even_(even)
  {
    SET_EVEN(); 
    ZERO(); 
  }


  //Freq constructors
  //
  Waveform::Waveform(const FrequencyRepresentation & freq)
    : freq_(freq) 
  {
    SET_FREQ(); 
    ZERO(); 
  }

  Waveform::Waveform(FrequencyRepresentation && freq)
    : freq_(freq) 
  {
    SET_FREQ(); 
    ZERO(); 
  }


  const Waveform & Waveform::hilbertTransform() const 
  {
    if (hilbert_dirty_) 
    {
      if (!hilbert_)
      {
        FrequencyRepresentation * fr = ops::doHilbertTransform(&freq()); 
        hilbert_ = new Waveform(std::move(*fr)); 
      }
      else
      {
        ops::doHilbertTransform(&freq(), &hilbert_->updateFreq()); 
      }
      hilbert_dirty_ = false; 
    }
    return *hilbert_; 
  }


  const EvenRepresentation & Waveform::envelope() const 
  {

    if (envelope_dirty_) 
    {
      if (!envelope_) envelope_ = new EvenRepresentation(even()); 

      envelope_->resize(even().N()); 

      auto hil = hilbertTransform(); 
      auto Y = envelope_->updateY(); 
      for (unsigned i = 0; i < even().N(); i++) 
      {
        Y[i] = sqrt(pow(even()[i],2) + pow(hil.even()[i],2)); 
      }
      envelope_dirty_ = false; 
    }

    return *envelope_; 
  }


  void Waveform::prepareEven() const
  {
    if (!even_dirty_) return; 
    if (!uneven_dirty_) 
    {
        even_ = uneven_; 
        even_dirty_ = false; 
    }

    else if (!freq_dirty_) 
    {
        even_ = freq_; 
        even_dirty_ = false; 
    }
    else
    {
      // ruh roh
      assert(false); 
    }

  }

  void Waveform::prepareUneven() const
  {
    if (!uneven_dirty_) return; 
    if (!even_dirty_) 
    {
        uneven_ = even_; 
        uneven_dirty_ = false; 
    }
    else if (!freq_dirty_) 
    {
        even_ = freq_; 
        even_dirty_ = false; 
        uneven_ = even_; 
        uneven_dirty_ = false; 
    }
    else
    {
      // ruh roh
      assert(false); 
    }

  }


  void Waveform::prepareFreq() const
  {
    if (!freq_dirty_) return; 
    if (!even_dirty_) 
    {
      freq_ = even_; 
      freq_dirty_=false;
    }
    else if (!uneven_dirty_) 
    {
      even_ = uneven_; 
      even_dirty_ = false;
      freq_ = even_; 
      freq_dirty_=false;
    }
    else
    {
      assert(false); 
    }
  }


  EvenRepresentation & Waveform::updateEven() 
  {
    
     prepareEven(); 
     SET_EVEN(); 
     return even_; 
  }

  const EvenRepresentation & Waveform::even() const 
  {
    prepareEven(); 
    return even_; 
  }


  UnevenRepresentation & Waveform::updateUneven()
  {
     prepareUneven(); 
     SET_UNEVEN(); 
     return uneven_; 
  }

  const UnevenRepresentation & Waveform::uneven() const 
  {
    prepareUneven(); 
    return uneven_; 
  }

  FrequencyRepresentation & Waveform::updateFreq() 
  {
     prepareFreq(); 
     SET_FREQ(); 
     return freq_; 
  }

  const FrequencyRepresentation & Waveform::freq() const 
  {
    prepareFreq(); 
    return freq_; 
  }

  void Waveform::Draw(Option_t * opt) 
  {
    TString o(opt); 

    if ( consume(o, "EVEN")) 
    {
      prepareEven(); 
      even_.Draw(o.Data()); 
    }
    else if ( consume(o, "UNEVEN"))
    {
      prepareUneven() ; 
      uneven_.Draw(o.Data()); 
    }
    else if ( consume(o, "ENVELOPE")) 
    {
      envelope(); 
      envelope_->Draw(o.Data()); 
    }
    else
    {
      prepareFreq(); 
      freq_.Draw(opt); 
    }
  }



  Waveform::~Waveform() 
  {
    if (envelope_) delete envelope_; 
    if (hilbert_) delete hilbert_; 

  }

}



