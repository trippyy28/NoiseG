cmake_minimum_required(VERSION 3.22)

project(NoiseG VERSION 0.1.0)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/NoiseG")
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME TrippySamples
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    PLUGIN_MANUFACTURER_CODE MCMP
    PLUGIN_CODE EXPL
    FORMATS VST3 Standalone 
    PRODUCT_NAME "NoiseG"
    MODULES juce_audio_basics juce_graphics juce_audio_devices juce_audio_formats juce_audio_processors juce_audio_utils juce_core juce_data_structures juce_dsp juce_events juce_graphics juce_gui_basics juce_gui_extra juce_opengl juce_product_unlocking juce_video
    ASSETS_TARGET Assets
)

target_sources(${PROJECT_NAME}
    PRIVATE
        source/PluginEditor.cpp
        source/PluginProcessor.cpp
        source/Synth.cpp
        source/NinjaAnimator.cpp
        source/CustomLookAndFeel.cpp
        ${INCLUDE_DIR}/PluginEditor.h
        ${INCLUDE_DIR}/PluginProcessor.h
        ${INCLUDE_DIR}/Synth.h
        ${INCLUDE_DIR}/Voice.h
        ${INCLUDE_DIR}/NoiseGenerator.h
        ${INCLUDE_DIR}/Utils.h
        ${INCLUDE_DIR}/NinjaAnimator.h
        ${INCLUDE_DIR}/CustomLookAndFeel.h
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

if (MSVC)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Embed assets using juce_add_binary_data
juce_add_binary_data(Assets
    SOURCES
        ${ASSETS_DIR}/Untitled.png
        ${ASSETS_DIR}/Ninja.png
        ${ASSETS_DIR}/play.png
        ${ASSETS_DIR}/stop.png
        ${ASSETS_DIR}/ffont.ttf
        ${ASSETS_DIR}/Soldier.gif
        ${ASSETS_DIR}/Healer.png
        # Add more assets here
)

# Link the binary data target to the plugin target
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Assets
)

# -------------------------------------------------------------------
# Set the output directory for the VST3 bundle
# -------------------------------------------------------------------
set(PLUGIN_OUTPUT_DIR "/Users/trippyy28/Library/Audio/Plug-Ins/VST32")

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
)