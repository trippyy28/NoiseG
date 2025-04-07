#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class NinjaAnimator : public juce::Component, public juce::Timer {
public:
  NinjaAnimator(juce::Image spriteSheet);
  void paint(juce::Graphics& g) override;
  void timerCallback() override;
  void setTotalFrames(int frames);
  void setAnimationPlaying(bool shouldPlay);
  int totalFrames = 2;

private:
  juce::Image sheet;
  int currentFrame = 0;
  double totalTime = 0.0;
  double switchTime =
      0.1;  // זמן במילים של שניות בין פריימים (0.1 שניות = 10FPS)
  juce::Time lastUpdateTime;
};