#include "NoiseG/Synth.h"
#include "NoiseG/Utils.h"
#include <iostream>

Synth::Synth() {
  sampleRate = 44100.0f;
}

Synth::~Synth() {}

void Synth::reset() {
  voice.reset();
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
  juce::dsp::AudioBlock<float> block(outputBuffers, 2, sampleCount);
  juce::dsp::ProcessContextReplacing<float> context(block);

  if (voice.note > 0) {
    for (int sample = 0; sample < sampleCount; ++sample) {
      float output = voice.render();
      outputBuffers[0][sample] = output;
      outputBuffers[1][sample] = output;
    }
  } else {
    std::fill(outputBuffers[0], outputBuffers[0] + sampleCount, 0.0f);
    std::fill(outputBuffers[1], outputBuffers[1] + sampleCount, 0.0f);
  }

  if (filterEnabled)
    filter.process(context);

  protectYourEars(outputBuffers[0], sampleCount);
  protectYourEars(outputBuffers[1], sampleCount);
}

void Synth::allocateResources(double sampleRate_, int /*samplesPerBlock*/) {
  sampleRate = static_cast<float>(sampleRate_);
  filterSpec.sampleRate = sampleRate;
  filterSpec.maximumBlockSize = 512;
  filterSpec.numChannels = 2;
  filter.prepare(filterSpec);
  filter.reset();
  filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
  filter.setCutoffFrequency(1000.0f);
}

void Synth::noteOn(int note, int velocity) {
  voice.note = note;
  float freq = 440.0f * std::exp2(float(note - 69) / 12.0f);
  voice.osc.amplitude = (velocity / 127.0f) * volume;
  voice.osc.inc = freq / sampleRate;
  voice.osc.waveform = waveform;
  voice.osc.reset();
}

void Synth::noteOff(int note) {
  if (voice.note == note) {
    voice.note = 0;
  }
}

void Synth::setVolume(float vol) {
  volume = vol;
  DBG("Synth Volume: " << vol);
  DBG("Synth Amplitude: " << voice.osc.amplitude);
}
void Synth::setWaveform(WaveformType wf) {
  waveform = wf;
  voice.osc.waveform = wf;
  DBG("Synth Waveform: " << wf);
}
void Synth::setCutoff(float freq) {
  filter.setCutoffFrequency(freq);
}
void Synth::setFilterEnabled(bool shouldEnable) {
  filterEnabled = shouldEnable;
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
