#ifndef DRAWER_H_
#define DRAWER_H_

#include <SDL2/SDL.h>
#include <map.h>
#include <stdbool.h>
#include <map_defs.h>

#define WALL_PATH "assets/walls.bmp"
#define ATLAS_PATH "assets/3d-atlas.bmp"
#define LEVEE_BODY_PATH "assets/levee.bmp"
#define LEVEE_HEAD_PATH "assets/levee-head.bmp"
#define LEVEE_WIDTH 32
#define LEVEE_HEIGHT 40
#define LEVEE_ATLAS_WIDTH LEVEE_WIDTH * 4
#define LEVEE_ATLAS_HEIGHT LEVEE_HEIGHT

#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define ATLAS_WIDTH TILE_WIDTH * 16
#define ATLAS_HEIGHT TILE_HEIGHT * 16
#define DEFAULT_SCREEN_WIDTH 208 * 4
#define DEFAULT_SCREEN_HEIGHT 144 * 4

typedef struct render_obj_t {
  SDL_Renderer * renderer;
  SDL_Window * window;
  SDL_Surface * surface;
  SDL_Texture * texture;
  SDL_Rect clip;
  SDL_Rect target;
} render_obj_t;

#include <camera.h>

void draw_view(render_obj_t * object);
void draw_player(render_obj_t * object);
#endif
