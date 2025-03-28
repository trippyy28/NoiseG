#include "NoiseG/PluginProcessor.h"
#include "NoiseG/NinjaAnimator.h"
#include "BinaryData.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_graphics/juce_graphics.h>
namespace audio_plugin {

class NoiseGAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Slider::Listener,
                                   public juce::ComboBox::Listener {
public:
  NoiseGAudioProcessorEditor(juce::AudioProcessor& p);
  ~NoiseGAudioProcessorEditor() override;

  void paint(juce::Graphics& g) override;
  void resized() override;

  // Implement the sliderValueChanged method
  void sliderValueChanged(juce::Slider* slider) override;
  void comboBoxChanged(juce::ComboBox* comboBox) override;

private:
  juce::ComboBox waveformSelector;
  juce::Slider volumeSlider;
  juce::Label volumeLabel;
  juce::AudioProcessor& processorRef;
  juce::Image myImage;
  juce::Image myImage2;
  std::unique_ptr<NinjaAnimator> ninjaAnim;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseGAudioProcessorEditor)
};

}  // namespace audio_plugin