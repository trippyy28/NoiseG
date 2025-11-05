// Voice.h
#include <juce_dsp/juce_dsp.h>
#pragma once
#include "Oscillator.h"

struct Voice {
  int note = -1;      // -1 = free
  bool held = false;  // key is down
  bool preview = false;
  float amplitude = 0.0f;

  float filterModAmount = 1.0f;
  Oscillator osc;

  juce::ADSR ampEnvelope;
  juce::ADSR filterEnvelope;

  juce::ADSR::Parameters ampParams;
  juce::ADSR::Parameters filterParams;

  // per-voice filter (mono)
  juce::dsp::StateVariableTPTFilter<float> filter;
  juce::SmoothedValue<float> filterEnvSmoother{0.0f};

  void reset() {
    note = -1;
    held = false;
    preview = false;
    amplitude = 0.0f;
    osc.reset();
    ampEnvelope.reset();
    filterEnvelope.reset();
    filter.reset();
    filterEnvSmoother.setCurrentAndTargetValue(0.0f);
  }

  bool isActive() const { return note >= 0 || ampEnvelope.isActive(); }

  void setFilterModAmount(float amount) { filterModAmount = amount; }

  void setAmpADSR(float a, float d, float s, float r) {
    ampParams.attack = a;
    ampParams.decay = d;
    ampParams.sustain = s;
    ampParams.release = r;
    ampEnvelope.setParameters(ampParams);
  }

  void setFilterADSR(float a, float d, float s, float r) {
    filterParams.attack = a;
    filterParams.decay = d;
    filterParams.sustain = s;
    filterParams.release = r;
    filterEnvelope.setParameters(filterParams);
  }

  // one-sample render (per voice)
  float renderSample(float baseCutoff, bool filterEnabled) {
    float x = osc.nextSample();

    // amp env
    float amp = ampEnvelope.getNextSample();
    amplitude = amp;
    x *= amp;

    if (filterEnabled) {
      // filter env → cutoff
      float env = filterEnvelope.getNextSample();  // 0..1
      filterEnvSmoother.setTargetValue(env);
      float smoothEnv = filterEnvSmoother.getNextValue();
      float cutoff = juce::jlimit(
          20.0f, 20000.0f, baseCutoff + smoothEnv * filterModAmount * 3000.0f);
      filter.setCutoffFrequency(cutoff);

      // mono per-voice filter: process channel 0.
      x = filter.processSample(0, x);
    }

    return x;
  }
};
