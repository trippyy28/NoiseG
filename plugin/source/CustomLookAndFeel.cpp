#include "NoiseG/CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel() {
  setColour(juce::ComboBox::backgroundColourId, juce::Colours::black);
  setColour(juce::ComboBox::textColourId, juce::Colours::limegreen);
  setColour(juce::ComboBox::outlineColourId, juce::Colours::orange);
  setColour(juce::ComboBox::buttonColourId, juce::Colours::red);
  setColour(juce::PopupMenu::backgroundColourId, juce::Colours::darkgrey);
  setColour(juce::PopupMenu::highlightedBackgroundColourId,
            juce::Colours::green);
  setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::black);
}

void CustomLookAndFeel::drawComboBox(juce::Graphics& g,
                                     int width,
                                     int height,
                                     bool isButtonDown,
                                     int buttonX,
                                     int buttonY,
                                     int buttonW,
                                     int buttonH,
                                     juce::ComboBox& box) {
  g.fillAll(findColour(juce::ComboBox::backgroundColourId));
  g.setColour(findColour(juce::ComboBox::outlineColourId));
  g.drawRect(0, 0, width, height, 2);
}
void CustomLookAndFeel::drawPopupMenuItem(juce::Graphics& g,
                                          const juce::Rectangle<int>& area,
                                          bool isSeparator,
                                          bool isActive,
                                          bool isHighlighted,
                                          bool isTicked,
                                          bool hasSubMenu,
                                          const juce::String& text,
                                          const juce::String& shortcutKeyText,
                                          const juce::Drawable* icon,
                                          const juce::Colour* textColour) {
  if (isSeparator) {
    g.setColour(juce::Colours::darkgrey);
    g.drawLine((float)area.getX(), (float)(area.getCentreY()),
               (float)area.getRight(), (float)(area.getCentreY()));
    return;
  }

  juce::Colour textCol =
      isHighlighted
          ? juce::Colours::black
          : (textColour != nullptr ? *textColour : juce::Colours::limegreen);

  if (isHighlighted) {
    g.setColour(juce::Colours::green);
    g.fillRect(area.reduced(1));
  }

  g.setColour(textCol);

  static juce::Typeface::Ptr pixelFont =
      juce::Typeface::createSystemTypefaceFor(BinaryData::ffont_ttf,
                                              BinaryData::ffont_ttfSize);
  juce::Font menuFont(pixelFont);
  menuFont.setHeight(16.0f);

  g.setFont(menuFont);

  auto textArea = area.reduced(6, 0);
  g.drawFittedText(text, textArea, juce::Justification::centredLeft, 1);
}

juce::Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox& box) {
  return juce::Font("Arial", 16.0f, juce::Font::bold);
}