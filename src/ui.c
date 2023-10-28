#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drawer.h>
#include <ui.h>
#include <player.h>
#include <camera.h>
#include <messages.h>
#include <map_defs.h>
#include <inventory.h>
#include <ticks.h>
#include <menu_defs.h>

int x_cursor = 0;
int y_cursor = 0;
int ui_x_scale = 1;
int ui_y_scale = 1;
int cols = 0;
int rows = 0;
int active_menu = 0;
ui_menu_t next_menu;

ui_menu_t game_menus[MENU_AMOUNT] = {
  {&handle_game_ui, &draw_game_ui, 0, GAME_UI_ID, PAUSE_UI_ID},
  {&handle_pause_ui, &draw_pause_ui, 0, PAUSE_UI_ID, GAME_UI_ID},
  {&handle_inventory_ui, &draw_inventory_ui, 0, INVENTORY_UI_ID, GAME_UI_ID},
};

void init_ui(){
  get_camera_view(&cols, &rows);
  cols = cols * 4;
  rows = rows * 2;
  zero_ui();
  ui_x_scale = (DEFAULT_SCREEN_WIDTH / cols);
  ui_y_scale = (DEFAULT_SCREEN_HEIGHT / rows);
}

void zero_ui(){
  x_cursor = 0;
  y_cursor = 0;
}

void newline_ui(){
  x_cursor = 0;
  y_cursor++;
  y_cursor = y_cursor % rows;
}

/* Draw and individual character from the font map */
void draw_letter(char letter, render_obj_t * object){
  object->target.x = x_cursor * ui_x_scale;
  object->target.y = y_cursor * ui_y_scale;

  if (letter > 0){
    object->clip.x = (letter * FONT_WIDTH) % (FONT_WIDTH*FONT_ATLAS_SIZE);
    object->clip.y = ((letter / FONT_ATLAS_SIZE) * FONT_HEIGHT);
  } else {
    object->clip.x = 0;
    object->clip.y = 0;
  }

  x_cursor++;
  x_cursor = x_cursor % cols;
  if (x_cursor == cols-1){
    newline_ui();
  }
    
  SDL_RenderCopy(object->renderer, object->texture, &object->clip, &object->target);
}

void draw_string(char * string, render_obj_t * object){
  object->surface = SDL_LoadBMP(FONT_PATH);
  object->texture = SDL_CreateTextureFromSurface(object->renderer, object->surface);
  /* Scale the font */
  object->target.w = ui_x_scale;
  object->target.h = ui_y_scale;
  object->clip.w = FONT_WIDTH;
  object->clip.h = FONT_HEIGHT;
  for (int i = 0 ; string[i] != 0 ; i++){
    draw_letter(string[i], object);
  }
  SDL_FreeSurface(object->surface);
  SDL_DestroyTexture(object->texture);
}

void draw_game_ui(render_obj_t * object){
  block_data_t block;
  init_ui();
  block = get_current_item()->item;
  draw_string(CURRENT_VERSION_MSG, object);

  newline_ui();
      
  draw_string(CURRENT_BLOCK_MSG, object);
  draw_string(block.name, object);
}

void draw_direction(render_obj_t * object){
  int view_x;
  int view_y;
  int x_off = 0;
  int y_off = 0;
  int render_angle = 0;
  transform_t direction = get_player_direction();
  
  get_camera_view(&view_x, &view_y);
  int width = DEFAULT_SCREEN_WIDTH/view_x;
  int height = DEFAULT_SCREEN_HEIGHT/view_y;
  object->surface = SDL_LoadBMP(ARROW_UI_PATH);

  if (direction.y == -1){
    x_off = width * (view_x/2);
    y_off = height * (view_y/2-1) - (height/4);
    render_angle = 0;
  }
  if (direction.y == 1){
    x_off = width * (view_x/2);
    y_off = height * (view_y/2+1) + (height/4);
    render_angle = 180;
  }
  if (direction.x == -1){
    x_off = width * (view_x/2-1);
    y_off = height * (view_y/2);
    render_angle = 270;
  }
  if (direction.x == 1){
    x_off = width * (view_x/2+1);
    y_off = height * (view_y/2);
    render_angle = 90;
  }

  object->texture = SDL_CreateTextureFromSurface(object->renderer, object->surface);
  object->target.x = x_off;
  object->target.y = y_off;
  object->target.w = DEFAULT_SCREEN_WIDTH/view_x;
  object->target.h = DEFAULT_SCREEN_HEIGHT/view_y;
  SDL_RenderCopyEx(object->renderer, object->texture, NULL, &object->target, render_angle, NULL, 0);
  SDL_DestroyTexture(object->texture);
  SDL_FreeSurface(object->surface);
}

/* Switch between UI Modes */
int handle_game_ui(SDL_Event event){
  handle_block_select(event);
  switch (event.key.keysym.sym){
    case SDLK_e:
      active_menu = INVENTORY_UI_ID;
      return HANDLE_CLOSE;
      
    case SDLK_ESCAPE:
      active_menu = PAUSE_UI_ID;
      return HANDLE_CLOSE;
  }
  return HANDLE_REGULAR;
}

void draw_inventory_ui(render_obj_t * object){
  init_ui();
  block_data_t block;
  char slot_label[4] = " - ";
  char amount[20];
  for (int slot = 1 ; slot < INVENTORY_SIZE+1 ; slot++){
    if (slot == INVENTORY_SIZE){
      slot = 0;
    }
    block = get_inventory_item(slot)->item;
    slot_label[1] = slot + 48;
    draw_string(slot_label, object);
    draw_string(block.name, object);
    sprintf(amount, ": %d", get_inventory_item(slot)->amount);
    draw_string(amount, object);
    
    newline_ui();
    
    if (slot == 0){
      break;
    }
  }
}

int handle_inventory_ui(SDL_Event event){
  switch(event.key.keysym.sym){
    case SDLK_e:
      active_menu = GAME_UI_ID;
      return HANDLE_CLOSE;
  }
  return HANDLE_REGULAR;
}

void handle_block_select(SDL_Event event){
  if (get_current_menu_id() != GAME_UI_ID){
    return;
  }
  char code = event.key.keysym.sym-48;
  if (code < 0 || code > 9){
    return;
  }
  if (code == 0){
    set_current_item(9);
  } else {
    set_current_item(code);
  }
}

void draw_pause_ui(render_obj_t * object){
  init_ui();
  draw_string("Nocturne Island", object);
  newline_ui();
  draw_string("Press ESC to continue", object);
  newline_ui();
  draw_string("Press Q to exit", object);
  newline_ui();
}

int handle_pause_ui(SDL_Event event){
  switch(event.key.keysym.sym){
    case SDLK_q:
      return HANDLE_EXIT;
    case SDLK_ESCAPE:
      active_menu = GAME_UI_ID;
      return HANDLE_CLOSE;
  }
  return HANDLE_REGULAR;
}

void draw_menu(render_obj_t * object){
  ui_menu_t current_menu;
  get_menu_info(&current_menu, active_menu);
  current_menu.draw(object);
}

void handle_menu(int * exit, SDL_Event event){
  ui_menu_t current_menu;
  get_menu_info(&current_menu, active_menu);
  int result = current_menu.update(event);

  switch(result){
    case HANDLE_EXIT:
      *exit = 0;
      break;
    case HANDLE_CLOSE:
      close_menu(&current_menu);
      get_menu_info(&next_menu, active_menu);
      open_menu(&next_menu);
      break;
    default:
      break;
  }
}

void get_menu_info(ui_menu_t * info, int id){
  ui_menu_t menu = game_menus[id];
  info->update = menu.update;
  info->draw = menu.draw;
  info->active = menu.active;
  info->id = menu.id;
  info->fallback_id = menu.fallback_id;
}

void open_menu(ui_menu_t * menu){
  for (int id = 0 ; id < MENU_AMOUNT ; id++){
    game_menus[id].active = 0;
  }  
  menu->active = 1;
  active_menu = menu->id;
}

void close_menu(ui_menu_t * menu){
  menu->active = 0;
}

int get_current_menu_id(){
  if (game_menus[active_menu].active){
    return active_menu;
  }
  return 0;
}
