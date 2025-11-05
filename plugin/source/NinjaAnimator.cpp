#include "NoiseG/NinjaAnimator.h"

NinjaAnimator::NinjaAnimator(juce::Image spriteSheet) : sheet(spriteSheet) {}

void NinjaAnimator::setTotalFrames(int frames) {
  totalFrames = frames;
}

void NinjaAnimator::paint(juce::Graphics& g) {
  int frameWidth = 16;
  int frameHeight = 16;

  int scaleFactor = 3;
  int targetWidth = frameWidth * scaleFactor;
  int targetHeight = frameHeight * scaleFactor;
  int x = (getWidth() - targetWidth) / 2;
  int y = (getHeight() - targetHeight) / 2;

  g.drawImage(sheet, x, y, targetWidth, targetHeight, currentFrame * frameWidth,
              32, frameWidth, frameHeight);  // ← מקור
}
void NinjaAnimator::timerCallback() {
  auto now = juce::Time::getCurrentTime();
  auto deltaTime =
      (now.toMilliseconds() - lastUpdateTime.toMilliseconds()) / 1000.0;

  lastUpdateTime = now;
  totalTime += deltaTime;

  if (totalTime >= switchTime) {
    totalTime -= switchTime;
    ++currentFrame;
    if (currentFrame >= totalFrames)
      currentFrame = 0;
    repaint();
  }
}

void NinjaAnimator::setAnimationPlaying(bool shouldPlay) {
  if (shouldPlay) {
    lastUpdateTime = juce::Time::getCurrentTime();
    totalTime = 0.0;
    startTimerHz(30);
  } else {
    stopTimer();
    currentFrame = 0;
    totalTime = 0.0;
    repaint();
  }
}
