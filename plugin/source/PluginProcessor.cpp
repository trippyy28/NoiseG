#include "NoiseG/PluginProcessor.h"
#include "NoiseG/PluginEditor.h"

namespace audio_plugin {

NoiseGAudioProcessor::NoiseGAudioProcessor()
    : volume(0.5f),
      modAmount(1.0f),  // אתחול ברירת מחדל ל־volume
      AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      apvts(*this, nullptr, "PARAMS", createParameterLayout()) {
  volumeParam =
      apvts.getRawParameterValue(ParameterID::outputLevel.getParamID());
  cutoffParam =
      apvts.getRawParameterValue(ParameterID::filterFreq.getParamID());
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

  const float dbGain = volumeParam ? static_cast<float>(*volumeParam) : -6.f;
  const float cutoffHz =
      cutoffParam ? static_cast<float>(*cutoffParam) : 1000.f;

  // העבר ל-Synth
  const float linearGain = juce::Decibels::decibelsToGain(dbGain);
  synth.setVolume(linearGain);
  synth.setCutoff(cutoffHz);
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
  for (auto& v : synth.voices)
    v.setAmpADSR(a, d, s, r);
}
void NoiseGAudioProcessor::setFilterADSR(float a, float d, float s, float r) {
  DBG("setFilter called - A: " << a << ", D: " << d << ", S: " << s
                               << ", R: " << r);
  for (auto& v : synth.voices)
    v.setFilterADSR(a, d, s, r);
}

void NoiseGAudioProcessor::setModulationFilter(float amount) {
  modAmount = amount;
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

  auto apvtsState = apvts.copyState();
  std::unique_ptr<juce::XmlElement> apvtsXml(apvtsState.createXml());

  // 2) ה-XML המותאם שלך
  juce::XmlElement root("MyPluginState");
  root.setAttribute("waveform", static_cast<int>(synth.getWaveform()));
  root.setAttribute("ampAttack", synth.voices[0].ampParams.attack);
  root.setAttribute("ampDecay", synth.voices[0].ampParams.decay);
  root.setAttribute("ampSustain", synth.voices[0].ampParams.sustain);
  root.setAttribute("ampRelease", synth.voices[0].ampParams.release);
  root.setAttribute("filterAttack", synth.voices[0].filterParams.attack);
  root.setAttribute("filterDecay", synth.voices[0].filterParams.decay);
  root.setAttribute("filterSustain", synth.voices[0].filterParams.sustain);
  root.setAttribute("filterRelease", synth.voices[0].filterParams.release);
  root.setAttribute("filterRes", getFilterRes());
  root.setAttribute("modFilterAmount", modAmount);
  root.setAttribute("filterEnabled", synth.getFilterEnbaled());

  // 3) קנן את ה-APVTS כ-child כדי שהכול יישמר יחד
  if (apvtsXml)
    root.addChildElement(apvtsXml.release());

  copyXmlToBinary(root, destData);
}

void NoiseGAudioProcessor::setStateInformation(const void* data,
                                               int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (!xml || !xml->hasTagName("MyPluginState"))
    return;

  // 1) שחזר APVTS אם נמצא child בשם ValueTreeState
  if (auto* apvtsXml = xml->getChildByName("ValueTreeState")) {
    juce::ValueTree vt = juce::ValueTree::fromXml(*apvtsXml);
    if (vt.isValid())
      apvts.replaceState(
          vt);  // זה יטעין את ה-volume/cutoff וכל מה שמופיע ב-layout
  }

  // 2) שחזר את השדות המותאמים שלך
  if (xml->hasAttribute("waveform")) {
    setWaveform(xml->getIntAttribute("waveform"));
  }
  // שאר ה-ADSR/Filter שלך:
  float ampA = xml->getDoubleAttribute("ampAttack", 0.01f);
  float ampD = xml->getDoubleAttribute("ampDecay", 0.1f);
  float ampS = xml->getDoubleAttribute("ampSustain", 1.0f);
  float ampR = xml->getDoubleAttribute("ampRelease", 0.1f);
  setAmpADSR(ampA, ampD, ampS, ampR);

  float filterA = xml->getDoubleAttribute("filterAttack", 0.01f);
  float filterD = xml->getDoubleAttribute("filterDecay", 0.01f);
  float filterS = xml->getDoubleAttribute("filterSustain", 1.0f);
  float filterR = xml->getDoubleAttribute("filterRelease", 0.01f);
  setFilterADSR(filterA, filterD, filterS, filterR);

  if (xml->hasAttribute("filterRes"))
    synth.setFilterResonance((float)xml->getDoubleAttribute("filterRes", 1.0f));

  if (xml->hasAttribute("modFilterAmount"))
    synth.setFilterModAmount(
        (float)xml->getDoubleAttribute("modFilterAmount", modAmount));

  if (xml->hasAttribute("filterEnabled"))
    synth.setFilterEnabled(xml->getBoolAttribute("filterEnabled", false));

  // אין צורך לטעון כאן volume/cutoff ידנית — APVTS כבר עשה זאת דרך
  // replaceState.
}

juce::AudioProcessorValueTreeState::ParameterLayout
NoiseGAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  // you will add the parameters here soon

  layout.add(std::make_unique<juce::AudioParameterChoice>(
      ParameterID::polyMode, "Polyphony", juce::StringArray{"Mono", "Poly"},
      1));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      ParameterID::outputLevel, "Output",
      juce::NormalisableRange<float>(-60.f, 0.f), -6.f));

  auto hz = juce::NormalisableRange<float>(20.f, 20000.f);
  hz.setSkewForCentre(1000.f);

  layout.add(std::make_unique<juce::AudioParameterFloat>(
      ParameterID::filterFreq, "Cutoff", hz, 1000.0f));
  return layout;
}

void NoiseGAudioProcessor::reset() {
  synth.reset();
}

}  // namespace audio_plugin

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new audio_plugin::NoiseGAudioProcessor();
}
