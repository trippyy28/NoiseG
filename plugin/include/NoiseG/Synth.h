#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Voice.h"
#include "NoiseGenerator.h"

class Synth {
public:
  Synth();
  ~Synth();
  WaveformType waveform = SAW;
  void setWaveform(WaveformType wf) { waveform = wf; }
  WaveformType getWaveform() const { return waveform; }
  void allocateResources(double sampleRate, int samplesPerBlock);
  void deallocateResources();
  void reset();
  void render(float** outputBuffers, int sampleCount);
  void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2);
  void setVolume(float volume);

private:
  NoiseGenerator noiseGen;
  void noteOn(int note, int velocity);
  void noteOff(int note);
  float sampleRate;
  float volume = 0.5f;
  Voice voice;
};