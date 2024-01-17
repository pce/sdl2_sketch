#include <cmath>
#include <vector>
#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CENTER_X = WINDOW_WIDTH / 2;
const int CENTER_Y = WINDOW_HEIGHT / 2;
const int RADIUS = 100;
const float PI = 3.14159265358979323846f;
const int CAM_SPEED = 10;

struct Circle
{
  SDL_Point center;
  int radius;
};

std::vector<Circle> circles;

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Point center = {.x = CENTER_X, .y = CENTER_Y};
const int radius = RADIUS;

SDL_Point cameraOffset = {.x = 0, .y = 0};

void draw_circle(SDL_Renderer *renderer, Circle circle)
{

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  const int32_t diameter = (circle.radius * 2);
  int32_t x = (circle.radius - 1);
  int32_t y = 0;
  int32_t tx = 1;
  int32_t ty = 1;
  int32_t error = (tx - diameter);

  while (x >= y)
  {
    // draw each octant
    SDL_RenderDrawPoint(renderer, circle.center.x + x - cameraOffset.x, circle.center.y - y - cameraOffset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x + x - cameraOffset.x, circle.center.y + y - cameraOffset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x - x - cameraOffset.x, circle.center.y - y - cameraOffset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x - x - cameraOffset.x, circle.center.y + y - cameraOffset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x + y - cameraOffset.x, circle.center.y - x - cameraOffset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x + y - cameraOffset.x, circle.center.y + x - cameraOffset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x - y - cameraOffset.x, circle.center.y - x - cameraOffset.y);
    SDL_RenderDrawPoint(renderer, circle.center.x - y - cameraOffset.x, circle.center.y + x - cameraOffset.y);

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

void draw_flower_at_position(int x, int y)
{
  for (int i = 0; i < 5; ++i)
  {
    float angle = (2 * PI / 5) * i;
    int center_x = x + cos(angle) * radius;
    int center_y = y + sin(angle) * radius;

    Circle newCircle = {{static_cast<int>(center_x), static_cast<int>(center_y)}, radius};
    circles.push_back(newCircle);
  }
}

void apply_rules()
{
  circles.clear();

  // initial position of the first flower
  int initial_x = CENTER_X;
  int initial_y = CENTER_Y;

  draw_flower_at_position(initial_x, initial_y);

  // determine the offset for placing additional flowers
  int offset_x = 3 * radius;
  int offset_y = sqrt(3) * radius;

  // additional flowers to the right
  for (int i = 1; i <= 2; ++i)
  {
    draw_flower_at_position(initial_x + i * offset_x, initial_y);
  }

  //  additional flowers to the upper left and lower left
  draw_flower_at_position(initial_x - offset_x, initial_y - offset_y);
  draw_flower_at_position(initial_x - offset_x, initial_y + offset_y);
}

void update()
{
  SDL_SetRenderDrawColor(renderer, /* RGBA: black */ 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);

  for (const auto &circle : circles)
  {
    draw_circle(renderer, circle);
  }
  SDL_RenderPresent(renderer);
}

uint32_t ticksForNextKeyDown = 0;

bool handle_events()
{
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

  apply_rules();

  update();
  run_main_loop();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
}
