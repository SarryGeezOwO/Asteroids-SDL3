// @Auhor: SarryGeezOwO
// >> Made in SDL3
// >> Made in 2025
// >> Copyright 2025

#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <queue>

#include "vector_math.hpp"
#include "engine/input.hpp"
#include "engine/sprite.hpp"
#include "engine/entity.hpp"
#include "engine/sound.hpp"
#include "scene/scene.hpp"

// Scenes
#include "scene/gamerun.hpp"

#define FPS 60
#define WIN_W 1080
#define WIN_H 720
#define APP_TITLE "Asteriods [SDL3]"
#define APP_VERSION "v1.0"

// Get
#define VAR(id, name, type)     (ent_mngr.get_var<type>(id, name))
#define VARP(ent, name, type)   (ent_mngr.get_var<type>(ent->instance_id, name))

// Set
#define SVAR(id, name, value)   (ent_mngr.set_var(id, name, value))
#define SVARP(ent, name, value) (ent_mngr.set_var(ent->instance_id, name, value))

constexpr float MS_PER_FRAME = 1000 / FPS;
Uint64 delta_time;
int frame_count = 0;
float fps = 0.0f;
Uint64 fps_timer = 0;

bool app_running = true;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

InputManager  input_mngr;
AudioManager  audio_mngr;
SpriteManager sprite_mngr;
EntityManager ent_mngr;
SceneManager  scene_mngr;

void error_msg(const std::string msg) {
    SDL_Log((msg + " {%s}").c_str(), SDL_GetError());
}

// TODO:
//     >> [/] Sound System 
//     >> [/] Add Sound effects to everything
//     >> [/] Scene Management System
//     >> [/] Player death
//     >> [x] GUI System 
//     >> [x] Main Menu
//     >> [/] Score system
//     >> [x] Final Boss: Record system (GYATTT!!!!)

void load_sprites() {
    SDL_Log("Loading Sprites to memory...");
    sprite_mngr.load_sprite(renderer, "missing", ".png");
    sprite_mngr.load_sprite(renderer, "life",    ".png");
    sprite_mngr.load_sprite(renderer, "big_rock_1", ".png");
    sprite_mngr.load_sprite(renderer, "med_rock_1", ".png");
    sprite_mngr.load_sprite(renderer, "med_rock_2", ".png");
    sprite_mngr.load_sprite(renderer, "med_rock_3", ".png");
    sprite_mngr.load_sprite(renderer, "small_rock_1", ".png");
    sprite_mngr.load_sprite(renderer, "ship",   ".png");
    sprite_mngr.load_sprite(renderer, "fire",   ".png");
    sprite_mngr.load_sprite(renderer, "bullet", ".png");
}

void load_audios() {
    SDL_Log("Loading Audios to memory...");
    audio_mngr.load_audio("laser_shoot", 0.7f);
    audio_mngr.load_audio("engine_rumble", 1);
    audio_mngr.load_audio("short_explosion", 0.6f);

    // Do this after loading all audio files for a proper master volume configuration
    audio_mngr.set_master_volume(1);
}

void start() {
    input_mngr.debug_mode(false);
    ent_mngr.debug_mode(false);
    SDL_srand(0);

    // Yep 🔥🔥🔥
    scene_mngr.change_scene(std::make_unique<Scene_RunGame>());
}

void update() {
    scene_mngr.run_update();
}

void render() {
    scene_mngr.run_render(renderer);
}

// ------------------------- SYSTEM -----------------------

void quit_app() {    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    SDL_Log("Application Closing... {%s}", APP_TITLE);
    SDL_Quit();
}

void init_window() {
    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_AUDIO)) {
        error_msg("SDL Failed Initializing...");
        quit_app();
    }

    window = SDL_CreateWindow(APP_TITLE, WIN_W, WIN_H, SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) {
        error_msg("Creation of window, failed...");
        quit_app();
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        error_msg("Creation of renderer, failed...");
        quit_app();
    }
}

int main(int argc, char const* argv[]) {
    SDL_Log("Application starting...");
    SDL_Log("Application name {%s}", APP_TITLE);
    SDL_Log("SDL_BasePath: {%s}", SDL_GetBasePath());
    
    SDL_SetAppMetadata(APP_TITLE, APP_VERSION, "com.sarrygeezOwO.asteroids");
    init_window();

    load_sprites();
    load_audios();
    ent_mngr.set_sprite_manager(sprite_mngr);
    scene_mngr.set_sprite_manager(&sprite_mngr);
    scene_mngr.set_entity_manager(&ent_mngr);
    scene_mngr.set_audio_manager(&audio_mngr);
    scene_mngr.set_input_manager(&input_mngr);
    
    Uint64 previous;
    Uint64 now;
    Uint64 lag;

    start();
    while(app_running) {
        input_mngr.handle_input();
        app_running = !input_mngr.has_quit();

        previous = now;
        now = SDL_GetTicks();

        delta_time = now - previous;
        lag += delta_time;

        frame_count++;
        fps_timer += delta_time;
        if (fps_timer >= 1000) {
            fps = frame_count * 1000.0f / fps_timer;
            frame_count = 0;
            fps_timer = 0;
        }

        while (lag >= MS_PER_FRAME) {
            update();
            // input.update_graph(SDL_SCANCODE_W);
            input_mngr.update();             
            lag -= MS_PER_FRAME;
        }

        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        ent_mngr.render_all_entities(renderer);
        render();

        SDL_RenderPresent(renderer);
    }

    quit_app();
    return 0;
}