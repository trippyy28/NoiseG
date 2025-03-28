#pragma once
#include <cmath>
const float TWO_PI = 6.2831853071795864f;

#pragma once

enum WaveformType { SINE, SQUARE, SAW, NOISE };

class Oscillator {
public:
  float amplitude = 0.5f;
  float inc = 0.0f;
  float phase = 0.0f;
  WaveformType waveform = SAW;  // Default waveform

  void reset() { phase = 0.0f; }

  float nextSample() {
    phase += inc;
    if (phase >= 1.0f) {
      phase -= 1.0f;
    }

    switch (waveform) {
      case SINE:
        return amplitude * std::sin(TWO_PI * phase);
      case SQUARE:
        return amplitude * (phase < 0.5f ? 1.0f : -1.0f);  // Square wave
      case SAW:
        return amplitude * (2.0f * phase - 1.0f);  // Saw wave
      case NOISE:
        return amplitude *
               (2.0f * static_cast<float>(rand()) / RAND_MAX - 1.0f);  // Noise
    }
    return 0.0f;  // Should never reach here
  }
};
