#include "NoiseG/PluginEditor.h"
#include "NoiseG/PluginProcessor.h"
#include "BinaryData.h"

namespace audio_plugin {

NoiseGAudioProcessorEditor::NoiseGAudioProcessorEditor(juce::AudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p) {
  // אתחול הסליידר עם הערך הנוכחי מה־Processor
  juce::Typeface::Ptr customTypeface = juce::Typeface::createSystemTypefaceFor(
      BinaryData::ffont_ttf, BinaryData::ffont_ttfSize);

  juce::Font customFont(customTypeface);
  customFont.setHeight(16.0f);
  auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
  int currentWaveform = static_cast<int>(proc.synth.getWaveform());
  chordSlotColour = juce::Colour::fromRGB(102, 255, 255);
  chordSlotEmptyColour = chordSlotColour.darker(0.2f);
  chordSlotActiveColour = juce::Colour::fromRGB(255, 220, 130);

  myBtn.setSize(50, 50);
  myBtn.setBounds(100, 370, 100, 40);
  // myBtn.setButtonText("Stop/Play");
  myBtn.addListener(this);
  setSize(800, 450);
  addAndMakeVisible(&volumeSlider);
  addAndMakeVisible(&volumeLabel);
  addAndMakeVisible(&myBtn);

  waveformSelector.addItem("Sine", 1);
  waveformSelector.addItem("Square", 2);
  waveformSelector.addItem("Saw", 3);
  waveformSelector.addItem("Noise", 4);
  waveformSelector.setSelectedId(currentWaveform + 1,
                                 juce::dontSendNotification);
  waveformSelector.addListener(this);
  // waveformSelector.setColour(juce::ComboBox::backgroundColourId,
  //                            juce::Colours::blueviolet);
  waveformSelector.setBounds(500, 30, 100, 50);
  waveformSelector.setTextWhenNothingSelected("Select Waveform");
  waveformSelector.setLookAndFeel(&customLook);
  addAndMakeVisible(&waveformSelector);

  // volumeSlider.setRange(0.0, 1.0, 0.01);
  volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  volumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::red);
  volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
  volumeSlider.setTextValueSuffix("dB");
  // volumeSlider.addListener(this);

  volumeLabel.setText("Volume", juce::dontSendNotification);
  volumeLabel.attachToComponent(&volumeSlider, true);
  volumeLabel.setFont(juce::Font(15.0f));
  volumeLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  volumeLabel.setFont(customFont);

  myImage = juce::ImageCache::getFromMemory(BinaryData::Soldier_gif,
                                            BinaryData::Soldier_gifSize);
  ninjaAnim = std::make_unique<NinjaAnimator>(juce::ImageCache::getFromMemory(
      BinaryData::Healer_png, BinaryData::Healer_pngSize));
  juce::Image myBtnImage = juce::ImageCache::getFromMemory(
      BinaryData::play_png, BinaryData::play_pngSize);
  myBtn.setImages(true, true, true, myBtnImage, 1.0f, {}, myBtnImage, 1.0f, {},
                  myBtnImage, 1.0f, {});
  cMajor = juce::ImageCache::getFromMemory(BinaryData::CMAJOR_png,
                                           BinaryData::CMAJOR_pngSize);
  cutoffSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  cutoffSlider.setTextValueSuffix(" Hz");
  cutoffSlider.setRange(20.0, 20000.0, 1.0);
  cutoffSlider.setSkewFactorFromMidPoint(1000.0);  // התנהגות לוג-אקולית

  addAndMakeVisible(cutoffSlider);
  auto& apvts = proc.apvts;
  volumeAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          apvts, ParameterID::outputLevel.getParamID(), volumeSlider);

  cutoffAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          apvts, ParameterID::filterFreq.getParamID(), cutoffSlider);

  // cutoffLabel.setText("Filter", juce::dontSendNotification);
  // cutoffLabel.attachToComponent(&cutoffSlider, false);
  // cutoffLabel.setJustificationType(juce::Justification::centredTop);
  // myToggleBtn.setButtonText("on/off");
  myToggleBtn.addListener(this);
  addAndMakeVisible(&myToggleBtn);
  myToggleBtn.setLookAndFeel(&customLook);
  myToggleBtn.setToggleState(
      proc.synth.getFilterEnbaled(),
      juce::
          dontSendNotification);  // myToggleBtn.setColour(juce::TextButton::buttonColourId,
  //                       juce::Colours::lightblue);

  resonanceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  resonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  resonanceSlider.setRange(0.1, 4.0, 0.01);
  resonanceSlider.setSkewFactorFromMidPoint(1.0);  // התנהגות לוג-אקולית
  resonanceSlider.addListener(this);
  resonanceSlider.setValue(proc.getFilterRes(), juce::dontSendNotification);
  addAndMakeVisible(resonanceSlider);
  resonanceLabel.setText("Q", juce::dontSendNotification);
  resonanceLabel.attachToComponent(&resonanceSlider, false);
  resonanceLabel.setJustificationType(juce::Justification::centredTop);
  resonanceLabel.setFont(customFont);
  resonanceLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  resonanceLabel.attachToComponent(&resonanceSlider, false);
  chordModeToggle.setButtonText("Chord Mode");
  chordModeToggle.addListener(this);
  chordModeToggle.setLookAndFeel(&customLook);
  chordModeToggle.setToggleState(proc.isChordModeEnabled(),
                                 juce::dontSendNotification);
  addAndMakeVisible(&chordModeToggle);
  chordModeLabel.setText("Chords", juce::dontSendNotification);
  chordModeLabel.setFont(customFont);
  chordModeLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  chordModeLabel.attachToComponent(&chordModeToggle, true);
  addAndMakeVisible(chordModeLabel);
  chordGroup.setText("Chord Bank");
  chordGroup.setColour(juce::GroupComponent::outlineColourId,
                       juce::Colours::black.withAlpha(0.4f));
  chordGroup.setColour(juce::GroupComponent::textColourId,
                       juce::Colours::black);
  addAndMakeVisible(chordGroup);
  chordPrevButton.addListener(this);
  chordNextButton.addListener(this);
  addAndMakeVisible(chordPrevButton);
  addAndMakeVisible(chordNextButton);
  chordBankLabel.setJustificationType(juce::Justification::centred);
  chordBankLabel.setFont(customFont);
  chordBankLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(chordBankLabel);
  for (size_t i = 0; i < chordLabels.size(); ++i) {
    auto& label = chordLabels[i];
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::backgroundColourId, chordSlotColour);
    label.setColour(juce::Label::outlineColourId,
                    juce::Colours::black.withAlpha(0.2f));
    label.setInterceptsMouseClicks(true, false);
    label.addMouseListener(this, false);
    label.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible(label);
  }
  auto setupADSRSlider = [](juce::Slider& slider, float min, float max) {
    slider.setRange(min, max);
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 30);
  };

  // AMP ADSR
  setupADSRSlider(attackSliderAmp, 0.001f, 5.0f);
  setupADSRSlider(decaySliderAmp, 0.001f, 5.0f);
  setupADSRSlider(sustainSliderAmp, 0.0f, 1.0f);
  setupADSRSlider(releaseSliderAmp, 0.001f, 5.0f);

  // FILTER ADSR
  setupADSRSlider(attackSliderFilter, 0.001f, 5.0f);
  setupADSRSlider(decaySliderFilter, 0.001f, 5.0f);
  setupADSRSlider(sustainSliderFilter, 0.0f, 1.0f);
  setupADSRSlider(releaseSliderFilter, 0.001f, 5.0f);
  addAndMakeVisible(attackSliderAmp);
  addAndMakeVisible(decaySliderAmp);
  addAndMakeVisible(sustainSliderAmp);
  addAndMakeVisible(releaseSliderAmp);

  addAndMakeVisible(attackSliderFilter);
  addAndMakeVisible(decaySliderFilter);
  addAndMakeVisible(sustainSliderFilter);
  addAndMakeVisible(releaseSliderFilter);
  attackSliderAmp.addListener(this);
  decaySliderAmp.addListener(this);
  sustainSliderAmp.addListener(this);
  releaseSliderAmp.addListener(this);

  ninjaAnim->setTotalFrames(3);  // תעדכן לפי כמה frames יש לך
  addAndMakeVisible(ninjaAnim.get());
  ninjaAnim->setBounds(200, 370, 48, 64);
  // volumeSlider.setValue(proc.getVolume(), juce::dontSendNotification);
  // Let the APVTS attachment drive the initial cutoff value
  attackSliderAmp.setValue(proc.getAmpAttack(), juce::dontSendNotification);
  decaySliderAmp.setValue(proc.getAmpDecay(), juce::dontSendNotification);
  sustainSliderAmp.setValue(proc.getAmpSustain(), juce::dontSendNotification);
  releaseSliderAmp.setValue(proc.getAmpRelease(), juce::dontSendNotification);
  attackSliderFilter.setValue(proc.getFilterAttack(),
                              juce::dontSendNotification);
  decaySliderFilter.setValue(proc.getFilterDecay(), juce::dontSendNotification);
  sustainSliderFilter.setValue(proc.getFilterSustain(),
                               juce::dontSendNotification);
  releaseSliderFilter.setValue(proc.getFilterRelease(),
                               juce::dontSendNotification);
  attackSliderFilter.addListener(this);
  decaySliderFilter.addListener(this);
  sustainSliderFilter.addListener(this);
  releaseSliderFilter.addListener(this);
  addAndMakeVisible(modulateFilterSlider);
  addAndMakeVisible(attackLabel);
  modulateFilterSlider.setRange(0.0, 1.0, 0.01);
  modulateFilterSlider.addListener(this);
  modulateFilterSlider.setValue(proc.getModFilterAmount(),
                                juce ::dontSendNotification);

  polyphonySlider.setRange(1, Synth::MAX_VOICES, 1);
  polyphonySlider.setSliderStyle(juce::Slider::LinearHorizontal);
  polyphonySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
  polyphonySlider.addListener(this);
  polyphonySlider.setValue(proc.getPolyphony(), juce::dontSendNotification);
  addAndMakeVisible(polyphonySlider);

  polyphonyLabel.setText("Voices", juce::dontSendNotification);
  polyphonyLabel.attachToComponent(&polyphonySlider, true);
  polyphonyLabel.setFont(customFont);
  polyphonyLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(polyphonyLabel);
  refreshChordDisplay();
}

NoiseGAudioProcessorEditor::~NoiseGAudioProcessorEditor() {
  stopChordPreview();
  // Ensure attachments are destroyed before sliders/components during teardown
  volumeAttach.reset();
  cutoffAttach.reset();

  // Clear custom LAFs in case of destruction ordering changes
  myToggleBtn.setLookAndFeel(nullptr);
  chordModeToggle.setLookAndFeel(nullptr);
  waveformSelector.setLookAndFeel(nullptr);
}

void NoiseGAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colour::fromRGB(102, 178, 255));
  // if (cMajor.isValid()) {
  //   g.drawImage(cMajor, 400, 100, 50, 50, 0, 0, cMajor.getWidth(),
  //               cMajor.getHeight());
  // }
}

void NoiseGAudioProcessorEditor::resized() {
  int startX = 10;
  int y = 100;
  int width = 40;
  int height = 100;
  int gap = 10;

  attackSliderAmp.setBounds(startX, y, width, height);
  decaySliderAmp.setBounds(startX + (width + gap), y, width, height);
  sustainSliderAmp.setBounds(startX + 2 * (width + gap), y, width, height);
  releaseSliderAmp.setBounds(startX + 3 * (width + gap), y, width, height);

  attackSliderFilter.setBounds(startX, y + 120, width, height);
  decaySliderFilter.setBounds(startX + (width + gap), y + 120, width, height);
  sustainSliderFilter.setBounds(startX + 2 * (width + gap), y + 120, width,
                                height);
  releaseSliderFilter.setBounds(startX + 3 * (width + gap), y + 120, width,
                                height);
  volumeSlider.setBounds(60, 30, 100, 100);
  volumeLabel.setBounds(10, 20, 100, 40);
  cutoffSlider.setBounds(300, 150, 100, 100);
  resonanceSlider.setBounds(250, 150, 60, 60);
  modulateFilterSlider.setBounds(400, 200, 140, 140);
  myToggleBtn.setBounds(308, 115, 60, 40);
  chordModeToggle.setBounds(600, 140, 100, 30);
  polyphonySlider.setBounds(500, 90, 160, 40);
  chordGroup.setBounds(500, 190, 240, 120);
  auto chordGroupBounds = chordGroup.getBounds();
  int buttonWidth = 32;
  int buttonHeight = 24;
  int controlY = chordGroupBounds.getY() - buttonHeight - 4;
  chordPrevButton.setBounds(chordGroupBounds.getX(),
                            controlY,
                            buttonWidth,
                            buttonHeight);
  chordNextButton.setBounds(chordGroupBounds.getRight() - buttonWidth,
                            controlY,
                            buttonWidth,
                            buttonHeight);
  chordBankLabel.setBounds(chordPrevButton.getRight(),
                           controlY,
                           chordGroupBounds.getWidth() - 2 * buttonWidth,
                           buttonHeight);
  auto groupInner = chordGroup.getBounds().reduced(12);
  int cellWidth = groupInner.getWidth() / 4;
  int cellHeight = groupInner.getHeight() / 2;
  for (int i = 0; i < Synth::chordsPerBank; ++i) {
    int row = i / 4;
    int col = i % 4;
    auto cell = juce::Rectangle<int>(groupInner.getX() + col * cellWidth,
                                     groupInner.getY() + row * cellHeight,
                                     cellWidth, cellHeight)
                    .reduced(4);
    chordLabels[(size_t)i].setBounds(cell);
  }
}

void NoiseGAudioProcessorEditor::refreshChordDisplay() {
  auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
  bool chordModeActive = proc.isChordModeEnabled();
  chordGroup.setVisible(chordModeActive);
  for (auto& label : chordLabels)
    label.setVisible(chordModeActive);
  chordPrevButton.setVisible(chordModeActive);
  chordNextButton.setVisible(chordModeActive);
  chordBankLabel.setVisible(chordModeActive);
  if (!chordModeActive) {
    stopChordPreview();
    return;
  }

  int bankCount =
      juce::jmax(1, static_cast<int>(proc.getChordBanks().size()));
  int activeBank =
      juce::jlimit(0, bankCount - 1, proc.getActiveChordBank());
  chordPrevButton.setEnabled(bankCount > 1);
  chordNextButton.setEnabled(bankCount > 1);
  chordBankLabel.setText("Bank " + juce::String(activeBank + 1) + " / " +
                             juce::String(bankCount),
                         juce::dontSendNotification);
  for (int i = 0; i < Synth::chordsPerBank; ++i) {
    const auto* chord = proc.synth.getChord(activeBank, i);
    if (chord != nullptr)
      chordLabels[(size_t)i].setText(formatChordLabel(*chord, i),
                                     juce::dontSendNotification);
    else
      chordLabels[(size_t)i].setText(formatChordLabel({}, i),
                                     juce::dontSendNotification);
    auto background =
        chord ? chordSlotColour : chordSlotEmptyColour;
    if (pressedChordIndex == i)
      background = chordSlotActiveColour;
    chordLabels[(size_t)i].setColour(juce::Label::backgroundColourId,
                                     background);
  }
}

juce::String NoiseGAudioProcessorEditor::formatChordLabel(
    const Synth::Chord& chord,
    int slotIndex) const {
  static const char* noteNames[12] = {"C",  "C#", "D",  "D#", "E",  "F",
                                      "F#", "G",  "G#", "A",  "A#", "B"};
  juce::String labelText = juce::String(slotIndex + 1);
  if (chord.empty())
    return labelText + "--";

  juce::StringArray noteStrings;
  for (int note : chord) {
    int clamped = juce::jlimit(0, 127, note);
    int octave = (clamped / 12) - 1;
    noteStrings.add(juce::String(noteNames[clamped % 12]) +
                    juce::String(octave));
  }
  return labelText + noteStrings.joinIntoString(" ");
}

bool NoiseGAudioProcessorEditor::isChordSlotComponent(
    const juce::Component* component,
    int& slotIndex) const {
  if (component == nullptr)
    return false;
  for (size_t i = 0; i < chordLabels.size(); ++i) {
    if (component == &chordLabels[i]) {
      slotIndex = static_cast<int>(i);
      return true;
    }
  }
  return false;
}

void NoiseGAudioProcessorEditor::startChordPreview(int slotIndex) {
  auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
  stopChordPreview();
  if (!proc.isChordModeEnabled()) {
    refreshChordDisplay();
    return;
  }
  if (slotIndex < 0 || slotIndex >= Synth::chordsPerBank) {
    refreshChordDisplay();
    return;
  }

  int bankCount =
      juce::jmax(1, static_cast<int>(proc.getChordBanks().size()));
  int activeBank =
      juce::jlimit(0, bankCount - 1, proc.getActiveChordBank());
  const auto* chord = proc.synth.getChord(activeBank, slotIndex);
  if (chord != nullptr) {
    pressedChordIndex = slotIndex;
    proc.previewChord(activeBank, slotIndex);
  }
  refreshChordDisplay();
}

void NoiseGAudioProcessorEditor::stopChordPreview() {
  auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
  proc.stopPreviewChord();
  pressedChordIndex = -1;
}

void NoiseGAudioProcessorEditor::mouseDown(const juce::MouseEvent& event) {
  int slotIndex = -1;
  if (isChordSlotComponent(event.eventComponent, slotIndex) ||
      isChordSlotComponent(event.originalComponent, slotIndex))
    startChordPreview(slotIndex);
  juce::AudioProcessorEditor::mouseDown(event);
}

void NoiseGAudioProcessorEditor::mouseUp(const juce::MouseEvent& event) {
  bool shouldStop = pressedChordIndex >= 0;
  int slotIndex = -1;
  if (isChordSlotComponent(event.eventComponent, slotIndex) ||
      isChordSlotComponent(event.originalComponent, slotIndex))
    shouldStop = true;
  if (shouldStop) {
    stopChordPreview();
    refreshChordDisplay();
  }
  juce::AudioProcessorEditor::mouseUp(event);
}

void NoiseGAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
  // if (slider == &volumeSlider) {
  //   float volumeValue = static_cast<float>(volumeSlider.getValue());
  //   dynamic_cast<NoiseGAudioProcessor&>(processorRef).setVolume(volumeValue);
  //   DBG("Volume: " << volumeValue);
  // }
  // cutoff is driven by APVTS attachment; no manual handling here
  if (slider == &resonanceSlider) {
    auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
    proc.synth.setFilterResonance(resonanceSlider.getValue());
  }
  if (slider == &attackSliderAmp || slider == &decaySliderAmp ||
      slider == &sustainSliderAmp || slider == &releaseSliderAmp) {
    auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
    proc.setAmpADSR(attackSliderAmp.getValue(), decaySliderAmp.getValue(),
                    sustainSliderAmp.getValue(), releaseSliderAmp.getValue());
  }

  if (slider == &attackSliderFilter || slider == &decaySliderFilter ||
      slider == &sustainSliderFilter || slider == &releaseSliderFilter) {
    auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
    proc.setFilterADSR(
        attackSliderFilter.getValue(), decaySliderFilter.getValue(),
        sustainSliderFilter.getValue(), releaseSliderFilter.getValue());
  }
  if (slider == &modulateFilterSlider) {
    auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
    proc.setModulationFilter(modulateFilterSlider.getValue());
  }
  if (slider == &polyphonySlider) {
    auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
    proc.setPolyphony(static_cast<int>(polyphonySlider.getValue()));
  }
}

void NoiseGAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox) {
  if (comboBox == &waveformSelector) {
    int selectedWaveform = comboBox->getSelectedId();
    DBG("Selected waveform: " << selectedWaveform);
    dynamic_cast<NoiseGAudioProcessor&>(processorRef)
        .setWaveform(selectedWaveform);
  }
}

void NoiseGAudioProcessorEditor::buttonClicked(juce::Button* button) {
  if (button == &myBtn) {
    static bool isPlaying = true;
    isPlaying = !isPlaying;
    ninjaAnim->setAnimationPlaying(isPlaying);
  }

  if (button == &myToggleBtn) {
    bool isToggled = myToggleBtn.getToggleState();
    dynamic_cast<NoiseGAudioProcessor&>(processorRef)
        .synth.setFilterEnabled(isToggled);
  }
  if (button == &chordModeToggle) {
    auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
    proc.enableChordMode(chordModeToggle.getToggleState());
    if (!chordModeToggle.getToggleState())
      stopChordPreview();
    refreshChordDisplay();
  }
  if (button == &chordPrevButton || button == &chordNextButton) {
    auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
    int bankCount =
        juce::jmax(1, static_cast<int>(proc.getChordBanks().size()));
    if (bankCount <= 0)
      return;
    int currentBank = proc.getActiveChordBank();
    int delta = button == &chordPrevButton ? -1 : 1;
    int nextBank = (currentBank + delta) % bankCount;
    if (nextBank < 0)
      nextBank += bankCount;
    proc.setActiveChordBank(nextBank);
    stopChordPreview();
    refreshChordDisplay();
  }
}
// namespace audio_plugin
}  // namespace audio_plugin
