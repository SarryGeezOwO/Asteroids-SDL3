#ifndef SOUND_CPP
#define SOUND_CPP

// Sadly I raw SDL3 only supports .wav files for audios

#include <unordered_map>
#include <SDL3/SDL.h>
#include <string>
#include <memory>

struct AudioInfo {
    // Default value for missing info
    std::string name = "...";
    float gain = -1;
    float speed = -1;
};

struct Audio {
private:
    bool error = false;
    AudioInfo info;

public:
    SDL_AudioStream* stream = nullptr;
    Uint8* buffer = nullptr;
    Uint32 buffer_len;

    Audio();
    Audio(const char* audio_name);
    void free();
    bool has_error() const;

    void apply_gain(float master_gain);
    void set_gain(float g);
    float get_gain() const;

    void set_speed(float s);
    float get_speed() const;
    AudioInfo get_info() const;
    ~Audio();
};

struct AudioManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Audio>>audio_map;
    float master_gain = 1;
    Audio* get_audio(const char* audio_name);

public:
    void set_master_volume(float f);
    float get_master_volume() const;
    bool audio_exist(const char* audio_name) const;

    void load_audio(const char* audio_name, float gain = 1);
    void play(const char* audio_name);
    void pause(const char* audio_name);
    void reset(const char* audio_name);
    void set_gain(const char* audio_name, float gain);
    AudioInfo get_audio_info(const char* name);
};

#endif