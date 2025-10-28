#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cmath>

/**
 * @brief Custom streaming class that loops an audio buffer continuously.
 * 
 * This example demonstrates how to:
 *  - Record audio from the microphone using sf::SoundBufferRecorder
 *  - Stream it back using a custom sf::SoundStream subclass
 *  - Properly handle looping playback
 */
class LoopingStream : public sf::SoundStream {
public:
    LoopingStream() : m_currentSample(0), m_loop(false) {}

    /**
     * @brief Load the audio data from an sf::SoundBuffer.
     * @param buffer The buffer containing recorded or loaded sound data.
     * @param loop Whether to enable looping playback.
     * @return true if loading succeeded.
     */
    bool load(const sf::SoundBuffer& buffer, bool loop = true) {
        m_samples = buffer.getSamples();
        m_sampleCount = buffer.getSampleCount();
        m_currentSample = 0;
        m_loop = loop;

        // Initialize the stream with the buffer's format
        initialize(buffer.getChannelCount(), buffer.getSampleRate());
        return true;
    }

protected:
    /**
     * @brief Called automatically when the sound stream needs new audio data.
     */
    bool onGetData(Chunk& data) override {
        const std::size_t samplesPerChunk = 44100; // ~1 second at 44100 Hz

        if (m_currentSample >= m_sampleCount) {
            if (m_loop) {
                m_currentSample = 0; // restart loop
            } else {
                return false; // stop streaming
            }
        }

        std::size_t remaining = m_sampleCount - m_currentSample;
        std::size_t count = std::min(samplesPerChunk, remaining);

        m_tempSamples.assign(m_samples + m_currentSample,
                             m_samples + m_currentSample + count);

        data.samples = m_tempSamples.data();
        data.sampleCount = m_tempSamples.size();

        m_currentSample += count;
        return true;
    }

    /**
     * @brief Called automatically when the stream starts.
     */
    void onSeek(sf::Time timeOffset) override {
        m_currentSample = static_cast<std::size_t>(
            timeOffset.asSeconds() * getSampleRate() * getChannelCount());
        if (m_currentSample > m_sampleCount)
            m_currentSample = m_sampleCount;
    }

private:
    const sf::Int16* m_samples = nullptr;
    std::size_t m_sampleCount = 0;
    std::size_t m_currentSample = 0;
    bool m_loop;
    std::vector<sf::Int16> m_tempSamples;
};

int main() {
    // 1️⃣ Check if audio capture is supported
    if (!sf::SoundBufferRecorder::isAvailable()) {
        std::cerr << "Audio recording not supported on this system.\n";
        return -1;
    }

    // 2️⃣ Record some audio
    sf::SoundBufferRecorder recorder;
    std::cout << "Recording for 5 seconds... Speak now!\n";
    recorder.start();
    sf::sleep(sf::seconds(5));
    recorder.stop();

    // 3️⃣ Retrieve recorded buffer
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    buffer.saveToFile("recorded.ogg"); // optional

    // 4️⃣ Play back using custom looping stream
    LoopingStream stream;
    stream.load(buffer, true);
    stream.play();

    std::cout << "Playing recorded audio in a loop... Press Enter to quit.\n";
    std::cin.ignore();

    return 0;
}