#include "scene.hpp"

SpriteManager& Scene::sprite_mngr() { return *subsytems.sprite_mngr; }
AudioManager&  Scene::audio_mngr()  { return *subsytems.audio_mngr; }
InputManager&  Scene::input_mngr()  { return *subsytems.input_mngr; }
SceneManager&  Scene::scene_mngr()  { return *subsytems.scene_mngr; }
EntityManager& Scene::ent_mngr()    { return *subsytems.ent_mngr; }

void Scene::set_subsystems(GameSubSystem& ss) {
    subsytems = ss;
}

SceneManager::SceneManager() {
    system.scene_mngr = this;
}

void SceneManager::set_sprite_manager(SpriteManager* mngr) {
    system.sprite_mngr = mngr;
}

void SceneManager::set_entity_manager(EntityManager* mngr) {
    system.ent_mngr = mngr;
}

void SceneManager::set_audio_manager(AudioManager*   mngr) {
    system.audio_mngr = mngr;
}

void SceneManager::set_input_manager(InputManager*   mngr) {
    system.input_mngr = mngr;
}

void SceneManager::change_scene(std::unique_ptr<Scene> next) {
    SDL_Log("Changing scenes...");
    current_scene = std::move(next);
    if (current_scene) {
        SDL_Log(">>> Scene Changed to {%s}", current_scene->name().c_str());
        current_scene->set_subsystems(system);
        current_scene->start();
    }
}

void SceneManager::run_update() {
    if (current_scene) {
        current_scene->update();
    }
}   

void SceneManager::run_render(SDL_Renderer* rend) {
    if (current_scene) {
        current_scene->render(rend);
    }
}