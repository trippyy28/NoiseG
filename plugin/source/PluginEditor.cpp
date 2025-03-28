#include "NoiseG/PluginEditor.h"
#include "NoiseG/PluginProcessor.h"
#include "BinaryData.h"

namespace audio_plugin {

NoiseGAudioProcessorEditor::NoiseGAudioProcessorEditor(juce::AudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p) {
  // אתחול הסליידר עם הערך הנוכחי מה־Processor
  auto& proc = dynamic_cast<NoiseGAudioProcessor&>(processorRef);
  int currentWaveform = static_cast<int>(proc.synth.getWaveform());
  volumeSlider.setValue(proc.getVolume(), juce::dontSendNotification);

  setSize(400, 300);
  addAndMakeVisible(&volumeSlider);
  addAndMakeVisible(&volumeLabel);

  waveformSelector.addItem("Sineesss", 1);
  waveformSelector.addItem("Square", 2);
  waveformSelector.addItem("Saw", 3);
  waveformSelector.addItem("Noiseee", 4);
  waveformSelector.setSelectedId(currentWaveform + 1,
                                 juce::dontSendNotification);
  waveformSelector.addListener(this);
  waveformSelector.setColour(juce::ComboBox::backgroundColourId,
                             juce::Colours::white);
  waveformSelector.setBounds(200, 30, 100, 100);
  addAndMakeVisible(&waveformSelector);

  volumeSlider.setRange(0.0, 1.0, 0.01);
  volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  volumeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::red);
  volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
  volumeSlider.addListener(this);

  volumeLabel.setText("Volume222", juce::dontSendNotification);
  volumeLabel.attachToComponent(&volumeSlider, true);
  volumeLabel.setFont(juce::Font(15.0f));
  volumeLabel.setColour(juce::Label::textColourId, juce::Colours::black);

  myImage = juce::ImageCache::getFromMemory(BinaryData::Untitled_png,
                                            BinaryData::Untitled_pngSize);
  ninjaAnim = std::make_unique<NinjaAnimator>(juce::ImageCache::getFromMemory(
      BinaryData::Ninja_png, BinaryData::Ninja_pngSize));

  ninjaAnim->setTotalFrames(4);  // תעדכן לפי כמה frames יש לך
  addAndMakeVisible(ninjaAnim.get());
  ninjaAnim->setBounds(200, 200, 48, 64);
}

NoiseGAudioProcessorEditor::~NoiseGAudioProcessorEditor() {}

void NoiseGAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colours::white);
  if (myImage.isValid()) {
    g.drawImage(myImage, 10, 10, 50, 50, 0, 0, myImage.getWidth(),
                myImage.getHeight());
  }
  // if (myImage2.isValid()) {
  //   g.drawImage(myImage2, 10, 10, 50, 50, 0, 0, myImage2.getWidth(),
  //               myImage2.getHeight());
  // }
}

void NoiseGAudioProcessorEditor::resized() {
  volumeSlider.setBounds(60, 30, 100, 100);
  volumeLabel.setBounds(10, 20, 100, 40);
}

void NoiseGAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
  if (slider == &volumeSlider) {
    float volumeValue = static_cast<float>(volumeSlider.getValue());
    dynamic_cast<NoiseGAudioProcessor&>(processorRef).setVolume(volumeValue);
    // DBG("Volume: " << volumeValue);
  }
}

void NoiseGAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox) {
  if (comboBox == &waveformSelector) {
    int selectedWaveform = comboBox->getSelectedId();
    // DBG("Selected waveform: " << selectedWaveform);
    dynamic_cast<NoiseGAudioProcessor&>(processorRef)
        .setWaveform(selectedWaveform);
  }
}

}  // namespace audio_plugin