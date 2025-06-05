#include <juce_dsp/juce_dsp.h>
#pragma once
#include "Oscillator.h"

struct Voice {
  int note;
  float amplitude;
  float filterModAmount = 1.0f;
  Oscillator osc;

  juce::ADSR ampEnvelope;
  juce::ADSR filterEnvelope;

  juce::ADSR::Parameters ampParams;
  juce::ADSR::Parameters filterParams;

  void reset() {
    note = 0;
    amplitude = 0.0f;
    osc.reset();
    ampEnvelope.reset();
    filterEnvelope.reset();
  }
  void setFilterModAmount(float amount) { filterModAmount = amount; }

  void setAmpADSR(float attack, float decay, float sustain, float release) {
    ampParams.attack = attack;
    ampParams.decay = decay;
    ampParams.sustain = sustain;
    ampParams.release = release;
    ampEnvelope.setParameters(ampParams);  // ← זה חובה
  }

  void setFilterADSR(float attack, float decay, float sustain, float release) {
    filterParams.attack = attack;
    filterParams.decay = decay;
    filterParams.sustain = sustain;
    filterParams.release = release;
    filterEnvelope.setParameters(filterParams);  // ← גם פה
  }
  float render() {
    float oscOut = osc.nextSample();
    float envAmp = ampEnvelope.getNextSample();
    return oscOut * envAmp;
  }
};