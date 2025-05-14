#include <juce_dsp/juce_dsp.h>
#pragma once
#include "Oscillator.h"

struct Voice {
  int note;
  float amplitude;
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

  void setAmpADSR(float attack, float decay, float sustain, float release) {
    ampParams.attack = attack;
    ampParams.decay = decay;
    ampParams.sustain = sustain;
    ampParams.release = release;
    ampEnvelope.setParameters(
        ampParams);  // Apply the parameters to the envelope
  }

  float render() {
    ampEnvelope.setParameters(ampParams);
    float oscOut = osc.nextSample();
    float envAmp = ampEnvelope.getNextSample();
    return oscOut * envAmp;
  }
};