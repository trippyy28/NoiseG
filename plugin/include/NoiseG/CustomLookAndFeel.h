#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "BinaryData.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
  CustomLookAndFeel();

  void drawComboBox(juce::Graphics& g,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    juce::ComboBox& box) override;
  void drawToggleButton(juce::Graphics& g,
                        juce::ToggleButton& button,
                        bool isMouseOverButton,
                        bool isButtonDown);

  void drawLinearSlider(juce::Graphics& g,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPos,
                        float minSliderPos,
                        float maxSliderPos,
                        const juce::Slider::SliderStyle style,
                        juce::Slider& slider) override;

  juce::Font getComboBoxFont(juce::ComboBox& box) override;
  void drawPopupMenuItem(juce::Graphics& g,
                         const juce::Rectangle<int>& area,
                         bool isSeparator,
                         bool isActive,
                         bool isHighlighted,
                         bool isTicked,
                         bool hasSubMenu,
                         const juce::String& text,
                         const juce::String& shortcutKeyText,
                         const juce::Drawable* icon,
                         const juce::Colour* textColour) override;

private:
  juce::Image sliderThumb;
  bool drawThumbImage(juce::Graphics& g, juce::Point<float> centre) const;
};
