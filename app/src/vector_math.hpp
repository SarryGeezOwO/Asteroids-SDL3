#ifndef VECTOR_MATH_HPP
#define VECTOR_MATH_HPP

#include <SDL3/SDL.h>

struct Vec2 {
    float x, y;
    Vec2() { x = 0; y = 0; }
    Vec2(float _x, float _y) {
        x = _x;
        y = _y;
    }

    Vec2 operator+(const Vec2 other) const {
        return {
            x + other.x, 
            y + other.y
        };
    }

    Vec2 operator-(const Vec2 other) const {
        return {
            x - other.x, 
            y - other.y
        };
    }

    Vec2 operator!() const {
        return {x * -1, y * -1};
    }

    Vec2 operator*(float scalar) const {
        return {x*scalar, y*scalar};
    }

    Vec2& operator+=(const Vec2& other) {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    bool operator!=(const Vec2& other) {
        return (x != other.x) && (y != other.y);
    }

    bool operator==(const Vec2& other) {
        return (x == other.x) && (y == other.y);
    }
};

struct Util {
    static float rad_to_deg(float rad) {
        return rad * (180 / SDL_PI_F);
    }

    static float deg_to_rad(float deg) {
        return deg * (SDL_PI_F / 180);
    }

    static float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    static float distance(const Vec2& a, const Vec2& b) {
        return SDL_sqrtf(
            SDL_powf((b.x - a.x), 2) +
            SDL_powf((b.y - a.y), 2)
        );
    }
};
#endif