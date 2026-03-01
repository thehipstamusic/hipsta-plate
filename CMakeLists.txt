cmake_minimum_required(VERSION 3.22)
project(IvanSound VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# clang-tidy linter (enable with -DENABLE_CLANG_TIDY=ON)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found, linting disabled")
    endif()
endif()

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.12
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(IvanSound
    COMPANY_NAME "IvanSound"
    PLUGIN_MANUFACTURER_CODE Ivan
    PLUGIN_CODE Ivs1
    FORMATS VST3
    PRODUCT_NAME "IvanSound"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(IvanSound
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

target_compile_definitions(IvanSound
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(IvanSound
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Sign the VST3 after build (ad-hoc if no identity specified)
set(CODESIGN_IDENTITY "-" CACHE STRING "Code signing identity (use '-' for ad-hoc)")
add_custom_command(TARGET IvanSound_VST3 POST_BUILD
    COMMAND codesign --force --sign "${CODESIGN_IDENTITY}" --deep --strict
            "$<TARGET_BUNDLE_DIR:IvanSound_VST3>"
    COMMENT "Signing IvanSound.vst3 with identity: ${CODESIGN_IDENTITY}"
)
