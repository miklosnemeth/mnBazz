cmake_minimum_required(VERSION 3.22)
project(mnBazz VERSION 1.0.1)
include(cmake/Version.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 9.0.5
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_binary_data(mnBazz_Fonts SOURCES
    Resources/Fonts/Montserrat-Bold.ttf
    Resources/Fonts/RobotoCondensed.ttf
    Resources/Fonts/RobotoCondensed-Bold.ttf)

juce_add_plugin(mnBazz
    COMPANY_NAME "mn"
    PLUGIN_MANUFACTURER_CODE Mnbz
    PLUGIN_CODE Mbaz
    FORMATS VST3 Standalone
    PRODUCT_NAME "mnBazz"
    VERSION ${PROJECT_VERSION}
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Instrument Synth)

target_sources(mnBazz PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_include_directories(mnBazz PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")

target_compile_definitions(mnBazz PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(mnBazz
    PRIVATE
        mnBazz_Fonts
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
