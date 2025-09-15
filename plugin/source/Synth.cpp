#include "NoiseG/Synth.h"
#include "NoiseG/Utils.h"
#include <iostream>

Synth::Synth() {
  sampleRate = 44100.0f;
}

Synth::~Synth() {}

void Synth::reset() {
  for (auto& v : voices)
    v.reset();
  noiseGen.reset();
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
    for (auto& v : voices) {
      if (v.isActive())
        mix += v.renderSample(baseCutoff, filterEnabled);
    }
    outputBuffers[0][n] = mix;
    outputBuffers[1][n] = mix;
  }

  protectYourEars(outputBuffers[0], sampleCount);
  protectYourEars(outputBuffers[1], sampleCount);
}
void Synth::allocateResources(double sampleRate_, int samplesPerBlock) {
  sampleRate = static_cast<float>(sampleRate_);  // set first!

  filterSpec.sampleRate = sampleRate;
  filterSpec.maximumBlockSize = (juce::uint32)samplesPerBlock;
  filterSpec.numChannels = 1;  // per-voice filter is mono

  for (auto& v : voices) {
    v.osc.sampleRate = sampleRate;
    v.ampEnvelope.setSampleRate(sampleRate);
    v.filterEnvelope.setSampleRate(sampleRate);
    v.filter.prepare(filterSpec);
    v.filter.reset();
    v.filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    v.filter.setCutoffFrequency(baseCutoff);
    v.reset();
  }
  noiseGen.reset();
}

void Synth::noteOn(int note, int velocity) {
  int vi = findFreeVoice();
  auto& v = voices[vi];

  v.note = note;
  v.held = true;

  v.osc.freq = 440.0f * std::exp2((float)(note - 69) / 12.0f);
  v.osc.amplitude = (velocity / 127.0f) * volume;
  v.osc.inc = v.osc.freq / sampleRate;
  v.osc.waveform = waveform;
  v.osc.reset();

  v.ampEnvelope.setParameters(v.ampParams);
  v.ampEnvelope.noteOn();

  v.filterEnvelope.setParameters(v.filterParams);
  v.filterEnvelope.noteOn();

  v.filter.setCutoffFrequency(baseCutoff);
}

void Synth::noteOff(int note) {
  int vi = findVoiceByNote(note);
  if (vi < 0)
    return;
  auto& v = voices[vi];
  v.held = false;
  v.ampEnvelope.noteOff();
  v.filterEnvelope.noteOff();
  v.note = -1;  // mark as available; will fade during Release
}
int Synth::findFreeVoice() {
  // 1) free (inactive)
  for (int i = 0; i < MAX_VOICES; ++i)
    if (!voices[i].isActive())
      return i;
  // 2) steal one not held (release stage)
  for (int i = 0; i < MAX_VOICES; ++i)
    if (!voices[i].held)
      return i;
  // 3) fallback
  return 0;
}

int Synth::findVoiceByNote(int note) {
  for (int i = 0; i < MAX_VOICES; ++i)
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
  for (auto& v : voices)
    v.osc.waveform = wf;
}

void Synth::setFilterResonance(float q) {
  for (auto& v : voices)
    v.filter.setResonance(q);
}

void Synth::setFilterModAmount(float amount) {
  for (auto& v : voices)
    v.setFilterModAmount(amount);
}

void Synth::setFilterEnabled(bool shouldEnable) {
  filterEnabled = shouldEnable;
  if (!shouldEnable) {
    // Ensure no stale state when bypassing
    for (auto& v : voices)
      v.filter.reset();
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
