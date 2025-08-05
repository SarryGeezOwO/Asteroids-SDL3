#include "input.hpp"

void InputManager::update() {
    prevKeymap = keymap;
}

void InputManager::debug_mode(bool _b) {
    debug = _b;
}

bool InputManager::prev_check_key(SDL_Scancode key) const {
    return prevKeymap[key];
}

bool InputManager::is_debug() const {
    return debug;
}

void InputManager::update_graph(SDL_Scancode key) {
    float p = check_key_pressed(*this, key);
    float c = check_key(*this, key);
    float r = check_key_release(*this, key);
    float val = ((p/2) + (c/2)) - r;

    graph_yVal.emplace(graph_yVal.end(), -val);
    if (graph_yVal.size() > 200) {
        graph_yVal.erase(graph_yVal.begin());
    }
}

void InputManager::render_graph(SDL_Renderer* rend) {
    SDL_SetRenderDrawColor(rend, 255, 255, 0, 255);

    SDL_FPoint points[graph_yVal.size()];
    size_t gap = 0;
    for (size_t i = 0; i < graph_yVal.size(); i++) {
        points[i].x = i+gap+20;
        points[i].y = (graph_yVal[i] * 20) + 60;
        gap += 1;
    }

    SDL_RenderLines(rend, points, graph_yVal.size());
}

bool InputManager::has_quit() const {
    return quit;
}

void InputManager::handle_input() {
    while (SDL_PollEvent(&event)) {
        SDL_Scancode key = event.key.scancode;
        
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            quit = true;
            break;
            
        case SDL_EVENT_KEY_DOWN:
            keymap[key] = true;
            break;
        
        case SDL_EVENT_KEY_UP:
            keymap[key] = false;
            break;

        default: break;
        }
    }
}

bool check_key(const InputManager& manager, SDL_Scancode key) {
    return manager.keymap[key];
}

bool check_key_pressed(const InputManager& manager, SDL_Scancode key) {
    bool res = (!manager.prev_check_key(key) && check_key(manager, key));
    if (manager.is_debug() && res) {
        SDL_Log("Key Pressed Success: Input {%s}", SDL_GetScancodeName(key));
    }
    return res;
}

bool check_key_release(const InputManager& manager, SDL_Scancode key) {
    bool res = (manager.prev_check_key(key) && !check_key(manager, key));
    if (manager.is_debug() && res) {
        SDL_Log("Key Release Success: Input {%s}", SDL_GetScancodeName(key));
    }
    return res;
}