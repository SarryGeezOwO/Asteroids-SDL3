#ifndef ENTITY_HPP
#define ENTITY_HPP

// Every entity only has Center Pivot
// No animation, bro this is asteroids
#include <iostream>
#include <algorithm>
#include <any>
#include "sprite.hpp"
#include "../vector_math.hpp"

typedef Uint32 EntityID;

// Living instance of an EntityObject
struct Entity {
private:
    bool render = true;

public:
    EntityID        instance_id;
    Sprite*         sprite;
    Vec2            position = {0, 0};
    Vec2            scale    = {1, 1};
    float           rotation = 0;

    Entity();
    void Render(SDL_Renderer* rend) const;
    void set_render(bool b);
    bool is_render() const;
};

struct EntityManager {
private:
    SpriteManager sprite_manager;
    std::unordered_map<EntityID, Entity*> entity_map; // Instance_ID, Entity Pointer
    std::vector<EntityID> entities_alive;

    // Variable holding
    typedef std::unordered_map<std::string, std::any> var_map;
    std::unordered_map<EntityID, var_map> variable_map;

public:
    void set_sprite_manager(SpriteManager& manager);
    std::vector<EntityID> get_entities() const;
    Entity* get_entity_by_id(EntityID id);
    size_t get_entity_count() const;
    void render_all_entities(SDL_Renderer* rend) const;
    
    EntityID spawn_entity(std::string sprite_name, const Vec2 position);
    Entity* spawn_entity_p(std::string sprite_name, const Vec2 position);
    void kill_entity(EntityID instance_id);
    void destroy_all_entities();

    template<typename T>
    void set_var(EntityID id, const std::string& key, T&& value) {
        variable_map[id][key] = std::forward<T>(value);
    }

    template<typename T>
    T get_var(EntityID id, const std::string& key) const {
        auto ent_it = variable_map.find(id);
        if (ent_it == variable_map.end()) {
            throw std::runtime_error("Entity not found... {" + std::to_string(id) + "}");
        }

        const auto& var_map = ent_it->second;
        auto var_it = var_map.find(key);
        if (var_it == var_map.end()) {
            throw std::runtime_error("Variable not found... {" + std::to_string(id) + " | " + key.c_str() + "}");
        }

        try {
            return std::any_cast<T>(var_it->second);
        }
        catch(const std::bad_any_cast&) {
            throw std::runtime_error("Bad any_cast for key: " + key);
        }
    }

    bool has_var(EntityID id, const std::string& key) const;
    void remove_var(EntityID id, const std::string& key);
    void clear_vars(EntityID id);
    void debug_mode(bool mode) const;
};


#endif