#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Synth.h"
#include "Voice.h"
#include <memory>
#include <iostream>

namespace audio_plugin {

class NoiseGAudioProcessor : public juce::AudioProcessor {
public:
  NoiseGAudioProcessor();
  ~NoiseGAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void reset() override;

  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  using AudioProcessor::processBlock;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  // מקורות אמת: הווליום מאוחסן כאן

  float getVolume() const { return volume; }
  float getFilterCutOff() const { return synth.filter.getCutoffFrequency(); }
  float getAmpAttack() const { return synth.voice.ampParams.attack; }
  float getAmpDecay() const { return synth.voice.ampParams.decay; }
  float getAmpSustain() const { return synth.voice.ampParams.sustain; }
  float getAmpRelease() const { return synth.voice.ampParams.release; }
  float getFilterAttack() const { return synth.voice.filterParams.attack; }
  float getFilterDecay() const { return synth.voice.filterParams.decay; }
  float getFilterSustain() const { return synth.voice.filterParams.sustain; }
  float getFilterRelease() const { return synth.voice.filterParams.release; }

  void setVolume(float volume);
  void setWaveform(int waveformType);
  void setAmpADSR(float a, float d, float s, float r);
  void setFilterADSR(float a, float d, float s, float r);
  void setModulationFilter(float amount);
  Synth synth;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseGAudioProcessor)
  void splitBufferByEvents(juce::AudioBuffer<float>& buffer,
                           juce::MidiBuffer& midiMessages);
  void handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2);
  void render(juce::AudioBuffer<float>& buffer,
              int sampleCount,
              int bufferOffset);

  float volume;  // עכשיו מאתחלים את זה בקונסטרקטור
  Voice voice;
};

}  // namespace audio_plugin