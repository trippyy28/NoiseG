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

  float render() {
    float oscOut = osc.nextSample();
    float envAmp = ampEnvelope.getNextSample();
    return oscOut * envAmp;
  }
};