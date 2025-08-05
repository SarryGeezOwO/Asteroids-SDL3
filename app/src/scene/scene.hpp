#ifndef SCENE_HPP
#define SCENE_HPP

#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <memory>

#include "../engine/sprite.hpp"
#include "../engine/entity.hpp"
#include "../engine/sound.hpp"
#include "../engine/input.hpp"

struct SceneManager;

struct GameSubSystem {
    SpriteManager* sprite_mngr  = nullptr;
    EntityManager* ent_mngr     = nullptr;
    AudioManager*  audio_mngr   = nullptr;
    InputManager*  input_mngr   = nullptr;
    SceneManager*  scene_mngr   = nullptr;
};

struct Scene {
protected: 
    GameSubSystem subsytems;

public:
    SpriteManager& sprite_mngr();
    AudioManager& audio_mngr();
    InputManager& input_mngr();
    SceneManager& scene_mngr();
    EntityManager& ent_mngr();
    void set_subsystems(GameSubSystem& ss);

    virtual void start() = 0;
    virtual void update() = 0;
    virtual void render(SDL_Renderer* rend) = 0;
    virtual std::string name() = 0;
    virtual ~Scene() = 0;
};
inline Scene::~Scene() {};

struct SceneManager {
private:
    std::unique_ptr<Scene> current_scene;
    GameSubSystem system;

public:
    SceneManager();
    void set_sprite_manager(SpriteManager* mngr);
    void set_entity_manager(EntityManager* mngr);
    void set_audio_manager(AudioManager*   mngr);
    void set_input_manager(InputManager*   mngr);

    // Free up the current Scene from memory and point to the next Scene
    // Also calls the start() function of the next scene
    void change_scene(std::unique_ptr<Scene> next);
    void run_update();
    void run_render(SDL_Renderer* rend);
};

#endif