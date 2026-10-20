#ifndef RENDER_DRAWSYSTEM_H
#define RENDER_DRAWSYSTEM_H
#include<vector>
#include<SDL.h>
#include<SDL_image.h>
#include<iostream>
#include<unordered_map>
#include<Scene.h>
#include<Game_Config.h>

//Indicate the Type the Draw Requerid in Each Frame
enum REDRAW_TYPES:uint8_t{
   REDRAW_ALL=0,
   REDRAW_ENTITIES=1,
   REDRAW_NONE=2,
};

//Store the Data for Draw a Sprite
struct RenderCommand{
  SDL_Texture* tex_id;
  SDL_Rect rect_world;
  SDL_Rect rect_sprite;
  SDL_Point pivot;
  uint16_t angle;
  uint8_t z_index;
  bool flipped;
  bool render_ColliderborderOnly;
};

//System for Draw  On the SDL Main Windows
struct Render_DrawSystem
{
    public:
      bool Init_System();
      bool Init_Main_Windows(std::unordered_map<std::string,std::string>config);
      void Free_System();
      void request_redraw(uint8_t redraw_type);
      void draw_scene(Scene* target_scene);
      void draw_entities(textureManager_system* texture_sys,uint16_t num_entities,Scene* target_scene,Camera2D* cam,bool draw_colliders_borders=false);
      void draw_tilemap(textureManager_system* texture_sys,System_TileMap* tilemap,Camera2D* cam);
      void draw_UI(textureManager_system* texture_sys,Scene* target_scene,uint16_t ui_id);
      SDL_Renderer* get_win_render();
      SDL_Window* get_win();
      void On_WindowResize(uint16_t new_w,uint16_t new_h);

    private:
      SDL_Renderer* main_render;
      SDL_Renderer* splash_render;
      SDL_Window* main_win;
      SDL_Window* splash_win;
      SDL_Texture* bg_texture=nullptr;
      std::vector<RenderCommand> render_queue;
      uint8_t redraw;
};



#endif // RENDER_DRAWSYSTEM_H
