#pragma once
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "Oscillator.h"

class MyVoice : public juce::SynthesiserVoice {
public:
  MyVoice();

  bool canPlaySound(juce::SynthesiserSound* sound) override;

  void startNote(int midiNoteNumber,
                 float velocity,
                 juce::SynthesiserSound*,
                 int /*currentPitchWheelPosition*/) override;

  void stopNote(float velocity, bool allowTailOff) override;

  void pitchWheelMoved(int) override {}
  void controllerMoved(int, int) override {}

  void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                       int startSample,
                       int numSamples) override;

  void setWaveform(WaveformType wf);
  void setVolume(float vol);
  void setFilterParams(float cutoff, float res, float modAmount);
  void setAmpADSR(const juce::ADSR::Parameters&);
  void setFilterADSR(const juce::ADSR::Parameters&);

private:
  Oscillator osc;
  juce::ADSR ampEnvelope;
  juce::ADSR filterEnvelope;
  juce::dsp::StateVariableTPTFilter<float> filter;
  juce::dsp::ProcessSpec spec;

  WaveformType waveform = SAW;
  float baseCutoff = 1000.0f;
  float reso = 1.0f;
  float modAmount = 1.0f;
  float sampleRate = 44100.0f;
  float volume = 0.5f;

  bool isPrepared = false;
};