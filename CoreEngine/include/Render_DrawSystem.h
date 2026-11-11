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
enum RENDER_BORDER_TYPE:uint8_t{
    RENDER_BORDER_NONE=0,
    RENDER_BORDER_ONLY=1,
    RENDER_BORDER_APPEND=2
};

//Store the Data for Draw a Sprite
struct RenderCommand{
  SDL_Texture* tex_id;
  SDL_Rect rect_world;
  SDL_Rect rect_sprite;
  uint16_t color;
  SDL_Point pivot;
  uint16_t angle;
  int8_t z_index;
  bool use_color;
  bool flipped;
  uint8_t render_border;
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
      void draw_Particles(textureManager_system* testure_sys,Camera2D* cam);
      void draw_UI(textureManager_system* texture_sys,Scene* target_scene);
      SDL_Renderer* get_win_render();
      SDL_Window* get_win();
      void On_WindowResize(uint16_t new_w,uint16_t new_h);
    private:
      void draw_scrollBar(ScrollBar* scroll,textureManager_system* texture_sys,SDL_Rect container_rect,int8_t z_index);
      void draw_UiSpriteComposer(Scene* target_scene,textureManager_system* texture_sys,uint16_t index_entity,int8_t z_index,float centerX,float centerY,SDL_Rect& rect_composer);
      void draw_spriteComposer(Scene* target_scene,textureManager_system* texture_sys,uint16_t index_entity,int8_t z_index,float centerX,float centerY,float angle,float initial_angle,bool flipped);
      bool draw_Valuebar_Sprites(SDL_Rect bg_rect,SDL_Rect bar_rect,textureManager_system* texture_sys,int8_t z_index,ValueBar* bar,UI_System* ui_sys);

      SDL_Texture* draw_text(Fonts_System* font_sys,Label*lbl,SDL_Rect& rect,Bitmap_Font* target_font);
      SDL_Renderer* main_render;
      SDL_Renderer* splash_render;
      SDL_Window* main_win;
      SDL_Window* splash_win;
      SDL_Texture* bg_texture=nullptr;
      std::vector<RenderCommand> render_queue;
      uint8_t redraw;
};



#endif // RENDER_DRAWSYSTEM_H
