#include "NoiseG/NinjaAnimator.h"

NinjaAnimator::NinjaAnimator(juce::Image spriteSheet) : sheet(spriteSheet) {
  startTimerHz(10);  // 10 frames לשנייה
}

void NinjaAnimator::setTotalFrames(int frames) {
  totalFrames = frames;
}

void NinjaAnimator::paint(juce::Graphics& g) {
  int frameWidth = 48;
  int frameHeight = 64;

  // צייר את ה-frame הנוכחי מתוך הספרייט שיט
  g.drawImage(sheet, 0, 0, frameWidth, frameHeight,                    // יעד
              currentFrame * frameWidth, 0, frameWidth, frameHeight);  // מקור
}

void NinjaAnimator::timerCallback() {
  ++currentFrame;
  if (currentFrame >= totalFrames)
    currentFrame = 0;

  repaint();
}