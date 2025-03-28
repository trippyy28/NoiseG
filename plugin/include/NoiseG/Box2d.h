#pragma once

#include <juce_graphics/juce_graphics.h>

class Box2DRenderer {
public:
  void drawCircle(float x,
                  float y,
                  float radius,
                  const juce::Colour& colour,
                  float thickness);
};