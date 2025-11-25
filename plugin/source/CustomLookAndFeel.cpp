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
  sliderThumb = juce::ImageCache::getFromMemory(BinaryData::thumb33_png,
                                                BinaryData::thumb33_pngSize);
  if (sliderThumb.isValid())
    DBG("thumb size: " << sliderThumb.getWidth() << "x"
                       << sliderThumb.getHeight());
  else
    DBG("thumb failed to load");
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
  menuFont.setHeight(15.0f);

  g.setFont(menuFont);

  auto textArea = area.reduced(2, 0);
  g.drawFittedText(text, textArea, juce::Justification::centredLeft, 1);
}
void CustomLookAndFeel::drawToggleButton(juce::Graphics& g,
                                         juce::ToggleButton& button,
                                         bool isMouseOverButton,
                                         bool isButtonDown) {
  auto bounds = button.getLocalBounds().toFloat().reduced(4.0f);

  // צבעים בהתאמה למצב הכפתור
  auto baseColour = button.getToggleState() ? juce::Colours::limegreen
                                            : juce::Colours::darkslategrey;
  auto borderColour =
      isMouseOverButton ? juce::Colours::yellow : juce::Colours::black;

  // רקע
  g.setColour(baseColour);
  g.fillRect(bounds);

  // מסגרת
  g.setColour(borderColour);
  g.drawRect(bounds, 1.5f);

  // טקסט
  g.setColour(juce::Colours::white);
  static juce::Typeface::Ptr pixelFont =
      juce::Typeface::createSystemTypefaceFor(BinaryData::ffont_ttf,
                                              BinaryData::ffont_ttfSize);
  juce::Font font(pixelFont);
  font.setHeight(12.0f);
  g.setFont(font);

  g.drawFittedText(button.getButtonText(), bounds.toNearestInt(),
                   juce::Justification::centred, 1);
}

juce::Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox& box) {
  return juce::Font("Arial", 16.0f, juce::Font::bold);
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g,
                                         int x,
                                         int y,
                                         int width,
                                         int height,
                                         float sliderPos,
                                         float minSliderPos,
                                         float maxSliderPos,
                                         const juce::Slider::SliderStyle style,
                                         juce::Slider& slider) {
  if (slider.isBar()) {
    g.setColour(slider.findColour(juce::Slider::trackColourId));
    auto rect =
        slider.isHorizontal()
            ? juce::Rectangle<float>((float)x, (float)y + 0.5f,
                                     sliderPos - (float)x, (float)height - 1.0f)
            : juce::Rectangle<float>((float)x + 0.5f, sliderPos,
                                     (float)width - 1.0f,
                                     (float)y + ((float)height - sliderPos));
    g.fillRect(rect);
    return;
  }

  bool isTwoVal = (style == juce::Slider::SliderStyle::TwoValueVertical ||
                   style == juce::Slider::SliderStyle::TwoValueHorizontal);
  bool isThreeVal = (style == juce::Slider::SliderStyle::ThreeValueVertical ||
                     style == juce::Slider::SliderStyle::ThreeValueHorizontal);

  auto trackWidth =
      juce::jmin(6.0f, slider.isHorizontal() ? (float)height * 0.12f
                                             : (float)width * 0.12f);

  juce::Point<float> startPoint(
      slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
      slider.isHorizontal() ? (float)y + (float)height * 0.5f
                            : (float)(height + y));

  juce::Point<float> endPoint(
      slider.isHorizontal() ? (float)(width + x) : startPoint.x,
      slider.isHorizontal() ? startPoint.y : (float)y);
  juce::Path backgroundTrack;
  backgroundTrack.startNewSubPath(startPoint);
  backgroundTrack.lineTo(endPoint);
  g.setColour(slider.findColour(juce::Slider::backgroundColourId));
  g.strokePath(backgroundTrack, {trackWidth, juce::PathStrokeType::curved,
                                 juce::PathStrokeType::rounded});

  juce::Path valueTrack;
  juce::Point<float> minPoint, maxPoint, thumbPoint;

  if (isTwoVal || isThreeVal) {
    minPoint = {slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
                slider.isHorizontal() ? (float)height * 0.5f : minSliderPos};

    if (isThreeVal) {
      thumbPoint = {slider.isHorizontal() ? sliderPos : (float)width * 0.5f,
                    slider.isHorizontal() ? (float)height * 0.5f : sliderPos};
    }

    maxPoint = {slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
                slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos};
  } else {
    auto kx =
        slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
    auto ky =
        slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

    minPoint = startPoint;
    maxPoint = {kx, ky};
  }

  auto thumbWidth = getSliderThumbRadius(slider);

  valueTrack.startNewSubPath(minPoint);
  valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
  g.setColour(slider.findColour(juce::Slider::trackColourId));
  g.strokePath(valueTrack, {trackWidth, juce::PathStrokeType::curved,
                            juce::PathStrokeType::rounded});

  if (!isTwoVal) {
    if (!drawThumbImage(g, isThreeVal ? thumbPoint : maxPoint)) {
      g.setColour(slider.findColour(juce::Slider::thumbColourId));
      g.fillEllipse(juce::Rectangle<float>((float)thumbWidth, (float)thumbWidth)
                        .withCentre(isThreeVal ? thumbPoint : maxPoint));
    }
  }

  if (isTwoVal || isThreeVal) {
    auto sr = juce::jmin(
        trackWidth,
        (slider.isHorizontal() ? (float)height : (float)width) * 0.4f);
    auto pointerColour = slider.findColour(juce::Slider::thumbColourId);

    if (slider.isHorizontal()) {
      juce::LookAndFeel_V4::drawPointer(
          g, minSliderPos - sr,
          juce::jmax(0.0f, (float)y + (float)height * 0.5f - trackWidth * 2.0f),
          trackWidth * 2.0f, pointerColour, 2);

      juce::LookAndFeel_V4::drawPointer(
          g, maxSliderPos - trackWidth,
          juce::jmin((float)(y + height) - trackWidth * 2.0f,
                     (float)y + (float)height * 0.5f),
          trackWidth * 2.0f, pointerColour, 4);
    } else {
      juce::LookAndFeel_V4::drawPointer(
          g,
          juce::jmax(0.0f, (float)x + (float)width * 0.5f - trackWidth * 2.0f),
          minSliderPos - trackWidth, trackWidth * 2.0f, pointerColour, 1);

      juce::LookAndFeel_V4::drawPointer(
          g,
          juce::jmin((float)(x + width) - trackWidth * 2.0f,
                     (float)x + (float)width * 0.5f),
          maxSliderPos - sr, trackWidth * 2.0f, pointerColour, 3);
    }
  }
}

bool CustomLookAndFeel::drawThumbImage(juce::Graphics& g,
                                       juce::Point<float> centre) const {
  if (!sliderThumb.isValid())
    return false;
  auto destBounds = juce::Rectangle<float>((float)sliderThumb.getWidth(),
                                           (float)sliderThumb.getHeight())
                        .withCentre(centre);
  g.setImageResamplingQuality(juce::Graphics::mediumResamplingQuality);
  g.drawImage(sliderThumb, destBounds);
  return true;
}
