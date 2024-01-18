#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
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
  SDL_Color color;
};

std::vector<Circle> circles;

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Point center = {.x = CENTER_X, .y = CENTER_Y};
const int radius = RADIUS;

SDL_Point cameraOffset = {.x = 0, .y = 0};


SDL_Color DynamicBlack = {0, 0, 0, 255}; // Black
SDL_Color StructuralWhite = {255, 255, 255, 255}; // White

std::vector<SDL_Color> bauhausColors = {
    {230, 0, 26, 255}, // Bauhaus Red
    {255, 239, 0, 255}, // Bauhaus Yellow
    {0, 35, 149, 255}  // Bauhaus Blue
};


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

    int32_t x0 = circle.center.x - cameraOffset.x;
    int32_t y0 = circle.center.y - cameraOffset.y;
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


void draw_random_lines(SDL_Renderer *renderer, int lineCount) {
    srand(static_cast<unsigned>(time(nullptr))); // Seed the random number generator

    SDL_SetRenderDrawColor(renderer, bauhausColors[0].r, bauhausColors[0].g, bauhausColors[0].b, bauhausColors[0].a);


    for (int i = 0; i < lineCount; ++i) {
        int randomWidth = (rand() % 3) + 1; // Random width between 1 and 3
        int randomAngle = rand() % 360; // Random angle between 0 and 359 degrees
        // Length of the line
        int length = CENTER_Y; 

        // Calculate the line's end point using trigonometry
        int endX = CENTER_X + length * cos(randomAngle * PI / 180.0);
        int endY = CENTER_Y + length * sin(randomAngle * PI / 180.0);

        // Draw the line with the specified random width
        for (int w = 0; w < randomWidth; ++w) {
            SDL_RenderDrawLine(renderer, CENTER_X, CENTER_Y, endX, endY + w);
        }
    }
}

void create_random_circles()
{
  srand(time(nullptr)); // Seed for random number generation
  int numCircles = 10;  // Number of circles to generate

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

void update()
{
  SDL_SetRenderDrawColor(renderer, /* RGBA: black */ 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);

  draw_random_lines(renderer, 10); 

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

  create_random_circles();

  update();
  run_main_loop();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
}
