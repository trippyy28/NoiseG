#include "NoiseG/MyVoice.h"

MyVoice::MyVoice() {
  ampEnvelope.setSampleRate(sampleRate);
  filterEnvelope.setSampleRate(sampleRate);
}

bool MyVoice::canPlaySound(juce::SynthesiserSound* sound) {
  return true;  // או תנאי אם תיצור מחלקת sound משלך
}

void MyVoice::startNote(int midiNoteNumber,
                        float velocity,
                        juce::SynthesiserSound*,
                        int) {
  float freq = 440.0f * std::pow(2.0f, (midiNoteNumber - 69) / 12.0f);
  osc.waveform = waveform;
  osc.reset();
  osc.amplitude = velocity * volume;
  osc.inc = freq / sampleRate;

  ampEnvelope.noteOn();
  filterEnvelope.noteOn();
}

void MyVoice::stopNote(float, bool allowTailOff) {
  ampEnvelope.noteOff();
  filterEnvelope.noteOff();

  if (!allowTailOff || !ampEnvelope.isActive()) {
    clearCurrentNote();
  }
}

void MyVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                              int startSample,
                              int numSamples) {
  if (!isPrepared) {
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<uint32_t>(numSamples);
    spec.numChannels = 2;
    filter.prepare(spec);
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    isPrepared = true;
  }

  for (int sample = 0; sample < numSamples; ++sample) {
    float raw = osc.nextSample();
    float envAmp = ampEnvelope.getNextSample();
    float envFilt = filterEnvelope.getNextSample();

    float cutoff = baseCutoff + envFilt * modAmount * 5000.0f;
    cutoff = juce::jlimit(20.0f, 20000.0f, cutoff);
    filter.setCutoffFrequency(cutoff);
    filter.setResonance(reso);

    float filtered = filter.processSample(0, raw * envAmp);

    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
      outputBuffer.addSample(channel, startSample + sample, filtered);
    }
  }
}

void MyVoice::setWaveform(WaveformType wf) {
  waveform = wf;
}

void MyVoice::setVolume(float vol) {
  volume = vol;
}

void MyVoice::setFilterParams(float cutoff, float res, float modAmt) {
  baseCutoff = cutoff;
  reso = res;
  modAmount = modAmt;
}

void MyVoice::setAmpADSR(const juce::ADSR::Parameters& params) {
  ampEnvelope.setParameters(params);
}

void MyVoice::setFilterADSR(const juce::ADSR::Parameters& params) {
  filterEnvelope.setParameters(params);
}