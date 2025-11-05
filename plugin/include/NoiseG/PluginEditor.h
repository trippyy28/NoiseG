#include "NoiseG/PluginProcessor.h"
#include "NoiseG/NinjaAnimator.h"
#include "BinaryData.h"
#include "NoiseG/CustomLookAndFeel.h"
#include <array>
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
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      volumeAttach;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      cutoffAttach;
  void paint(juce::Graphics& g) override;
  void resized() override;

  // Implement the sliderValueChanged method
  void sliderValueChanged(juce::Slider* slider) override;
  void comboBoxChanged(juce::ComboBox* comboBox) override;
  void buttonClicked(juce::Button* button) override;
  void setCutoff(float freq);
  void refreshChordDisplay();
  juce::String formatChordLabel(const Synth::Chord& chord, int slotIndex) const;

private:
  void mouseDown(const juce::MouseEvent& event) override;
  void mouseUp(const juce::MouseEvent& event) override;
  void startChordPreview(int slotIndex);
  void stopChordPreview();
  bool isChordSlotComponent(const juce::Component* component,
                            int& slotIndex) const;
  juce::ComboBox waveformSelector;
  juce::Slider volumeSlider;
  juce::Slider cutoffSlider;
  juce::Slider resonanceSlider;
  juce::Label cutoffLabel;
  juce::Label volumeLabel;
  juce::Label resonanceLabel;
  juce::Label attackLabel;
  juce::Slider polyphonySlider;
  juce::Label polyphonyLabel;
  CustomLookAndFeel customLook;
  juce::AudioProcessor& processorRef;
  juce::Image myImage;
  juce::Image myImage2;
  juce::ImageButton myBtn;
  juce::Image myBtnImage;
  juce::Image cMajor;
  juce::ToggleButton myToggleBtn;
  juce::ToggleButton chordModeToggle;
  juce::Label chordModeLabel;
  juce::GroupComponent chordGroup;
  juce::TextButton chordPrevButton{"<"};
  juce::TextButton chordNextButton{">"};
  juce::Label chordBankLabel;
  std::array<juce::Label, Synth::chordsPerBank> chordLabels;
  juce::Colour chordSlotColour;
  juce::Colour chordSlotEmptyColour;
  juce::Colour chordSlotActiveColour;
  int pressedChordIndex = -1;
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
