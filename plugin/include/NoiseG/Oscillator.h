#pragma once
#include <cmath>
const float TWO_PI = 6.2831853071795864f;

#pragma once

enum WaveformType { SINE, SQUARE, SAW, NOISE };

class Oscillator {
private:
  float sin0;
  float sin1;
  float dsin;

public:
  float amplitude = 0.5f;
  float inc = 0.0f;
  float phase = 0.0f;
  float freq;
  float sampleRate;
  float phaseBL;
  WaveformType waveform = SAW;

  // Default waveform

  void reset() {
    phase = 0.0f;

    sin0 = amplitude * std::sin(phase * TWO_PI);
    sin1 = amplitude * std::sin((phase - inc) * TWO_PI);
    dsin = 2.0f * std::cos(inc * TWO_PI);
  }
  float nextBandlimitedSample() {
    phaseBL += inc;
    if (phaseBL >= 1.0f) {
      phaseBL -= 1.0f;
    }
    float output = 0.0f;
    float nyquist = sampleRate / 2.0f;
    float h = freq;
    float i = 1.0f;
    float m = 0.6366197724f;  // this is 2/pi while (h < nyquist) {
    while (h < nyquist) {
      output += m * std::sin(TWO_PI * phaseBL * i) / i;
      h += freq;
      i += 1.0f;
      m = -m;
    }
    return output;
  }

  float nextSample() {
    phase += inc;
    if (phase >= 1.0f) {
      phase -= 1.0f;
    }

    float sinx = dsin * sin0 - sin1;
    sin1 = sin0;
    sin0 = sinx;
    switch (waveform) {
      case SINE:
        return sinx;
      case SQUARE:
        return amplitude * (phase < 0.5f ? 1.0f : -1.0f);  // Square wave
      case SAW:
        return amplitude * nextBandlimitedSample();  // Saw wave
      case NOISE:
        return amplitude *
               (2.0f * static_cast<float>(rand()) / RAND_MAX - 1.0f);  // Noise
    }
    return 0.0f;  // Should never reach here
  }
};