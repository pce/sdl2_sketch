#include "shapes.h"
#include <cmath>

namespace Shapes {

Camera camera = {{0, 0}};  // Initializing camera offset

void set_camera_offset(int x, int y) {
    camera.offset.x = x;
    camera.offset.y = y;
}


void draw_circle(SDL_Renderer *renderer, Circle circle)
{
  SDL_SetRenderDrawColor(renderer, circle.color.r, circle.color.g, circle.color.b, circle.color.a);

  const int32_t diameter = (circle.radius * 2);
  int32_t x = (circle.radius - 1);
  int32_t y = 0;
  int32_t tx = 1;
  int32_t ty = 1;
  int32_t error = (tx - diameter);

  while (x >= y)
  {
    // draw each octant
    SDL_RenderDrawPoint(renderer, circle.center.x + x - camera.offset.x, circle.center.y - y - camera.offset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x + x - camera.offset.x, circle.center.y + y - camera.offset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x - x - camera.offset.x, circle.center.y - y - camera.offset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x - x - camera.offset.x, circle.center.y + y - camera.offset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x + y - camera.offset.x, circle.center.y - x - camera.offset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x + y - camera.offset.x, circle.center.y + x - camera.offset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x - y - camera.offset.x, circle.center.y - x - camera.offset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x - y - camera.offset.x, circle.center.y + x - camera.offset.y);

    if (error <= 0)
    {
      ++y;
      error += ty;
      ty += 2;
    }

    if (error > 0)
    {
      --x;
      tx += 2;
      error += (tx - diameter);
    }
  }
}

void draw_filled_circle_simple(SDL_Renderer *renderer, Circle circle)
{
  SDL_SetRenderDrawColor(renderer, circle.color.r, circle.color.g, circle.color.b, circle.color.a);

  const int32_t diameter = (circle.radius * 2);

  for (int w = 0; w < diameter; w++)
  {
    for (int h = 0; h < diameter; h++)
    {
      int32_t dx = circle.radius - w; // horizontal offset
      int32_t dy = circle.radius - h; // vertical offset
      if ((dx * dx + dy * dy) <= (circle.radius * circle.radius))
      {
        SDL_RenderDrawPoint(renderer, circle.center.x + dx, circle.center.y + dy);
      }
    }
  }
}

void draw_filled_circle(SDL_Renderer *renderer, Circle circle)
{
    SDL_SetRenderDrawColor(renderer, circle.color.r, circle.color.g, circle.color.b, circle.color.a);

    int32_t x0 = circle.center.x - camera.offset.x;
    int32_t y0 = circle.center.y - camera.offset.y;
    int32_t radius = circle.radius;

    int32_t x = radius - 1;
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = tx - (radius << 1); // shifting left by 1 is the same as multiplying by 2

    while (x >= y)
    {
        // Draw horizontal lines from the left to the right of the circle
        SDL_RenderDrawLine(renderer, x0 - x, y0 + y, x0 + x, y0 + y);
        SDL_RenderDrawLine(renderer, x0 - y, y0 + x, x0 + y, y0 + x);
        SDL_RenderDrawLine(renderer, x0 - x, y0 - y, x0 + x, y0 - y);
        SDL_RenderDrawLine(renderer, x0 - y, y0 - x, x0 + y, y0 - x);

        if (error <= 0)
        {
            y++;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            x--;
            tx += 2;
            error += (tx - (radius << 1));
        }
    }
}


void draw_rect(SDL_Renderer *renderer, Rect rect) {
    SDL_SetRenderDrawColor(renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a);

    SDL_Rect sdlRect = { rect.rect.x - camera.offset.x, rect.rect.y - camera.offset.y, rect.rect.w, rect.rect.h };
    SDL_RenderDrawRect(renderer, &sdlRect);
}

void draw_filled_rect(SDL_Renderer *renderer, Rect rect) {
    SDL_SetRenderDrawColor(renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a);

    SDL_Rect sdlRect = { rect.rect.x - camera.offset.x, rect.rect.y - camera.offset.y, rect.rect.w, rect.rect.h };
    SDL_RenderFillRect(renderer, &sdlRect);
}


void draw_triangle(SDL_Renderer *renderer, Triangle triangle) {
    SDL_SetRenderDrawColor(renderer, triangle.color.r, triangle.color.g, triangle.color.b, triangle.color.a);

    // Draw lines between the triangle's points
    SDL_RenderDrawLine(renderer, triangle.p1.x, triangle.p1.y, triangle.p2.x, triangle.p2.y);
    SDL_RenderDrawLine(renderer, triangle.p2.x, triangle.p2.y, triangle.p3.x, triangle.p3.y);
    SDL_RenderDrawLine(renderer, triangle.p3.x, triangle.p3.y, triangle.p1.x, triangle.p1.y);
}


void draw_filled_triangle(SDL_Renderer *renderer, Triangle triangle) {
    SDL_SetRenderDrawColor(renderer, triangle.color.r, triangle.color.g, triangle.color.b, triangle.color.a);

    // Find the bounding box of the triangle
    int minX = std::min({triangle.p1.x, triangle.p2.x, triangle.p3.x});
    int maxX = std::max({triangle.p1.x, triangle.p2.x, triangle.p3.x});
    int minY = std::min({triangle.p1.y, triangle.p2.y, triangle.p3.y});
    int maxY = std::max({triangle.p1.y, triangle.p2.y, triangle.p3.y});

    for (int y = minY; y <= maxY; y++) {
        std::vector<int> nodeX;
        
        // Find intersections of the triangle with the current scanline

        // Sort the intersections
        std::sort(nodeX.begin(), nodeX.end());

        // Fill between the intersections
        for (size_t i = 0; i < nodeX.size(); i += 2) {
            if (nodeX[i] > maxX) break;
            if (nodeX[i+1] > minX) {
                if (nodeX[i] < minX) nodeX[i] = minX;
                if (nodeX[i+1] > maxX) nodeX[i+1] = maxX;
                for (int x = nodeX[i]; x < nodeX[i+1]; x++) {
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
    }
}


} // namespace Shapes
