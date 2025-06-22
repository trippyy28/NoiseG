#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Voice.h"
#include "NoiseGenerator.h"

class Synth {
public:
  Synth();
  ~Synth();
  WaveformType waveform = SAW;
  void setWaveform(WaveformType wf);
  WaveformType getWaveform() const { return waveform; }
  void allocateResources(double sampleRate, int samplesPerBlock);
  void deallocateResources();
  void reset();
  void render(float** outputBuffers, int sampleCount);
  void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2);
  void setVolume(float volume);
  void setCutoff(float freq);
  void setFilterResonance(float q);
  void setFilterEnabled(bool shouldEnable);
  void setFilterModAmount(float amount);
  bool getFilterEnbaled() const { return filterEnabled; }
  juce::dsp::StateVariableTPTFilter<float> filter;
  Voice voice;

private:
  NoiseGenerator noiseGen;

  juce::dsp::ProcessSpec filterSpec;
  void noteOn(int note, int velocity);
  void noteOff(int note);
  float sampleRate;
  float volume = 0.5f;
  float baseCutoff = 300.f;
  bool filterEnabled = true;
};