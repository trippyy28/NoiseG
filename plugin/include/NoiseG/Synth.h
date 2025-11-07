#pragma once
#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Voice.h"
#include "NoiseGenerator.h"

class Synth {
public:
  static constexpr int chordsPerBank = 8;
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
  void setNumVoices(int newNumVoices);
  int getNumVoices() const { return numVoices; }
  using Chord = std::vector<int>;
  struct ChordBank {
    std::array<Chord, chordsPerBank> chords;
  };
  void enableChordMode(bool enabled);
  bool isChordModeEnabled() const { return chordModeEnabled; }
  void setActiveChordBank(int bankIndex);
  int getActiveChordBank() const { return activeChordBank; }
  void setChordBanks(std::vector<ChordBank> banks);
  const std::vector<ChordBank>& getChordBanks() const { return chordBanks; }
  void setChord(int bankIndex, int chordIndex, const Chord& chordNotes);
  const Chord* getChord(int bankIndex, int chordIndex) const;
  void previewChord(int bankIndex, int chordIndex, int velocity = 100);
  void stopPreviewChord();
  bool isChordSlotActive(int bankIndex, int chordIndex) const;
  // juce::dsp::StateVariableTPTFilter<float> filter;
  // Voice voice;
  static constexpr int MAX_VOICES = 32;
  int numVoices = MAX_VOICES;
  std::array<Voice, MAX_VOICES> voices{};
  juce::SmoothedValue<float> mixGain{1.0f};

private:
  NoiseGenerator noiseGen;

  juce::dsp::ProcessSpec filterSpec;
  void noteOn(int note, int velocity, bool bypassChordMapping = false);
  void noteOff(int note, bool bypassChordMapping = false);
  int triggerVoice(int note, int velocity);
  void releaseVoice(int note);
  const Chord* fetchChordForMidiNote(int midiNote) const;
  void releaseTrackedChordVoices();
  float sampleRate;
  float volume = 0.5f;
  float baseCutoff = 300.f;
  bool filterEnabled = true;
  bool chordModeEnabled = false;
  int activeChordBank = 0;
  static constexpr int chordBaseNote = 36;  // C2
  std::vector<ChordBank> chordBanks;
  std::unordered_map<int, Chord> activeChordNotes;
  std::unordered_map<int, int> chordSlotsByNote;
  std::unordered_set<int> activeChordSlots;
  std::vector<int> previewVoiceIndices;
  int findFreeVoice();
  int findVoiceByNote(int note);
};
