#include "sound.hpp"

// Helper 
float clamp01(float x) {
    return (x < 0.0f) ? 0.0f : (x > 1.0f ? 1.0f : x);
}


Audio::Audio() {
    // Don't ever call this constructor ever
}

Audio::~Audio() {
    free();
}

Audio::Audio(const char* audio_name) {
    info.name = audio_name;

    SDL_AudioSpec spec;
    char* wav_path = nullptr;

    SDL_asprintf(&wav_path, "%sassets\\sounds\\%s.wav", SDL_GetBasePath(), audio_name);
    if (!SDL_LoadWAV(wav_path, &spec, &buffer, &buffer_len)) {
        SDL_Log("[!!!] Couldn't load .wav file {%s}", audio_name);
        error = true;
    }
    
    // Create Stream
    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!stream && !error) {
        SDL_Log("[!!!] Couldn't create audio stream {%s}", wav_path);
        error = true;
    }
    
    if (!error) SDL_Log(">>> Successfully loaded Audio to memory {%s}", wav_path);
    SDL_free(wav_path);
}

bool Audio::has_error() const {
    return error;
}

void Audio::apply_gain(float master_gain) {
    float final_gain = clamp01(info.gain) * clamp01(master_gain);
    SDL_SetAudioStreamGain(stream, final_gain);
}

void Audio::set_gain(float f) {
    info.gain = f;
}

float Audio::get_gain() const {
    return SDL_GetAudioStreamGain(stream);
}

void Audio::set_speed(float s) {
    info.speed = s;
    SDL_SetAudioStreamFrequencyRatio(stream, s);
}

float Audio::get_speed() const {
    return SDL_GetAudioStreamFrequencyRatio(stream);
}

void Audio::free() {
    SDL_DestroyAudioStream(stream);
    SDL_free(buffer);
}

AudioInfo Audio::get_info() const {
    return info;
}

void AudioManager::load_audio(const char* audio_name, float gain) {
    auto audio = std::make_unique<Audio>(audio_name);
    if (audio->has_error()) 
        return;
    audio->set_gain(gain);
    audio_map[audio_name] = std::move(audio);
}

Audio* AudioManager::get_audio(const char* audio_name) {
    // We should still let the game continue even with missing audio
    if (!audio_exist(audio_name)) {
        return nullptr;
    }
    return audio_map[audio_name].get();
}

bool AudioManager::audio_exist(const char* audio_name) const {
    auto it = audio_map.find(audio_name);
    bool res = (it != audio_map.end());
    if (!res) 
        SDL_Log("Audio does not exists... {%s}", audio_name);
    return res;
}

void AudioManager::set_master_volume(float f) {
    master_gain = f;
    for (auto& pair : audio_map) {
        Audio* audio = get_audio(pair.first.c_str());
        if (!audio) 
            continue;
        audio->apply_gain(master_gain);
    }
}

float AudioManager::get_master_volume() const {
    return master_gain;
}

void AudioManager::play(const char* audio_name) {
    Audio* audio = get_audio(audio_name);
    if (!audio) {
        SDL_Log("Failed to play audio {%s}", audio_name);
        return;   
    }

    if (SDL_GetAudioStreamQueued(audio->stream) < (int)audio->buffer_len) {
        SDL_PutAudioStreamData(audio->stream, audio->buffer, audio->buffer_len);
    }
    SDL_ResumeAudioStreamDevice(audio->stream);
}

void AudioManager::pause(const char* audio_name) {
    Audio* audio = get_audio(audio_name);
    if (!audio) {
        SDL_Log("Failed to pause audio {%s}", audio_name);
        return;   
    }

    SDL_PauseAudioStreamDevice(audio->stream);
}

void AudioManager::reset(const char* audio_name) {
    Audio* audio = get_audio(audio_name);
    if (!audio) {
        SDL_Log("Failed to reset audio {%s}", audio_name);
        return;   
    }

    SDL_ClearAudioStream(audio->stream); 
}

void AudioManager::set_gain(const char* audio_name, float gain) {
    Audio* audio = get_audio(audio_name);
    if (!audio) {
        SDL_Log("Failed to set gain to audio {%s}", audio_name);
        return;   
    }

    audio->set_gain(gain);
    audio->apply_gain(master_gain);
}

AudioInfo AudioManager::get_audio_info(const char* audio_name) {
    Audio* audio = get_audio(audio_name);
    if (!audio) {
        SDL_Log("Failed to retrieve audio info {%s}", audio_name);
        return AudioInfo();   
    }
    return audio->get_info();
}