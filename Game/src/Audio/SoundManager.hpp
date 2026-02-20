#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <memory>

namespace game::audio {

enum class SoundEffect {
    PlayerWalk,
    PlayerAttack,
    PlayerHit,
    EnemyHit,
    EnemyDeath,
    GameOver
};

enum class Music {
    MainTheme,
    BattleTheme
};

class SoundManager {
public:
    SoundManager();
    
    // Load sounds
    bool loadSound(SoundEffect effect, const std::string& filepath);
    bool loadMusic(Music music, const std::string& filepath);
    
    // Play sounds
    void playSound(SoundEffect effect, float volume = 100.f);
    void playMusic(Music music, bool loop = true, float volume = 50.f);
    
    // Control
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    
    void setMasterVolume(float volume); // 0-100
    void setSoundVolume(float volume);  // 0-100
    void setMusicVolume(float volume);  // 0-100
    
    void stopAllSounds();
    
    // Update (call every frame to manage sound instances)
    void update();

    void playLoopingSound(SoundEffect effect, float volume = 100.f);
    void stopLoopingSound(SoundEffect effect);
    void stopAllLoopingSounds();
    
private:
    // Sound buffers (must persist)
    std::unordered_map<SoundEffect, sf::SoundBuffer> m_soundBuffers;
    
    // Sound instances (multiple can play at once)
    std::vector<std::unique_ptr<sf::Sound>> m_activeSounds;
    
    // Music (only one plays at a time)
    std::unordered_map<Music, std::unique_ptr<sf::Music>> m_music;
    Music m_currentMusic;
    
    // Volume settings
    float m_masterVolume = 100.f;
    float m_soundVolume = 100.f;
    float m_musicVolume = 50.f;
    
    float getEffectiveVolume(float localVolume, bool isMusic) const;

    // Looping sound channels (for sustained effects like footsteps)
    std::unordered_map<SoundEffect, std::unique_ptr<sf::Sound>> m_loopingSounds;

};

} // namespace game::audio