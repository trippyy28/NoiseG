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
  for (int sample = 0; sample < sampleCount; ++sample) {
    float rawOutput = voice.render();
    float left = rawOutput;
    float right = rawOutput;

    if (filterEnabled) {
      float filterEnvValue = voice.filterEnvelope.getNextSample();  // 0–1
      float mod = filterEnvValue * voice.filterModAmount;
      float modulatedCutoff =
          juce::jlimit(20.0f, 20000.0f, baseCutoff + mod * 5000.0f);
      filter.setCutoffFrequency(modulatedCutoff);

      left = filter.processSample(0, rawOutput);
      right = filter.processSample(1, rawOutput);
    }

    outputBuffers[0][sample] = left;
    outputBuffers[1][sample] = right;
  }

  protectYourEars(outputBuffers[0], sampleCount);
  protectYourEars(outputBuffers[1], sampleCount);
}

void Synth::allocateResources(double sampleRate_, int /*samplesPerBlock*/) {
  voice.ampEnvelope.setSampleRate(sampleRate);
  voice.filterEnvelope.setSampleRate(sampleRate);
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
  voice.osc.freq = 440.0f * std::exp2(float(note - 69) / 12.0f);
  voice.osc.sampleRate = sampleRate;
  voice.osc.amplitude = (velocity / 127.0f) * volume;
  voice.osc.inc = voice.osc.freq / sampleRate;
  voice.osc.waveform = waveform;
  voice.osc.reset();

  voice.ampEnvelope.setParameters(voice.ampParams);
  voice.ampEnvelope.noteOn();
  voice.filterEnvelope.setParameters(voice.filterParams);
  voice.filterEnvelope.noteOn();
}

void Synth::noteOff(int note) {
  if (voice.note == note) {
    voice.ampEnvelope.noteOff();
    voice.filterEnvelope.noteOff();
    voice.note = 0;
  }
}

void Synth::setVolume(float vol) {
  volume = vol;
}
void Synth::setWaveform(WaveformType wf) {
  waveform = wf;
  voice.osc.waveform = wf;
}
void Synth::setCutoff(float freq) {
  baseCutoff = freq;  // רק שומר את הערך, לא שולח אותו לפילטר
}

void Synth::setFilterResonance(float q) {
  filter.setResonance(q);
}
void Synth::setFilterEnabled(bool shouldEnable) {
  filterEnabled = shouldEnable;
}
void Synth::setFilterModAmount(float amount) {
  voice.setFilterModAmount(amount);
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
