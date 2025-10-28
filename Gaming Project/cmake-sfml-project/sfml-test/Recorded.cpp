/*
====================================================================================
   SFML Audio Recording and Playback Example
   -----------------------------------------
   This program demonstrates how to:
   1. Detect available recording devices
   2. Record audio from the microphone using sf::SoundBufferRecorder
   3. Save the recorded audio to a file (e.g., "recorded.ogg")
   4. Play back the recorded sound

   Requirements:
   - SFML must be compiled with audio support (libsfml-audio)
   - A working microphone/audio input device must be connected

   Compilation command (Linux):
   g++ audio_record.cpp -o audio_record $(pkg-config --cflags --libs sfml-audio sfml-system)

   Author: Siddhant Agarwal
   Date: 2025
====================================================================================
*/

#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

int main()
{
    std::cout << "==== SFML Audio Recorder Example ====\n";

    // ---------------------------------------------------------------
    // 1. Check if audio capture (microphone input) is available
    // ---------------------------------------------------------------
    if (!sf::SoundBufferRecorder::isAvailable())
    {
        std::cerr << "Error: Audio capture is not available on this system.\n";
        return -1;
    }

    // ---------------------------------------------------------------
    // 2. List available input devices
    // ---------------------------------------------------------------
    std::vector<std::string> devices = sf::SoundRecorder::getAvailableDevices();

    std::cout << "\nAvailable Input Devices:\n";
    for (size_t i = 0; i < devices.size(); ++i)
        std::cout << "  [" << i << "] " << devices[i] << "\n";

    // Choose a device (default: first one)
    std::string inputDevice = sf::SoundRecorder::getDefaultDevice();
    std::cout << "\nUsing device: " << inputDevice << "\n";

    // ---------------------------------------------------------------
    // 3. Create and configure the recorder
    // ---------------------------------------------------------------
    sf::SoundBufferRecorder recorder;

    if (!recorder.setDevice(inputDevice))
    {
        std::cerr << "Error: Failed to set recording device.\n";
        return -1;
    }

    // ---------------------------------------------------------------
    // 4. Start recording
    // ---------------------------------------------------------------
    std::cout << "\nRecording... Speak into your microphone!\n";
    recorder.start();

    // Wait 5 seconds while recording (you can change duration)
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // ---------------------------------------------------------------
    // 5. Stop recording
    // ---------------------------------------------------------------
    recorder.stop();
    std::cout << "Recording stopped.\n";

    // ---------------------------------------------------------------
    // 6. Retrieve recorded buffer
    // ---------------------------------------------------------------
    const sf::SoundBuffer& buffer = recorder.getBuffer();

    std::cout << "Captured " << buffer.getSampleCount() << " samples ("
              << buffer.getDuration().asSeconds() << " seconds).\n";

    // ---------------------------------------------------------------
    // 7. Save recorded sound to file
    // ---------------------------------------------------------------
    if (buffer.saveToFile("recorded.ogg"))
        std::cout << "Saved recorded audio to 'recorded.ogg'.\n";
    else
        std::cerr << "Error: Failed to save audio file.\n";

    // ---------------------------------------------------------------
    // 8. Play back the recorded sound
    // ---------------------------------------------------------------
    sf::Sound sound(buffer);
    std::cout << "Playing back the recorded sound...\n";
    sound.play();

    // Wait until playback finishes
    while (sound.getStatus() == sf::Sound::Playing)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Playback finished.\n";

    // ---------------------------------------------------------------
    // 9. Done
    // ---------------------------------------------------------------
    std::cout << "\nProgram finished successfully.\n";
    return 0;
}