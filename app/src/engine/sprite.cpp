#include "sprite.hpp"

void SpriteManager::load_sprite(SDL_Renderer* rend, std::string name, const char* extension) {
    std::string file;
    // SDL_GetBasePath() + "assets/images/" + name + extension;
    file.append(SDL_GetBasePath());
    file.append("assets\\images\\");
    file.append(name);
    file.append(extension);
    SDL_Surface* surf   = IMG_Load(file.c_str());
    if (!surf) {
        SDL_Log("[!!!] Failed Loading Sprite {%s}", file.c_str());
        return;
    }

    SDL_Texture* tex  = SDL_CreateTextureFromSurface(rend, surf);
    if (!tex) {
        SDL_Log("[!!!] Failed at creating texture from surface {%s}", file.c_str());
        return;
    }

    Sprite* spr       = new Sprite();
    spr->sprite_name  = name;
    spr->texture      = tex;
    spr->w            = surf->w;
    spr->h            = surf->h;
    sprite_map[name]  = spr;
    SDL_Log(">> Sprite Loaded Successfully! {%s} size{%dx%d}", file.c_str(), spr->w, spr->h);
    SDL_DestroySurface(surf);
}

void SpriteManager::destroy_all_sprites() {
    for (auto spr : sprite_map) {
        SDL_DestroyTexture(spr.second->texture);
        delete spr.second;
    }
}

Sprite* SpriteManager::get_sprite(std::string name) {
    Sprite* spr = sprite_map[name];
    if (!spr) SDL_Log("[!!!] Failed to retrieve sprite {%s}", name.c_str());
    return spr;
}

std::vector<std::string> SpriteManager::get_all_sprite_names() const {
    std::vector<std::string> names;
    for (const auto& pair : sprite_map) {
        names.push_back(pair.first);
    }
    return names;
}

void SpriteManager::render_sprite(SDL_Renderer* rend, int x, int y, std::string name) {
    Sprite* spr = get_sprite("life");
    SDL_FRect rect = {(float)x, (float)y, (float)spr->w, (float)spr->h};
    SDL_RenderTexture(
        rend,
        spr->texture,
        NULL,
        &rect
    );
}