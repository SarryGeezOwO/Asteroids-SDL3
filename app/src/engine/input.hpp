#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL3/SDL.h>
#include <bitset>
#include <vector>

struct InputManager {
private:
    std::bitset<SDL_SCANCODE_COUNT> prevKeymap;  // Keep track of Keymap on every Game Update
    bool debug = false;
    bool quit = false;
    std::vector<float> graph_yVal;

public:
    std::bitset<SDL_SCANCODE_COUNT> keymap;      // Keymap updated every FRAME
    SDL_Event event;
    void update();                               // Updates the InputManager on GameUpdate
    void debug_mode(bool _b);                    // Literally SDL_Logs or something
    bool is_debug() const;                    
    bool prev_check_key(SDL_Scancode key) const; // Returns the boolean value of a key from PrevKeyMap
    void update_graph(SDL_Scancode key);
    void render_graph(SDL_Renderer* rend); 
    bool has_quit() const;
    void handle_input();
};

bool check_key(const InputManager& manager, SDL_Scancode key);
bool check_key_pressed(const InputManager& manager, SDL_Scancode key);
bool check_key_release(const InputManager& manager, SDL_Scancode key);

#endif