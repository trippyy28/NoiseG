#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
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
};
