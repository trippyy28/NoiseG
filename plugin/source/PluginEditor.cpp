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
  volumeSlider.setValue(proc.getVolume(), juce::dontSendNotification);
  myBtn.setSize(50, 50);
  myBtn.setBounds(100, 30, 100, 40);
  myBtn.setButtonText("Stop/Play");
  myBtn.addListener(this);
  setSize(400, 300);
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
  waveformSelector.setBounds(200, 30, 100, 100);
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
      BinaryData::Ninja_png, BinaryData::Ninja_pngSize));
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

  cutoffLabel.setText("Cutoff", juce::dontSendNotification);
  cutoffLabel.attachToComponent(&cutoffSlider, false);
  cutoffLabel.setJustificationType(juce::Justification::centredTop);

  ninjaAnim->setTotalFrames(3);  // תעדכן לפי כמה frames יש לך
  addAndMakeVisible(ninjaAnim.get());
  ninjaAnim->setBounds(200, 200, 48, 64);
}

NoiseGAudioProcessorEditor::~NoiseGAudioProcessorEditor() {
  waveformSelector.setLookAndFeel(nullptr);
}

void NoiseGAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colours::blue);
  if (myImage.isValid()) {
    g.drawImage(myImage, 10, 135, 200, 200, 0, 0, myImage.getWidth(),
                myImage.getHeight());
  }
}

void NoiseGAudioProcessorEditor::resized() {
  volumeSlider.setBounds(60, 30, 100, 100);
  volumeLabel.setBounds(10, 20, 100, 40);
  cutoffSlider.setBounds(60, 150, 100, 100);
}

void NoiseGAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
  if (slider == &volumeSlider) {
    float volumeValue = static_cast<float>(volumeSlider.getValue());
    dynamic_cast<NoiseGAudioProcessor&>(processorRef).setVolume(volumeValue);
    // DBG("Volume: " << volumeValue);
  }
  if (slider == &cutoffSlider) {
    auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
    proc.synth.setCutoff(cutoffSlider.getValue());
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
    dynamic_cast<NoiseGAudioProcessor&>(processorRef)
        .synth.setFilterEnabled(isPlaying);
  }
}
}  // namespace audio_plugin