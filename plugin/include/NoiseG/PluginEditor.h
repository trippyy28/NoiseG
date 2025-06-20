#include "NoiseG/PluginProcessor.h"
#include "NoiseG/NinjaAnimator.h"
#include "BinaryData.h"
#include "NoiseG/CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_graphics/juce_graphics.h>
namespace audio_plugin {

class NoiseGAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Slider::Listener,
                                   public juce::ComboBox::Listener,
                                   public juce::Button::Listener {
public:
  NoiseGAudioProcessorEditor(juce::AudioProcessor& p);
  ~NoiseGAudioProcessorEditor() override;

  void paint(juce::Graphics& g) override;
  void resized() override;

  // Implement the sliderValueChanged method
  void sliderValueChanged(juce::Slider* slider) override;
  void comboBoxChanged(juce::ComboBox* comboBox) override;
  void buttonClicked(juce::Button* button) override;
  void setCutoff(float freq);

private:
  juce::ComboBox waveformSelector;
  juce::Slider volumeSlider;
  juce::Slider cutoffSlider;
  juce::Slider resonanceSlider;
  juce::Label cutoffLabel;
  juce::Label volumeLabel;
  juce::Label resonanceLabel;
  juce::Label attackLabel;
  CustomLookAndFeel customLook;
  juce::AudioProcessor& processorRef;
  juce::Image myImage;
  juce::Image myImage2;
  juce::ImageButton myBtn;
  juce::Image myBtnImage;
  juce::ToggleButton myToggleBtn;
  // ADSR for amp
  juce::Slider attackSliderAmp, decaySliderAmp, sustainSliderAmp,
      releaseSliderAmp;
  // ADSR for filter
  juce::Slider attackSliderFilter, decaySliderFilter, sustainSliderFilter,
      releaseSliderFilter;
  juce ::Slider modulateFilterSlider;
  std::unique_ptr<NinjaAnimator> ninjaAnim;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseGAudioProcessorEditor)
};

}  // namespace audio_plugin