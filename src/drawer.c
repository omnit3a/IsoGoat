#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <drawer.h>
#include <map.h>
#include <camera.h>
#include <player.h>
#include <map_defs.h>
#include <entity.h>
#include <ticks.h>

int view_x = 0;
int view_y = 0;

void draw_view(render_obj_t * object){
  get_camera_view(&view_x, &view_y);
  object->surface = SDL_LoadBMP(ATLAS_PATH);
  object->texture = SDL_CreateTextureFromSurface(object->renderer, object->surface);
  object->clip.w = TILE_WIDTH;
  object->clip.h = TILE_HEIGHT;
  int screen_x = 0;
  int screen_y = 0;
  transform_t pos = get_player_entity()->position;
  int drawing_height = MAP_HEIGHT;
  
  for (int z = 0 ; z < drawing_height ; z++){
    screen_x = 0;
    for (int x = pos.x-(view_x/2) ; x <= pos.x+(view_x/2) ; x++){
      screen_y = 0;
      for (int y = pos.y-(view_y/2) ; y <= pos.y+(view_y/2) ; y++){

	if (is_block_shaded(pos.x, pos.y, pos.z-1)){
	  drawing_height = pos.z;
	} else {
	  drawing_height = MAP_HEIGHT;
	}
	
	int state = get_block(x, y, z).current_state;
	int block = get_block(x, y, z).block.block[state];

	if (get_block(x, y, z).id > 0){
	  object->clip.x = (block % (ATLAS_WIDTH / TILE_WIDTH)) * TILE_WIDTH;
	  object->clip.y = (block / (ATLAS_HEIGHT / TILE_HEIGHT)) * TILE_HEIGHT;
	} else {
	  object->clip.x = 0;
	  object->clip.y = 0;
	}

	int brightness = (32 * is_daytime())+((z) * 25);
	if (brightness > 255){
	  brightness = 255;
	}

	if (is_block_shaded(x, y, z)){
	  SDL_SetTextureColorMod(object->texture, 64, 64, 64);
	} else {
	  SDL_SetTextureColorMod(object->texture, brightness, brightness, brightness);
	}
	
        object->target.x = (screen_x * DEFAULT_SCREEN_WIDTH/view_x);
        object->target.y = (screen_y * DEFAULT_SCREEN_HEIGHT/view_y);
        object->target.w = DEFAULT_SCREEN_WIDTH/view_x;
        object->target.h = DEFAULT_SCREEN_HEIGHT/view_y;
	SDL_RenderCopy(object->renderer, object->texture, &object->clip, &object->target);
	screen_y++;
      }
      screen_x++;
    }
  }
  SDL_FreeSurface(object->surface);
  SDL_DestroyTexture(object->texture);
}
  
/* Draw player */
void draw_player(render_obj_t * object){
  get_camera_view(&view_x, &view_y);
  object->surface = SDL_LoadBMP(LEVEE_PATH);
  object->texture = SDL_CreateTextureFromSurface(object->renderer, object->surface);

  object->target.x = (DEFAULT_SCREEN_WIDTH/2)-((DEFAULT_SCREEN_WIDTH/view_x)/2);
  object->target.y = (DEFAULT_SCREEN_HEIGHT/2)-((DEFAULT_SCREEN_HEIGHT/view_y)/2);
  object->target.w = DEFAULT_SCREEN_WIDTH/view_x;
  object->target.h = DEFAULT_SCREEN_HEIGHT/view_y;

  SDL_RenderCopy(object->renderer, object->texture, NULL, &object->target);
  SDL_DestroyTexture(object->texture);
  SDL_FreeSurface(object->surface);
}
