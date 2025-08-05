#ifndef GAMERUN_HPP
#define GAMERUN_HPP

#include "scene.hpp"

enum AsteroidType {
    SMALL, MEDIUM, BIG
};

struct Scene_RunGame : Scene {
    // Utility / game logic functions
private:
    void wrap_around(Entity* ent);
    void kill(EntityID id);
    void kill(std::initializer_list<EntityID> ids);
    void kill(Entity* ent);
    void handle_kill_list();
    std::string get_random_asteroid_sprite(AsteroidType type, SpriteManager& manager);
    float random_angle();
    Vec2 random_initial_velocity();
    void spawn_asteroid(AsteroidType type);
    void spawn_asteroid(AsteroidType type, Vec2 initial_pos);
    void destroy_asteroid(EntityID id);

public:
    void start() override;
    void update() override;
    void render(SDL_Renderer* rend) override;
    std::string name() override;
    ~Scene_RunGame() override;
};

#endif