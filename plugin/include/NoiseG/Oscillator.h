#pragma once
#include <cmath>
#include <cstdlib>

const float TWO_PI = 6.2831853071795864f;

enum WaveformType { SINE, SQUARE, SAW, NOISE };

// Simple, normalized-phase oscillator.
// - Set `inc = freq / sampleRate` before calling nextSample.
// - `phase` stays in [0, 1).
class Oscillator {
public:
  float amplitude = 0.5f;
  float inc = 0.0f;     // phase increment (cycles per sample)
  float phase = 0.0f;   // normalized phase [0, 1)
  float freq = 0.0f;    // optional
  float sampleRate = 0; // optional
  WaveformType waveform = SAW;

  void reset() { phase = 0.0f; }

  float nextSample() {
    // advance phase
    phase += inc;
    if (phase >= 1.0f)
      phase -= 1.0f;

    switch (waveform) {
      case SINE:
        return amplitude * std::sin(TWO_PI * phase);
      case SQUARE:
        return amplitude * (phase < 0.5f ? 1.0f : -1.0f);
      case SAW:
        // bipolar saw in [-1, 1]
        return amplitude * (2.0f * phase - 1.0f);
      case NOISE:
        return amplitude * (2.0f * static_cast<float>(std::rand()) / RAND_MAX - 1.0f);
    }
    return 0.0f;
  }
};
