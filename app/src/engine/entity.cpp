#include "entity.hpp"
#include <cmath>

static bool debug = false;

Entity::Entity() {
    static Uint32 sigma = 0;
    instance_id = sigma++;
}

void Entity::Render(SDL_Renderer* rend) const {
    if (!render) {
        return;
    }

    float w = sprite->w * scale.x;
    float h = sprite->h * scale.y;
    SDL_FRect rect{
        position.x - w/2,
        position.y - h/2,
        w,
        h
    };
    SDL_RenderTextureRotated(
        rend, 
        sprite->texture,
        NULL,
        &rect,
        rotation,
        NULL,
        SDL_FLIP_NONE  
    );

    if (debug) {
        SDL_SetRenderDrawColor(rend, 150, 150, 0, 255);
        // Calculate center of the rect
        float cx = rect.x + rect.w / 2.0f;
        float cy = rect.y + rect.h / 2.0f;
        // Precompute cos/sin
        float rad = rotation * (3.14159265f / 180.0f);
        float c = std::cos(rad);
        float s = std::sin(rad);
        // Corners before rotation
        SDL_FPoint pts[4] = {
            {rect.x, rect.y},
            {rect.x + rect.w, rect.y},
            {rect.x + rect.w, rect.y + rect.h},
            {rect.x, rect.y + rect.h}
        };
        // Rotate each corner
        SDL_FPoint rpts[4];
        for (int i = 0; i < 4; ++i) {
            float dx = pts[i].x - cx;
            float dy = pts[i].y - cy;
            rpts[i].x = cx + dx * c - dy * s;
            rpts[i].y = cy + dx * s + dy * c;
        }
        // Draw lines between corners (convert to int for compatibility)
        for (int i = 0; i < 4; ++i) {
            int j = (i + 1) % 4;
            SDL_RenderLine(
                rend,
                static_cast<int>(rpts[i].x), static_cast<int>(rpts[i].y),
                static_cast<int>(rpts[j].x), static_cast<int>(rpts[j].y)
            );
        }

        SDL_FRect pos{position.x-2, position.y-2, 4, 4};
        SDL_RenderFillRect(rend, &pos);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    }
}

void Entity::set_render(bool b) {
    render = b;
}

bool Entity::is_render() const {
    return render;
}

void EntityManager::set_sprite_manager(SpriteManager& manager) {
    sprite_manager = manager;
}

size_t EntityManager::get_entity_count() const {
    return entity_map.size();
}

Entity* EntityManager::get_entity_by_id(EntityID id) {
    auto it = entity_map.find(id);
    if (it != entity_map.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<EntityID> EntityManager::get_entities() const {
    return entities_alive;
}

void EntityManager::render_all_entities(SDL_Renderer* rend) const {
    for (auto ent : entity_map) {
        ent.second->Render(rend);
    }
}

Uint32 EntityManager::spawn_entity(std::string sprite_name, const Vec2 position) {
    Sprite* spr    = sprite_manager.get_sprite(sprite_name);
    if (!spr) {
        SDL_Log("[!!!] Failed to retrieve sprite {%s}", sprite_name.c_str());
        return -1;
    }

    Entity* ent    = new Entity();
    ent->position  = position;
    ent->sprite    = spr;

    entity_map[ent->instance_id] = ent;
    variable_map[ent->instance_id] = var_map();
    entities_alive.push_back(ent->instance_id);
    SDL_Log("Entity Spawned {%s} at {%.0f | %.0f}", sprite_name.c_str(), position.x, position.y);
    return ent->instance_id;
}

Entity* EntityManager::spawn_entity_p(std::string sprite_name, const Vec2 position) {
    return get_entity_by_id(spawn_entity(sprite_name, position));
}

void EntityManager::kill_entity(EntityID instance_id) {
    auto it = std::find(entities_alive.begin(), entities_alive.end(), instance_id);
    if (it != entities_alive.end()) {
        int i = std::distance(entities_alive.begin(), it);
        entities_alive.erase(entities_alive.begin() + i);
    }
    else {
        // This entity doesn't exist
        return;
    }
    
    SDL_Log(
        "Entity Killed {%d | %s}", 
        instance_id, 
        get_entity_by_id(instance_id)->sprite->sprite_name.c_str()
    );
    
    clear_vars(instance_id);
    delete(entity_map[instance_id]);
    entity_map.erase(instance_id);
}

void EntityManager::destroy_all_entities() {
    for (auto ent : entity_map) {
        kill_entity(ent.first);
    }
}

bool EntityManager::has_var(EntityID id, const std::string& key) const {
    auto it = variable_map.find(id);
    return it != variable_map.end() && it->second.count(key) > 0;
}

void EntityManager::remove_var(EntityID id, const std::string& key) {
    variable_map[id].erase(key);
}

void EntityManager::clear_vars(EntityID id) {
    variable_map.erase(id);
}

void EntityManager::debug_mode(bool mode) const {
    debug = mode;
}