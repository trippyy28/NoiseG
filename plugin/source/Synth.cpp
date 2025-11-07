#include "NoiseG/Synth.h"
#include "NoiseG/Utils.h"
#include <iostream>
#include <utility>

Synth::Synth() {
  sampleRate = 44100.0f;
  mixGain.setCurrentAndTargetValue(1.0f);
  chordBanks = {ChordBank{}, ChordBank{}};
  auto& bank0 = chordBanks[0];
  bank0.chords[0] = {48, 52, 55};  // C major
  bank0.chords[1] = {50, 53, 57};  // D minor
  bank0.chords[2] = {52, 55, 59};  // E minor
  bank0.chords[3] = {53, 57, 60};  // F major
  bank0.chords[4] = {55, 59, 62};  // G major
  bank0.chords[5] = {57, 60, 64};  // A minor
  bank0.chords[6] = {59, 62, 65};  // B diminished
  bank0.chords[7] = {60, 64, 67};  // C major (octave)
  auto& bank1 = chordBanks[1];
  bank1.chords[0] = {49, 53, 56};  // C# major
  bank1.chords[1] = {51, 54, 58};  // D# minor
  bank1.chords[2] = {53, 56, 60};  // F minor
  bank1.chords[3] = {54, 59, 61};  // F# major
  bank1.chords[4] = {62, 64, 66};  // G# Major
  bank1.chords[5] = {61, 67, 70};  // G# Major
  bank1.chords[6] = {58, 72, 67};  // G# Major
  bank1.chords[7] = {56, 65, 73};  // G# Major
  auto& bank2 = chordBanks[2];
  bank2.chords[0] = {50, 54, 57};  // C# major
  bank2.chords[1] = {51, 54, 58};  // D# minor
  bank2.chords[2] = {53, 56, 60};  // F minor
  bank2.chords[3] = {54, 59, 61};  // F# major
  bank2.chords[4] = {62, 64, 66};  // G# Major
  bank2.chords[5] = {61, 67, 70};  // G# Major
  bank2.chords[6] = {58, 72, 67};  // G# Major
  bank2.chords[7] = {56, 65, 73};  // G# Major
  enableChordMode(true);
}

Synth::~Synth() {}

void Synth::reset() {
  stopPreviewChord();
  for (auto& v : voices)
    v.reset();
  noiseGen.reset();
  mixGain.setCurrentAndTargetValue(1.0f);
  activeChordNotes.clear();
  chordSlotsByNote.clear();
  activeChordSlots.clear();
  previewVoiceIndices.clear();
}

// void Synth::render(float** outputBuffers, int sampleCount) {
//   juce::dsp::AudioBlock<float> block(outputBuffers, 2, sampleCount);
//   // float* outputBufferLeft = outputBuffers[0];
//   // float* outputBufferRight = outputBuffers[1];
//   juce::dsp::ProcessContextReplacing<float> context(block);

//   for (int sample = 0; sample < sampleCount; ++sample) {
//     float output = 0.0f;
//     if (voice.note > 0) {
//       output = voice.render();
//       output = filter.processSample(
//           0, output);  // keeping this for single sample processing
//     }
//     outputBufferLeft[sample] =
//         filter.processSample(0, outputBufferLeft[sample]);
//     if (outputBufferRight != nullptr) {
//       outputBufferRight[sample] =
//           filter.processSample(1, outputBufferRight[sample]);
//     }
//   }
//   protectYourEars(outputBufferLeft, sampleCount);
//   protectYourEars(outputBufferRight, sampleCount);
// }
void Synth::render(float** outputBuffers, int sampleCount) {
  for (int n = 0; n < sampleCount; ++n) {
    float mix = 0.0f;
    int activeCount = 0;
    float envelopeSum = 0.0f;
    for (int i = 0; i < numVoices; ++i) {
      auto& v = voices[i];
      if (!v.isActive())
        continue;
      mix += v.renderSample(baseCutoff, filterEnabled);
      ++activeCount;
      envelopeSum += v.amplitude;
    }

    float targetGain = 1.0f;
    if (activeCount > 1 && envelopeSum > 1.0f)
      targetGain = 1.0f / envelopeSum;

    mixGain.setTargetValue(targetGain);
    mix *= mixGain.getNextValue();

    outputBuffers[0][n] = mix;
    if (outputBuffers[1] != nullptr)
      outputBuffers[1][n] = mix;
  }

  protectYourEars(outputBuffers[0], sampleCount);
  if (outputBuffers[1] != nullptr)
    protectYourEars(outputBuffers[1], sampleCount);
}
void Synth::allocateResources(double sampleRate_, int samplesPerBlock) {
  sampleRate = static_cast<float>(sampleRate_);  // set first!

  filterSpec.sampleRate = sampleRate;
  filterSpec.maximumBlockSize = (juce::uint32)samplesPerBlock;
  filterSpec.numChannels = 1;  // per-voice filter is mono

  mixGain.reset(sampleRate, 0.01f);  // smooth normalization changes over 10 ms
  mixGain.setCurrentAndTargetValue(1.0f);

  for (auto& v : voices) {
    v.osc.sampleRate = sampleRate;
    v.ampEnvelope.setSampleRate(sampleRate);
    v.filterEnvelope.setSampleRate(sampleRate);
    v.filter.prepare(filterSpec);
    v.filter.reset();
    v.filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    v.filter.setCutoffFrequency(baseCutoff);
    v.filterEnvSmoother.reset(sampleRate, 0.005);
    v.filterEnvSmoother.setCurrentAndTargetValue(0.0f);
    v.reset();
  }
  noiseGen.reset();
}

void Synth::noteOn(int note, int velocity, bool bypassChordMapping) {
  if (!bypassChordMapping) {
    if (const auto* chord = fetchChordForMidiNote(note)) {
      if (!chord->empty()) {
        auto& triggered = activeChordNotes[note];
        triggered.clear();
        triggered.reserve(chord->size());
        int slotIndex = note - chordBaseNote;
        for (int chordNote : *chord) {
          int clampedNote = juce::jlimit(0, 127, chordNote);
          int voiceIdx = triggerVoice(clampedNote, velocity);
          if (voiceIdx >= 0)
            triggered.push_back(clampedNote);
        }
        if (triggered.empty()) {
          activeChordNotes.erase(note);
          activeChordSlots.erase(slotIndex);
          chordSlotsByNote.erase(note);
        } else {
          activeChordSlots.insert(slotIndex);
          chordSlotsByNote[note] = slotIndex;
        }
        return;
      }
    }
  }
  triggerVoice(note, velocity);
}

int Synth::triggerVoice(int note, int velocity) {
  int vi = findFreeVoice();
  if (vi < 0)
    return -1;
  auto& v = voices[vi];

  v.note = note;
  v.held = true;
  v.preview = false;

  v.osc.freq = 440.0f * std::exp2((float)(note - 69) / 12.0f);
  v.osc.amplitude = (velocity / 127.0f) * volume;
  v.osc.inc = v.osc.freq / sampleRate;
  v.osc.waveform = waveform;
  v.osc.reset();

  v.ampEnvelope.setParameters(v.ampParams);
  v.ampEnvelope.noteOn();

  v.filterEnvelope.setParameters(v.filterParams);
  v.filterEnvelope.noteOn();
  v.filterEnvSmoother.setCurrentAndTargetValue(0.0f);

  v.filter.setCutoffFrequency(baseCutoff);
  return vi;
}

void Synth::noteOff(int note, bool bypassChordMapping) {
  if (!bypassChordMapping) {
    auto it = activeChordNotes.find(note);
    if (it != activeChordNotes.end()) {
      for (int chordNote : it->second)
        noteOff(chordNote, true);
      activeChordNotes.erase(it);
      auto slotIt = chordSlotsByNote.find(note);
      if (slotIt != chordSlotsByNote.end()) {
        activeChordSlots.erase(slotIt->second);
        chordSlotsByNote.erase(slotIt);
      }
      return;
    }
  }
  auto slotIt = chordSlotsByNote.find(note);
  if (slotIt != chordSlotsByNote.end()) {
    activeChordSlots.erase(slotIt->second);
    chordSlotsByNote.erase(slotIt);
  }
  releaseVoice(note);
}

void Synth::releaseVoice(int note) {
  int vi = findVoiceByNote(note);
  if (vi < 0)
    return;
  auto& v = voices[vi];
  v.held = false;
  v.ampEnvelope.noteOff();
  v.filterEnvelope.noteOff();
  v.preview = false;
  v.note = -1;  // mark as available; will fade during Release
}
int Synth::findFreeVoice() {
  // 1) free (inactive)
  for (int i = 0; i < numVoices; ++i)
    if (!voices[i].isActive())
      return i;
  // 2) steal one not held (release stage)
  for (int i = 0; i < numVoices; ++i)
    if (!voices[i].held)
      return i;
  // 3) fallback
  return numVoices > 0 ? 0 : -1;
}

int Synth::findVoiceByNote(int note) {
  for (int i = 0; i < numVoices; ++i)
    if (voices[i].note == note)
      return i;
  return -1;
}

void Synth::setVolume(float vol) {
  volume = vol;
}
void Synth::setCutoff(float freq) {
  baseCutoff = freq;  // רק שומר את הערך, לא שולח אותו לפילטר
}

void Synth::setWaveform(WaveformType wf) {
  waveform = wf;
  for (int i = 0; i < MAX_VOICES; ++i)
    voices[i].osc.waveform = wf;
}

void Synth::setFilterResonance(float q) {
  for (int i = 0; i < MAX_VOICES; ++i)
    voices[i].filter.setResonance(q);
}

void Synth::setFilterModAmount(float amount) {
  for (int i = 0; i < MAX_VOICES; ++i)
    voices[i].setFilterModAmount(amount);
}

void Synth::setFilterEnabled(bool shouldEnable) {
  filterEnabled = shouldEnable;
  if (!shouldEnable) {
    // Ensure no stale state when bypassing
    for (int i = 0; i < MAX_VOICES; ++i)
      voices[i].filter.reset();
  }
}

void Synth::deallocateResources() {
  // אם יש שחרור משאבים יש להוסיף כאן
}

void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2) {
  switch (data0 & 0xF0) {
    case 0x80:
      noteOff(data1 & 0x7F);
      break;
    case 0x90: {
      uint8_t note = data1 & 0x7F;
      uint8_t velo = data2 & 0x7F;
      if (velo > 0) {
        noteOn(note, velo);
      } else {
        noteOff(note);
      }
      break;
    }
  }
}

void Synth::setNumVoices(int newNumVoices) {
  int clamped = juce::jlimit(1, MAX_VOICES, newNumVoices);
  if (clamped == numVoices)
    return;

  stopPreviewChord();
  if (clamped < numVoices) {
    for (int i = clamped; i < numVoices; ++i)
      voices[i].reset();
  }

  numVoices = clamped;
}

void Synth::enableChordMode(bool enabled) {
  if (chordModeEnabled == enabled)
    return;
  if (!enabled) {
    stopPreviewChord();
    releaseTrackedChordVoices();
  }
  chordModeEnabled = enabled;
}

void Synth::setActiveChordBank(int bankIndex) {
  if (chordBanks.empty()) {
    activeChordBank = 0;
    return;
  }
  int clamped =
      juce::jlimit(0, static_cast<int>(chordBanks.size()) - 1, bankIndex);
  if (clamped == activeChordBank)
    return;
  stopPreviewChord();
  releaseTrackedChordVoices();
  activeChordBank = clamped;
}

void Synth::setChordBanks(std::vector<ChordBank> banks) {
  chordBanks = std::move(banks);
  if (chordBanks.empty())
    chordBanks.resize(1);
  stopPreviewChord();
  setActiveChordBank(juce::jlimit(0, static_cast<int>(chordBanks.size()) - 1,
                                  activeChordBank));
}

void Synth::setChord(int bankIndex, int chordIndex, const Chord& chordNotes) {
  if (bankIndex < 0 || chordIndex < 0 || chordIndex >= chordsPerBank)
    return;
  if (bankIndex >= static_cast<int>(chordBanks.size()))
    chordBanks.resize(bankIndex + 1);
  chordBanks[bankIndex].chords[chordIndex] = chordNotes;
}

const Synth::Chord* Synth::getChord(int bankIndex, int chordIndex) const {
  if (bankIndex < 0 || chordIndex < 0 || chordIndex >= chordsPerBank)
    return nullptr;
  if (bankIndex >= static_cast<int>(chordBanks.size()))
    return nullptr;
  const auto& chord = chordBanks[bankIndex].chords[chordIndex];
  if (chord.empty())
    return nullptr;
  return &chord;
}

void Synth::previewChord(int bankIndex, int chordIndex, int velocity) {
  stopPreviewChord();
  if (bankIndex < 0 || chordIndex < 0 || chordIndex >= chordsPerBank)
    return;
  if (bankIndex >= static_cast<int>(chordBanks.size()))
    return;
  const auto& chord = chordBanks[bankIndex].chords[chordIndex];
  if (chord.empty())
    return;
  previewVoiceIndices.clear();
  previewVoiceIndices.reserve(chord.size());
  for (int chordNote : chord) {
    int clampedNote = juce::jlimit(0, 127, chordNote);
    int voiceIndex = triggerVoice(clampedNote, velocity);
    if (voiceIndex >= 0) {
      auto& voice = voices[(size_t)voiceIndex];
      voice.preview = true;
      previewVoiceIndices.push_back(voiceIndex);
    }
  }
}

void Synth::stopPreviewChord() {
  if (previewVoiceIndices.empty())
    return;
  for (int voiceIndex : previewVoiceIndices) {
    if (voiceIndex < 0 || voiceIndex >= numVoices)
      continue;
    auto& voice = voices[(size_t)voiceIndex];
    if (!voice.preview)
      continue;
    voice.preview = false;
    voice.held = false;
    voice.ampEnvelope.noteOff();
    voice.filterEnvelope.noteOff();
    voice.note = -1;
  }
  previewVoiceIndices.clear();
}

const Synth::Chord* Synth::fetchChordForMidiNote(int midiNote) const {
  if (!chordModeEnabled)
    return nullptr;
  if (midiNote < chordBaseNote)
    return nullptr;
  int offset = midiNote - chordBaseNote;
  if (offset >= chordsPerBank)
    return nullptr;
  if (activeChordBank < 0 ||
      activeChordBank >= static_cast<int>(chordBanks.size()))
    return nullptr;
  const auto& chord = chordBanks[activeChordBank].chords[offset];
  if (chord.empty())
    return nullptr;
  return &chord;
}

void Synth::releaseTrackedChordVoices() {
  stopPreviewChord();
  for (auto& entry : activeChordNotes) {
    for (int chordNote : entry.second)
      releaseVoice(chordNote);
  }
  activeChordNotes.clear();
  activeChordSlots.clear();
  chordSlotsByNote.clear();
}

bool Synth::isChordSlotActive(int bankIndex, int chordIndex) const {
  if (bankIndex != activeChordBank)
    return false;
  return activeChordSlots.find(chordIndex) != activeChordSlots.end();
}
