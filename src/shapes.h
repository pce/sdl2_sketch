#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include <SDL2/SDL.h>

namespace Shapes {

struct Camera {
    SDL_Point offset;
};

struct Circle {
    SDL_Point center;
    int radius;
    SDL_Color color;
};

struct Rect {
    SDL_Rect rect;
    SDL_Color color;
};


struct Triangle {
    SDL_Point p1, p2, p3;
    SDL_Color color;
};

void set_camera_offset(int x, int y);

void draw_circle(SDL_Renderer *renderer, Circle circle);
void draw_filled_circle_simple(SDL_Renderer *renderer, Circle circle);
void draw_filled_circle(SDL_Renderer *renderer, Circle circle);

void draw_rect(SDL_Renderer *renderer, Rect rect);
void draw_filled_rect(SDL_Renderer *renderer, Rect rect);

void draw_triangle(SDL_Renderer *renderer, Triangle triangle);
void draw_filled_triangle(SDL_Renderer *renderer, Triangle triangle);

}

#endif // SHAPES_H
