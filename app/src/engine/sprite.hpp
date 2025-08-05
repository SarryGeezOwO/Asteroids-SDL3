#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include <unordered_map>
#include <SDL3_image/SDL_image.h>
#include <cstring> 

struct Sprite {
    std::string     sprite_name;
    int             w;
    int             h;
    SDL_Texture*    texture = nullptr;
};

struct SpriteManager {
private:
    std::unordered_map<std::string, Sprite*> sprite_map;

public:
    void load_sprite(SDL_Renderer* rend, std::string name, const char* extension);
    void destroy_all_sprites();
    void render_sprite(SDL_Renderer* rend, int x, int y, std::string name);
    Sprite* get_sprite(std::string name);
    std::vector<std::string> get_all_sprite_names() const;
};

#endif