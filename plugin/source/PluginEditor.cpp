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

  volumeSlider.setRange(0.0, 1.0, 0.01);
  volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  volumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::red);
  volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
  volumeSlider.addListener(this);

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
  cutoffSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  cutoffSlider.setRange(20.0, 20000.0, 1.0);
  cutoffSlider.setSkewFactorFromMidPoint(1000.0);  // התנהגות לוג-אקולית

  cutoffSlider.addListener(this);
  addAndMakeVisible(cutoffSlider);

  // cutoffLabel.setText("Filter", juce::dontSendNotification);
  // cutoffLabel.attachToComponent(&cutoffSlider, false);
  // cutoffLabel.setJustificationType(juce::Justification::centredTop);
  // myToggleBtn.setButtonText("on/off");
  myToggleBtn.addListener(this);
  addAndMakeVisible(&myToggleBtn);
  myToggleBtn.setLookAndFeel(&customLook);
  // myToggleBtn.setColour(juce::TextButton::buttonColourId,
  //                       juce::Colours::lightblue);

  resonanceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  resonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  resonanceSlider.setRange(0.1, 10.0, 0.01);
  resonanceSlider.setSkewFactorFromMidPoint(1.0);  // התנהגות לוג-אקולית
  resonanceSlider.addListener(this);
  addAndMakeVisible(resonanceSlider);
  resonanceLabel.setText("Q", juce::dontSendNotification);
  resonanceLabel.attachToComponent(&resonanceSlider, false);
  resonanceLabel.setJustificationType(juce::Justification::centredTop);
  resonanceLabel.setFont(customFont);
  resonanceLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  resonanceLabel.attachToComponent(&resonanceSlider, false);
  auto setupADSRSlider = [](juce::Slider& slider, float min, float max) {
    slider.setRange(min, max);
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 15);
  };

  // AMP ADSR
  setupADSRSlider(attackSliderAmp, 0.001f, 5.0f);
  setupADSRSlider(decaySliderAmp, 0.001f, 5.0f);
  setupADSRSlider(sustainSliderAmp, 0.0f, 1.0f);
  setupADSRSlider(releaseSliderAmp, 0.001f, 5.0f);

  // FILTER ADSR
  // setupADSRSlider(attackSliderFilter, 0.001f, 5.0f);
  // setupADSRSlider(decaySliderFilter, 0.001f, 5.0f);
  // setupADSRSlider(sustainSliderFilter, 0.0f, 1.0f);
  // setupADSRSlider(releaseSliderFilter, 0.001f, 5.0f);
  addAndMakeVisible(attackSliderAmp);
  addAndMakeVisible(decaySliderAmp);
  addAndMakeVisible(sustainSliderAmp);
  addAndMakeVisible(releaseSliderAmp);

  // addAndMakeVisible(attackSliderFilter);
  // addAndMakeVisible(decaySliderFilter);
  // addAndMakeVisible(sustainSliderFilter);
  // addAndMakeVisible(releaseSliderFilter);
  attackSliderAmp.addListener(this);
  decaySliderAmp.addListener(this);
  sustainSliderAmp.addListener(this);
  releaseSliderAmp.addListener(this);

  ninjaAnim->setTotalFrames(3);  // תעדכן לפי כמה frames יש לך
  addAndMakeVisible(ninjaAnim.get());
  ninjaAnim->setBounds(200, 370, 48, 64);
  volumeSlider.setValue(proc.getVolume(), juce::dontSendNotification);
  cutoffSlider.setValue(proc.getFilterCutOff(), juce::dontSendNotification);
  attackSliderAmp.setValue(proc.getAmpAttack(), juce::dontSendNotification);
  decaySliderAmp.setValue(proc.getAmpDecay(), juce::dontSendNotification);
  sustainSliderAmp.setValue(proc.getAmpSustain(), juce::dontSendNotification);
  releaseSliderAmp.setValue(proc.getAmpRelease(), juce::dontSendNotification);
}

NoiseGAudioProcessorEditor::~NoiseGAudioProcessorEditor() {
  waveformSelector.setLookAndFeel(nullptr);
}

void NoiseGAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colour::fromRGB(102, 178, 255));
  // if (myImage.isValid()) {
  //   g.drawImage(myImage, 100, 370, 200, 200, 0, 0, myImage.getWidth(),
  //               myImage.getHeight());
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
  myToggleBtn.setBounds(308, 115, 60, 40);
}

void NoiseGAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
  if (slider == &volumeSlider) {
    float volumeValue = static_cast<float>(volumeSlider.getValue());
    dynamic_cast<NoiseGAudioProcessor&>(processorRef).setVolume(volumeValue);
    DBG("Volume: " << volumeValue);
  }
  if (slider == &cutoffSlider) {
    auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
    proc.synth.setCutoff(cutoffSlider.getValue());
  }
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
}
// namespace audio_plugin
}  // namespace audio_plugin