#pragma once
#include "Oscillator.h"

struct Voice {
  int note;
  float amplitude;

  Oscillator osc;
  void reset() {
    note = 0;
    amplitude = 0.0f;
    osc.reset();
  }
  float render() { return osc.nextSample(); }
};