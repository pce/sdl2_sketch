#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "shapes.h"

using namespace Shapes;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CENTER_X = WINDOW_WIDTH / 2;
const int CENTER_Y = WINDOW_HEIGHT / 2;
const int RADIUS = 100;
const float PI = 3.14159265358979323846f;
const int CAM_SPEED = 10;

std::vector<Circle> circles;

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Point center = {.x = CENTER_X, .y = CENTER_Y};
const int radius = RADIUS;

// SDL_Point cameraOffset = {.x = 0, .y = 0};

SDL_Color DynamicBlack = {0, 0, 0, 255};          // Black
SDL_Color StructuralWhite = {255, 255, 255, 255}; // White

std::vector<SDL_Color> bauhausColors = {
    {230, 0, 26, 255},  // Bauhaus Red
    {255, 239, 0, 255}, // Bauhaus Yellow
    {0, 35, 149, 255}   // Bauhaus Blue
};

void draw_random_lines_only(SDL_Renderer *renderer, int lineCount)
{
  srand(static_cast<unsigned>(time(nullptr))); // Seed the random number generator

  SDL_SetRenderDrawColor(renderer, bauhausColors[0].r, bauhausColors[0].g, bauhausColors[0].b, bauhausColors[0].a);

  for (int i = 0; i < lineCount; ++i)
  {
    int randomWidth = (rand() % 3) + 1; // Random width between 1 and 3
    int randomAngle = rand() % 360;     // Random angle between 0 and 359 degrees
    // Length of the line
    int length = CENTER_Y;

    // Calculate the line's end point using trigonometry
    int endX = CENTER_X + length * cos(randomAngle * PI / 180.0);
    int endY = CENTER_Y + length * sin(randomAngle * PI / 180.0);

    // Draw the line with the specified random width
    for (int w = 0; w < randomWidth; ++w)
    {
      SDL_RenderDrawLine(renderer, CENTER_X, CENTER_Y, endX, endY + w);
    }
  }
}

void draw_rotated_rect(SDL_Renderer *renderer, Rect rect, int angle)
{
  double radAngle = angle * PI / 180.0;
  int cx = rect.rect.x + rect.rect.w / 2; // Center x of rectangle
  int cy = rect.rect.y + rect.rect.h / 2; // Center y of rectangle

  // Corners of the rectangle
  SDL_Point corners[4] = {
      {rect.rect.x, rect.rect.y},
      {rect.rect.x + rect.rect.w, rect.rect.y},
      {rect.rect.x + rect.rect.w, rect.rect.y + rect.rect.h},
      {rect.rect.x, rect.rect.y + rect.rect.h}};

  // Rotate each corner
  for (int i = 0; i < 4; ++i)
  {
    int dx = corners[i].x - cx;
    int dy = corners[i].y - cy;

    corners[i].x = cx + dx * cos(radAngle) - dy * sin(radAngle);
    corners[i].y = cy + dx * sin(radAngle) + dy * cos(radAngle);
  }

  // Draw the rectangle using lines between rotated corners
  SDL_SetRenderDrawColor(renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a);
  for (int i = 0; i < 4; ++i)
  {
    SDL_RenderDrawLine(renderer, corners[i].x, corners[i].y, corners[(i + 1) % 4].x, corners[(i + 1) % 4].y);
  }
}

void draw_random_lines(SDL_Renderer *renderer, int lineCount)
{
  srand(static_cast<unsigned>(time(nullptr))); // Seed the random number generator

  for (int i = 0; i < lineCount; ++i)
  {
    SDL_SetRenderDrawColor(renderer, bauhausColors[i % bauhausColors.size()].r,
                           bauhausColors[i % bauhausColors.size()].g,
                           bauhausColors[i % bauhausColors.size()].b,
                           bauhausColors[i % bauhausColors.size()].a);

    int randomWidth = (rand() % 3) + 1; // Random width between 1 and 3
    int randomAngle = rand() % 360;     // Random angle between 0 and 359 degrees
    int length = CENTER_Y;

    int endX = CENTER_X + length * cos(randomAngle * PI / 180.0);
    int endY = CENTER_Y + length * sin(randomAngle * PI / 180.0);

    // Draw the line
    for (int w = 0; w < randomWidth; ++w)
    {
      SDL_RenderDrawLine(renderer, CENTER_X, CENTER_X, endX, endY + w);
    }

    // Every two lines, add a third line at 90 degrees
    if (i % 2 == 1)
    {
      int thirdLineEndX = endX + length * cos((randomAngle + 90) * PI / 180.0);
      int thirdLineEndY = endX + length * sin((randomAngle + 90) * PI / 180.0);

      SDL_RenderDrawLine(renderer, endX, endY, thirdLineEndX, thirdLineEndY);

      // Draw a rectangle at the end of the third line
      // Rect rectangle;
      // rectangle.rect = {thirdLineEndX, thirdLineEndY, rand() % 50 + 20, rand() % 50 + 20};
      // rectangle.color = bauhausColors[(i + 1) % bauhausColors.size()];
      // draw_rect(renderer, rectangle);

      // Draw a rotated rectangle at the end of the third line
      Rect rectangle;
      rectangle.rect = {thirdLineEndX - 25, thirdLineEndY - 25, rand() % 50 + 20, rand() % 50 + 20}; // Centered on third line's end
      rectangle.color = bauhausColors[(i + 1) % bauhausColors.size()];
      // draw_rotated_rect(renderer, rectangle, randomAngle + 90);
      draw_rotated_rect(renderer, rectangle, (rand() % 1) ? randomAngle : randomAngle + 90);
    }

    // Every fifth line, add a triangle that matches the sixth line
    // if (i % 5 == 4)
    // {
    //   Triangle triangle;
    //   triangle.p1 = {endX, endY};
    //   triangle.p2 = {endX + 50, endY}; 
    //   triangle.p3 = {endX, endY + 50};
    //   triangle.color = bauhausColors[(i + 1) % bauhausColors.size()];
    //   draw_filled_triangle(renderer, triangle);
    // }
  }
}

void create_random_circles()
{
  srand(time(nullptr)); // Seed for random number generation
  int numCircles = (rand() % 3) + 3;   // Number of circles to generate

  for (int i = 0; i < numCircles; ++i)
  {
    Circle circle;
    circle.center.x = rand() % WINDOW_WIDTH;
    circle.center.y = rand() % WINDOW_HEIGHT;
    circle.radius = (rand() % 50) + 25; // Random radius between 25 and 75

    // Random color
    // circle.color.r = rand() % 256;
    // circle.color.g = rand() % 256;
    // circle.color.b = rand() % 256;
    // circle.color.a = 255; // Full opacity

    // Randomly select one of the Bauhaus colors
    int colorIndex = rand() % bauhausColors.size(); // Random index for color
    circle.color = bauhausColors[colorIndex];

    circles.push_back(circle);
  }
}

void setup()
{
  Shapes::set_camera_offset(0, 0);
  create_random_circles();
}

void update()
{
  SDL_SetRenderDrawColor(renderer, /* RGBA: black */ 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);

  draw_random_lines(renderer,  (rand() % 5) + 3);

  for (const auto &circle : circles)
  {
    if (rand() % 2)
    {
      draw_filled_circle(renderer, circle);
    }
    else
    {
      draw_circle(renderer, circle);
    }
  }

  SDL_RenderPresent(renderer);
}

uint32_t ticksForNextKeyDown = 0;

bool handle_events()
{
  static SDL_Point cameraOffset = {0, 0};
  SDL_Event event;
  SDL_PollEvent(&event);
  if (event.type == SDL_QUIT)
  {
    return false;
  }
  if (event.type == SDL_KEYDOWN)
  {
    uint32_t ticksNow = SDL_GetTicks();
    if (SDL_TICKS_PASSED(ticksNow, ticksForNextKeyDown))
    {
      // Throttle keydown events for 10ms.
      ticksForNextKeyDown = ticksNow + 10;
      switch (event.key.keysym.sym)
      {
      case SDLK_UP:
        cameraOffset.y -= CAM_SPEED;
        break;
      case SDLK_DOWN:
        cameraOffset.y += CAM_SPEED;
        break;
      case SDLK_RIGHT:
        cameraOffset.x += CAM_SPEED;
        break;
      case SDLK_LEFT:
        cameraOffset.x -= CAM_SPEED;
        break;
      }
      Shapes::set_camera_offset(cameraOffset.x, cameraOffset.y);
      update();
    }
  }
  return true;
}

void run_main_loop()
{
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop([]()
                           { handle_events(); },
                           0, true);
#else
  while (handle_events())
    ;
#endif
}

int main()
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

  setup();
  update();
  run_main_loop();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
}
