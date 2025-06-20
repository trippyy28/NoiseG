#include "NoiseG/PluginProcessor.h"
#include "NoiseG/PluginEditor.h"

namespace audio_plugin {

NoiseGAudioProcessor::NoiseGAudioProcessor()
    : volume(0.5f),  // אתחול ברירת מחדל ל־volume
      AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ) {
}

NoiseGAudioProcessor::~NoiseGAudioProcessor() {}

const juce::String NoiseGAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool NoiseGAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool NoiseGAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool NoiseGAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double NoiseGAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int NoiseGAudioProcessor::getNumPrograms() {
  return 1;
}

int NoiseGAudioProcessor::getCurrentProgram() {
  return 0;
}

void NoiseGAudioProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String NoiseGAudioProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void NoiseGAudioProcessor::changeProgramName(int index,
                                             const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

void NoiseGAudioProcessor::prepareToPlay(double sampleRate,
                                         int samplesPerBlock) {
  synth.allocateResources(sampleRate, samplesPerBlock);
  reset();
}

void NoiseGAudioProcessor::releaseResources() {
  synth.deallocateResources();
}

bool NoiseGAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

void NoiseGAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                        juce::MidiBuffer& midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  splitBufferByEvents(buffer, midiMessages);
}

void NoiseGAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer,
                                               juce::MidiBuffer& midiMessages) {
  int bufferOffset = 0;
  for (const auto metadata : midiMessages) {
    int samplesThisSegment = metadata.samplePosition - bufferOffset;
    if (samplesThisSegment > 0) {
      render(buffer, samplesThisSegment, bufferOffset);
      bufferOffset += samplesThisSegment;
    }
    if (metadata.numBytes <= 3) {
      uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
      uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
      handleMIDI(metadata.data[0], data1, data2);
    }
  }
  int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
  if (samplesLastSegment > 0)
    render(buffer, samplesLastSegment, bufferOffset);
  midiMessages.clear();
}

void NoiseGAudioProcessor::handleMIDI(uint8_t data0,
                                      uint8_t data1,
                                      uint8_t data2) {
  synth.midiMessage(data0, data1, data2);
}

void NoiseGAudioProcessor::render(juce::AudioBuffer<float>& buffer,
                                  int sampleCount,
                                  int bufferOffset) {
  float* outputBuffers[2] = {nullptr, nullptr};
  outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
  if (getTotalNumOutputChannels() > 1)
    outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
  synth.render(outputBuffers, sampleCount);
}

void NoiseGAudioProcessor::setVolume(float vol) {
  volume = vol;          // עדכון מקור האמת ב־Processor
  synth.setVolume(vol);  // העברת הערך ל־Synth
}

void NoiseGAudioProcessor::setWaveform(int waveformType) {
  auto wf = static_cast<WaveformType>(waveformType - 1);
  synth.setWaveform(wf);  // ← העברת ה־waveform לסינתיסייזר האמיתי
  DBG("Waveform set to: " << wf);
}
void NoiseGAudioProcessor::setAmpADSR(float a, float d, float s, float r) {
  DBG("setAmpADSR called - A: " << a << ", D: " << d << ", S: " << s
                                << ", R: " << r);
  synth.voice.setAmpADSR(a, d, s, r);
  synth.voice.ampEnvelope.setParameters(synth.voice.ampParams);
}
void NoiseGAudioProcessor::setFilterADSR(float a, float d, float s, float r) {
  DBG("setFilter called - A: " << a << ", D: " << d << ", S: " << s
                               << ", R: " << r);
  synth.voice.setFilterADSR(a, d, s, r);
  // synth.voice.filterParams = {a, d, s, r};
  // synth.voice.filterEnvelope.setParameters(synth.voice.filterParams);
}

void NoiseGAudioProcessor::setModulationFilter(float amount) {
  synth.setFilterModAmount(amount);
}

bool NoiseGAudioProcessor::hasEditor() const {
  return true;
}

juce::AudioProcessorEditor* NoiseGAudioProcessor::createEditor() {
  return new NoiseGAudioProcessorEditor(*this);
}

void NoiseGAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  juce::XmlElement state("MyPluginState");
  state.setAttribute("volume", volume);
  state.setAttribute("waveform", static_cast<int>(synth.getWaveform()));
  state.setAttribute("ampAttack", synth.voice.ampParams.attack);
  state.setAttribute("ampDecay", synth.voice.ampParams.decay);
  state.setAttribute("ampSustain", synth.voice.ampParams.sustain);
  state.setAttribute("ampRelease", synth.voice.ampParams.release);
  state.setAttribute("filterAttack", synth.voice.filterParams.attack);
  state.setAttribute("filterDecay", synth.voice.filterParams.decay);
  state.setAttribute("filterSustain", synth.voice.filterParams.sustain);
  state.setAttribute("filterRelease", synth.voice.filterParams.release);
  state.setAttribute("filterCutOff", getFilterCutOff());
  copyXmlToBinary(state, destData);
}

void NoiseGAudioProcessor::setStateInformation(const void* data,
                                               int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState != nullptr && xmlState->hasTagName("MyPluginState")) {
    volume = xmlState->getDoubleAttribute("volume", volume);
    setVolume(volume);
  }
  if (xmlState->hasAttribute("waveform")) {
    int waveformType = xmlState->getIntAttribute("waveform");
    setWaveform(waveformType);  // זה יפעיל את synth.setWaveform
  }
  if (xmlState->hasAttribute("filterCutOff")) {
    float filterCutOff = xmlState->getDoubleAttribute("filterCutOff", 1000.0f);
    synth.setCutoff(filterCutOff);
  }

  float ampA = xmlState->getDoubleAttribute("ampAttack", 0.01f);
  float ampD = xmlState->getDoubleAttribute("ampDecay", 0.1f);
  float ampS = xmlState->getDoubleAttribute("ampSustain", 1.0f);
  float ampR = xmlState->getDoubleAttribute("ampRelease", 0.1f);
  setAmpADSR(ampA, ampD, ampS, ampR);
  float filterA = xmlState->getDoubleAttribute("filterAttack", 0.01f);
  float filterD = xmlState->getDoubleAttribute("filterDecay", 0.01f);
  float filterS = xmlState->getDoubleAttribute("filterSustain", 1.01f);
  float filterR = xmlState->getDoubleAttribute("filterRelease", 0.01f);
  setFilterADSR(filterA, filterD, filterS, filterR);
}

void NoiseGAudioProcessor::reset() {
  synth.reset();
}

}  // namespace audio_plugin

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new audio_plugin::NoiseGAudioProcessor();
}