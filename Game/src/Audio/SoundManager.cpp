#include "SoundManager.hpp"
#include <iostream>
#include <algorithm>

namespace game::audio {

SoundManager::SoundManager()
    : m_currentMusic(Music::MainTheme)
{
}

bool SoundManager::loadSound(SoundEffect effect, const std::string& filepath)
{
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filepath)) {  // SoundBuffer uses loadFromFile
        std::cerr << "Failed to load sound: " << filepath << "\n";
        return false;
    }
    
    m_soundBuffers[effect] = std::move(buffer);
    std::cout << "Loaded sound: " << filepath << "\n";
    return true;
}

bool SoundManager::loadMusic(Music music, const std::string& filepath)
{
    auto musicPtr = std::make_unique<sf::Music>();
    if (!musicPtr->openFromFile(filepath)) {  // Music uses openFromFile
        std::cerr << "Failed to load music: " << filepath << "\n";
        return false;
    }
    
    m_music[music] = std::move(musicPtr);
    std::cout << "Loaded music: " << filepath << "\n";
    return true;
}

void SoundManager::playSound(SoundEffect effect, float volume)
{
    auto it = m_soundBuffers.find(effect);
    if (it == m_soundBuffers.end()) {
        std::cerr << "Sound effect not loaded\n";
        return;
    }
    
    // SFML 3: Create sound with buffer in constructor
    auto sound = std::make_unique<sf::Sound>(it->second);
    sound->setVolume(getEffectiveVolume(volume, false));
    sound->play();
    
    m_activeSounds.push_back(std::move(sound));
}

void SoundManager::playMusic(Music music, bool loop, float volume)
{
    auto it = m_music.find(music);
    if (it == m_music.end()) {
        std::cerr << "Music not loaded\n";
        return;
    }
    
    // Stop current music if playing
    stopMusic();
    
    m_currentMusic = music;
    it->second->setLooping(loop);  // SFML 3: setLooping instead of setLoop
    it->second->setVolume(getEffectiveVolume(volume, true));
    it->second->play();
    
    std::cout << "Playing music (loop: " << loop << ")\n";
}

void SoundManager::stopMusic()
{
    for (auto& [music, musicPtr] : m_music) {
        if (musicPtr && musicPtr->getStatus() == sf::Sound::Status::Playing) {
            musicPtr->stop();
        }
    }
}

void SoundManager::pauseMusic()
{
    for (auto& [music, musicPtr] : m_music) {
        if (musicPtr && musicPtr->getStatus() == sf::Sound::Status::Playing) {
            musicPtr->pause();
        }
    }
}

void SoundManager::resumeMusic()
{
    auto it = m_music.find(m_currentMusic);
    if (it != m_music.end() && it->second) {
        if (it->second->getStatus() == sf::Sound::Status::Paused) {
            it->second->play();
        }
    }
}

void SoundManager::setMasterVolume(float volume)
{
    m_masterVolume = std::clamp(volume, 0.f, 100.f);
    
    // Update all active sounds
    for (auto& sound : m_activeSounds) {
        if (sound) {
            sound->setVolume(getEffectiveVolume(m_soundVolume, false));
        }
    }
    
    // Update music
    for (auto& [music, musicPtr] : m_music) {
        if (musicPtr) {
            musicPtr->setVolume(getEffectiveVolume(m_musicVolume, true));
        }
    }
}

void SoundManager::setSoundVolume(float volume)
{
    m_soundVolume = std::clamp(volume, 0.f, 100.f);
    
    for (auto& sound : m_activeSounds) {
        if (sound) {
            sound->setVolume(getEffectiveVolume(m_soundVolume, false));
        }
    }
}

void SoundManager::setMusicVolume(float volume)
{
    m_musicVolume = std::clamp(volume, 0.f, 100.f);
    
    for (auto& [music, musicPtr] : m_music) {
        if (musicPtr) {
            musicPtr->setVolume(getEffectiveVolume(m_musicVolume, true));
        }
    }
}

void SoundManager::stopAllSounds()
{
    for (auto& sound : m_activeSounds) {
        if (sound) {
            sound->stop();
        }
    }
    m_activeSounds.clear();
}

void SoundManager::update()
{
    // Remove finished sounds
    m_activeSounds.erase(
        std::remove_if(m_activeSounds.begin(), m_activeSounds.end(),
            [](const auto& sound) {
                return !sound || sound->getStatus() == sf::Sound::Status::Stopped;
            }),
        m_activeSounds.end()
    );
}

float SoundManager::getEffectiveVolume(float localVolume, bool isMusic) const
{
    float baseVolume = isMusic ? m_musicVolume : m_soundVolume;
    return (baseVolume / 100.f) * (m_masterVolume / 100.f) * localVolume;
}

void SoundManager::playLoopingSound(SoundEffect effect, float volume)
{
    // Already playing? Don't restart it
    auto existing = m_loopingSounds.find(effect);
    if (existing != m_loopingSounds.end() &&
        existing->second->getStatus() == sf::Sound::Status::Playing) {
        return;
    }

    auto it = m_soundBuffers.find(effect);
    if (it == m_soundBuffers.end()) {
        std::cerr << "Sound effect not loaded\n";
        return;
    }

    auto sound = std::make_unique<sf::Sound>(it->second);
    sound->setLooping(true);
    sound->setVolume(getEffectiveVolume(volume, false));
    sound->play();
    m_loopingSounds[effect] = std::move(sound);
}

void SoundManager::stopLoopingSound(SoundEffect effect)
{
    auto it = m_loopingSounds.find(effect);
    if (it != m_loopingSounds.end()) {
        it->second->stop();
        m_loopingSounds.erase(it);
    }
}

void SoundManager::stopAllLoopingSounds()
{
    for (auto& [effect, sound] : m_loopingSounds)
        if (sound) sound->stop();
    m_loopingSounds.clear();
}

} // namespace game::audio