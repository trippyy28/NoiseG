#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Synth.h"
#include "Voice.h"
#include <memory>
#include <iostream>
#include <vector>

namespace ParameterID {
#define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);
PARAMETER_ID(filterFreq)
PARAMETER_ID(filterReso)
PARAMETER_ID(outputLevel)
PARAMETER_ID(polyMode)
#undef PARAMETER_ID
}  // namespace ParameterID

namespace audio_plugin {

class NoiseGAudioProcessor : public juce::AudioProcessor {
public:
  NoiseGAudioProcessor();
  ~NoiseGAudioProcessor() override;

  juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters",
                                           createParameterLayout()};
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
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      volumeAttach;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      cutoffAttach;

  // מקורות אמת: הווליום מאוחסן כאן

  float getVolume() const { return volume; }
  float getFilterCutOff() const { return synth.voices[0].filter.getCutoffFrequency(); }
  float getFilterRes() const { return synth.voices[0].filter.getResonance(); }
  float getModFilterAmount() const { return modAmount; }
  float getAmpAttack() const { return synth.voices[0].ampParams.attack; }
  float getAmpDecay() const { return synth.voices[0].ampParams.decay; }
  float getAmpSustain() const { return synth.voices[0].ampParams.sustain; }
  float getAmpRelease() const { return synth.voices[0].ampParams.release; }
  float getFilterAttack() const { return synth.voices[0].filterParams.attack; }
  float getFilterDecay() const { return synth.voices[0].filterParams.decay; }
  float getFilterSustain() const { return synth.voices[0].filterParams.sustain; }
  float getFilterRelease() const { return synth.voices[0].filterParams.release; }
  int getPolyphony() const { return synth.getNumVoices(); }
  bool isChordModeEnabled() const { return synth.isChordModeEnabled(); }
  const std::vector<Synth::ChordBank>& getChordBanks() const {
    return synth.getChordBanks();
  }
  int getActiveChordBank() const { return synth.getActiveChordBank(); }
  bool isChordSlotActive(int bankIndex, int chordIndex) const {
    return synth.isChordSlotActive(bankIndex, chordIndex);
  }

  void setVolume(float volume);
  void setWaveform(int waveformType);
  void setAmpADSR(float a, float d, float s, float r);
  void setFilterADSR(float a, float d, float s, float r);
  void setModulationFilter(float amount);
  void setPolyphony(int voices);
  void enableChordMode(bool enabled);
  void setChordBank(int bankIndex, const Synth::ChordBank& bank);
  void setChord(int bankIndex, int chordIndex, const Synth::Chord& chord);
  void setChordBanks(std::vector<Synth::ChordBank> banks);
  void setActiveChordBank(int bankIndex);
  void previewChord(int bankIndex, int chordIndex, int velocity = 100);
  void stopPreviewChord();
  Synth synth;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseGAudioProcessor)
  void splitBufferByEvents(juce::AudioBuffer<float>& buffer,
                           juce::MidiBuffer& midiMessages);
  void handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2);
  void render(juce::AudioBuffer<float>& buffer,
              int sampleCount,
              int bufferOffset);

  float volume;
  float modAmount;
  std::atomic<float>* volumeParam = nullptr;  // dB
  std::atomic<float>* cutoffParam =
      nullptr;  // Hz  // עכשיו מאתחלים את זה בקונסטרקטור
  Voice voice;
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
  juce::AudioParameterFloat* oscMixParam;
  juce::AudioParameterFloat* oscTuneParam;
  juce::AudioParameterFloat* oscFineParam;
  juce::AudioParameterChoice* glideModeParam;
  juce::AudioParameterFloat* glideRateParam;
  juce::AudioParameterFloat* glideBendParam;
  juce::AudioParameterFloat* filterFreqParam;
  juce::AudioParameterFloat* filterResoParam;
  juce::AudioParameterFloat* filterEnvParam;
  juce::AudioParameterFloat* filterLFOParam;
  juce::AudioParameterFloat* filterVelocityParam;
  juce::AudioParameterFloat* filterAttackParam;
  juce::AudioParameterFloat* filterDecayParam;
  juce::AudioParameterFloat* filterSustainParam;
  juce::AudioParameterFloat* filterReleaseParam;
  juce::AudioParameterFloat* envAttackParam;
  juce::AudioParameterFloat* envDecayParam;
  juce::AudioParameterFloat* envSustainParam;
  juce::AudioParameterFloat* envReleaseParam;
  juce::AudioParameterFloat* lfoRateParam;
  juce::AudioParameterFloat* vibratoParam;
  juce::AudioParameterFloat* noiseParam;
  juce::AudioParameterFloat* octaveParam;
  juce::AudioParameterFloat* tuningParam;
  juce::AudioParameterFloat* outputLevelParam;
  juce::AudioParameterChoice* polyModeParam;
};

}  // namespace audio_plugin
