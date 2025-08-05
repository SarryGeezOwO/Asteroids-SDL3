#include "gamerun.hpp"
#include <queue>
#include <unordered_map>

#define FPS 60
#define WIN_W 1080
#define WIN_H 720
constexpr float MS_PER_FRAME = 1000 / FPS;

// Get
#define VAR(id, name, type)     (ent_mngr().get_var<type>(id, name))
#define VARP(ent, name, type)   (ent_mngr().get_var<type>(ent->instance_id, name))

// Set
#define SVAR(id, name, value)   (ent_mngr().set_var(id, name, value))
#define SVARP(ent, name, value) (ent_mngr().set_var(ent->instance_id, name, value))

static Entity* player;
static std::vector<EntityID> kill_check_list;
static std::queue<EntityID> kill_queue;
static bool debug = false;

struct GameInfo {
    int life;
    int score;
    bool game_over;
};
static GameInfo game_state;
static std::unordered_map<std::string, int> scoreMap = {
    {"big_rock", 300},
    {"med_rock", 200},
    {"small_rock", 100},
    {"life", 200}
};

void add_score(const char* name) {
    game_state.score += scoreMap[name];
}

void show_game_over_screen() {
    // Something UI
}

void Scene_RunGame::wrap_around(Entity* ent) {
    int scrn_pad = 20;
    Vec2 c       = ent->position;
    Vec2 scrn(WIN_W + scrn_pad, WIN_H + scrn_pad); 
    if (c.x < -scrn_pad) ent->position.x += scrn.x; 
    if (c.x > scrn.x)    ent->position.x -= scrn.x;
    
    if (c.y < -scrn_pad) ent->position.y += scrn.y;
    if (c.y > scrn.y)    ent->position.y -= scrn.y;
}

void Scene_RunGame::kill(EntityID id) {
    const auto it = std::find(kill_check_list.begin(), kill_check_list.end(), id);
    if (it != kill_check_list.end()) {
        // Duplicate found
        return;
    }
    kill_check_list.push_back(id);
    kill_queue.push(id);
}

void Scene_RunGame::kill(std::initializer_list<EntityID> ids) {
    for (auto id : ids) {
        kill(id);
    }
}

void Scene_RunGame::kill(Entity* ent) {
    kill(ent->instance_id);
}

void Scene_RunGame::handle_kill_list() {
    while (!kill_queue.empty()) {
        ent_mngr().kill_entity(kill_queue.front());
        kill_queue.pop();
    }
    kill_check_list.clear();
}

std::string Scene_RunGame::get_random_asteroid_sprite(AsteroidType type, SpriteManager& manager) {
    std::vector<std::string> candidates;

    // Get all sprite names from the manager
    for (const auto& spr : manager.get_all_sprite_names()) {
        if (spr.find("rock") != std::string::npos) {

            // Check for sprite substring to determine size
            if (type == BIG && spr.find("big") != std::string::npos)
                candidates.push_back(spr);
            else if (type == MEDIUM && spr.find("med") != std::string::npos)
                candidates.push_back(spr);
            else if (type == SMALL && spr.find("small") != std::string::npos)
                candidates.push_back(spr);
        }
    }
    if (candidates.empty()) return "";

    int idx = SDL_rand(candidates.size());
    return candidates[idx];
}

float Scene_RunGame::random_angle() {
    return SDL_rand(360);
}

Vec2 Scene_RunGame::random_initial_velocity() {
    float angle = random_angle();
    float speed = 1.0f + ((float)SDL_rand(100)) / 100.0f; // Random speed
    Vec2 velocity = {
        SDL_cosf(Util::deg_to_rad(angle)), 
        SDL_sinf(Util::deg_to_rad(angle))
    };
    return velocity * speed;
}

void Scene_RunGame::spawn_asteroid(AsteroidType type) {
    int edge = SDL_rand(4);
    Vec2 pos;
    float padding = 20;
    switch (edge) {
        case 0: pos = {(float)(SDL_rand(WIN_W)), -padding}; break;         // Top
        case 1: pos = {(float)(SDL_rand(WIN_W)), WIN_H + padding}; break;  // Bottom
        case 2: pos = {-padding, (float)(SDL_rand(WIN_H))}; break;         // Left
        case 3: pos = {WIN_W + padding, (float)(SDL_rand(WIN_H))}; break;  // Right
    }

    EntityID id = ent_mngr().spawn_entity(get_random_asteroid_sprite(type, sprite_mngr()), pos);
    ent_mngr().get_entity_by_id(id)->rotation = random_angle();
    SVAR(id, "velocity", random_initial_velocity());
    SVAR(id, "size_type", type);
}

void Scene_RunGame::spawn_asteroid(AsteroidType type, Vec2 initial_pos) {
    EntityID id = ent_mngr().spawn_entity(get_random_asteroid_sprite(type, sprite_mngr()), initial_pos);
    ent_mngr().get_entity_by_id(id)->rotation = random_angle();
    SVAR(id, "velocity", random_initial_velocity());
    SVAR(id, "size_type", type);
}

void Scene_RunGame::destroy_asteroid(EntityID id) {
    // Check if this Instance is a "valid" asteroid rock
    if (!ent_mngr().has_var(id, "size_type")) {
        SDL_Log(">! Instance is not an asteroid type.");
        return;
    }
    
    kill(id);
    audio_mngr().reset("short_explosion");
    audio_mngr().play("short_explosion");
    AsteroidType type = VAR(id, "size_type", AsteroidType);
    AsteroidType spawnType;
    int spawnCount = 2;

    switch (type)
    {
        case BIG:    
            add_score("big_rock");
            spawnType = MEDIUM; 
            break;

        case MEDIUM: 
            add_score("med_rock");
            spawnType = SMALL;
            break;

        case SMALL:
            add_score("small_rock");
            return;
    }

    for (int i = 0; i < spawnCount; i++) {
        int decx  = (bool)SDL_rand(2) ? 1 : -1;
        int decy  = (bool)SDL_rand(2) ? 1 : -1;
        float rx  = (float)SDL_rand(30) * decx;
        float ry  = (float)SDL_rand(30) * decy;
        Vec2  pos = ent_mngr().get_entity_by_id(id)->position; 
        spawn_asteroid(spawnType, Vec2( pos.x + rx, pos.y + ry));
    }
}

// Start of the application before game loop
void Scene_RunGame::start() {
    // Initial Game state
    game_state.life = 3;
    game_state.game_over = false;
    
    player = ent_mngr().spawn_entity_p("ship", {WIN_W/2, WIN_H/2});
    SVARP(player, "velocity",   Vec2());
    SVARP(player, "direction",  Vec2());
    SVARP(player, "rot_delta",  2.5f);
    SVARP(player, "move_spd",   1.0f);
    SVARP(player, "firerate",   300.0f);
    SVARP(player, "fire_timer", 0.0f);
    SVARP(player, "bullet_spd", 15.0f);
    SVARP(player, "effects",    false);
    SVARP(player, "fx_id",      (EntityID)-1);

    spawn_asteroid(BIG);
    spawn_asteroid(BIG);
    spawn_asteroid(BIG);
    spawn_asteroid(MEDIUM);
    spawn_asteroid(SMALL);
}

void Scene_RunGame::update() {
    if (check_key_pressed(input_mngr(), SDL_SCANCODE_P)) {
        debug = !debug;
        input_mngr().debug_mode(debug);
        ent_mngr().debug_mode(debug);
    }

    bool left   = check_key(input_mngr(), SDL_SCANCODE_A);
    bool right  = check_key(input_mngr(), SDL_SCANCODE_D);
    bool forwrd = check_key(input_mngr(), SDL_SCANCODE_W);
    bool shoot  = check_key(input_mngr(), SDL_SCANCODE_SPACE);
    player->rotation += (right - left) * VARP(player, "rot_delta", float);

    // Moving and Turning
    float rad = Util::deg_to_rad(player->rotation - 90);
    Vec2 direction = { SDL_cosf(rad), SDL_sinf(rad) };
    Vec2 velocity = VARP(player, "velocity", Vec2);
    float moveSpd = VARP(player, "move_spd", float);

    // Apply thrust only when forward is pressed
    if (forwrd && !game_state.game_over) {
        velocity.x += direction.x * moveSpd * 0.1f; 
        velocity.y += direction.y * moveSpd * 0.1f;
        audio_mngr().play("engine_rumble");
    }
    else {
        audio_mngr().pause("engine_rumble");
    }

    // Apply friction/damping
    velocity.x = Util::lerp(velocity.x, 0.0f, 0.02f);
    velocity.y = Util::lerp(velocity.y, 0.0f, 0.02f);

    SVARP(player, "velocity", velocity);
    player->position += velocity;
    SVARP(player, "direction", direction);

    // Shooting
    float fireTimer = VARP(player, "fire_timer", float);
    if (fireTimer > 0) {
        fireTimer -= MS_PER_FRAME;
    }
    // Shoot ---
    if (shoot && fireTimer <= 0 && !game_state.game_over) {
        fireTimer = VARP(player, "firerate", float);
        EntityID bullet = ent_mngr().spawn_entity("bullet", player->position);
        Entity* ent     = ent_mngr().get_entity_by_id(bullet);
        ent->rotation   = player->rotation;
        audio_mngr().play("laser_shoot");
    }
    SVARP(player, "fire_timer", fireTimer);


    // Entity loop --------------------------
    for (EntityID id : ent_mngr().get_entities()) {
        Entity* ent = ent_mngr().get_entity_by_id(id);
        std::string name = ent->sprite->sprite_name;

        // Making bullets fly
        if (name == "bullet") {
            float bulletRad = Util::deg_to_rad(ent->rotation - 90);
            Vec2 bulletDir = { SDL_cosf(bulletRad), SDL_sinf(bulletRad) };  
            ent->position += bulletDir * VARP(player, "bullet_spd", float);            
            Vec2 pos = ent->position;

            // Destroy this bullet upong exiting the screen
            int padding = 100;
            if (
                (pos.x < -padding || pos.x > WIN_W + padding) ||
                (pos.y < -padding || pos.y > WIN_H + padding)
            ) {
                kill(id);
            }
        }

        // Teleportnig objects to the other side except bullets
        if (name != "bullet") {
            wrap_around(ent);
        }

        // Asteroid (moving and collision)
        if (name.find("rock") != std::string::npos) {

            // Moving Asteroids by their velocity
            ent->position += VAR(id, "velocity", Vec2);

            // Well rocks are supposed to be equal in both axis
            float sx  = ent->sprite->w * ent->scale.x;
            float mercy = 8;  // <- adjust by difficulty or some shit, lower = harder
            for (EntityID other_id : ent_mngr().get_entities()) {
                Entity*     other       = ent_mngr().get_entity_by_id(other_id);
                std::string other_name  = other->sprite->sprite_name;
            
                // Only check for bullets
                if (other_name == "bullet") {
                    // Destroy both the asteroid and bullet on collision
                    if (Util::distance(ent->position, other->position) <= (sx/2) + mercy) {
                        destroy_asteroid(id);   
                        kill(other_id);
                        break;
                    }
                }

                // Check for player collision
                if (other_name == "ship" && !game_state.game_over) {
                    // Half of asteroid size and player half size
                    float distCheck = sx/2 + (player->sprite->w * player->scale.x)/2;
                    if (Util::distance(ent->position, other->position) <= distCheck) {
                        game_state.life--;
                        player->position.x = WIN_W/2;
                        player->position.y = WIN_H/2;
                        audio_mngr().reset("short_explosion");
                        audio_mngr().play("short_explosion");
                    }
                }
            }
        }

        // Move Fire Effects with the player position
        if (name == "fire") {
            Vec2 fx_dir = VARP(player, "direction", Vec2) * -1;
            ent->position = player->position + (fx_dir * ((player->sprite->w * player->scale.x) - 6));
            ent->rotation = player->rotation;
        }
    }
    
    // Fire effects for player
    if (!game_state.game_over) {
        bool prev_fx = VARP(player, "effects", bool);
        SVARP(player, "effects", forwrd);

        // spawn if not yet
        if (!prev_fx && forwrd) {
            EntityID fx_id = ent_mngr().spawn_entity("fire", Vec2());
            Entity* fx_ent = ent_mngr().get_entity_by_id(fx_id);
            
            // Let's just assume "fire" sprite is scale 1 as always o(*^＠^*)o
            Vec2 fx_dir = VARP(player, "direction", Vec2) * -1;
            fx_ent->position = player->position + (fx_dir * ((player->sprite->w * player->scale.x) - 6));
            fx_ent->rotation = player->rotation;
            SVARP(player, "fx_id", fx_id);
        }

        // Kill this fire fx
        if (prev_fx && !forwrd) {
            kill(VARP(player, "fx_id", EntityID));
        }
    }

    if (game_state.life <= 0) {
        game_state.game_over = true;
        player->set_render(false);
        kill(VARP(player, "fx_id", EntityID));
        SVARP(player, "effects", false);
    }
    handle_kill_list();
}

void Scene_RunGame::render(SDL_Renderer* rend) {
    // Render Life as sprite
    for (int i = 0; i < game_state.life; i++) {
        sprite_mngr().render_sprite(rend, (int)(15 + (20 * i)), (int)15, "life");
    }
}

std::string Scene_RunGame::name() {
    return "RunGame";
}

Scene_RunGame::~Scene_RunGame() {
    // TODO: Cleanup if needed
}

