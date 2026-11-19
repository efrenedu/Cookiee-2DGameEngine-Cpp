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

//Commands for Draw Lines
struct RenderLineCommand{
   uint16_t line_color;
   float x1;
   float x2;
   float y1;
   float y2;
   float thick;

};

//Commands for Draw Sprites or Rects
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


//User Request for Render
struct RenderRequest{
   RenderCommand cmd;
   Dinamic_Text* dinamic_text_ptr;
   float desplz_x;
   float desplz_y;
   float last_y;
   float last_x;
   float time_draw;
   float count_time;

};

//System for Draw  On the SDL Main Windows
struct Render_DrawSystem
{
    public:
      bool Init_System();
      bool Init_Main_Windows();
      void Free_System();
      void request_redraw(uint8_t redraw_type);
      void draw_scene(Scene* target_scene);
      void draw_entities(textureManager_system* texture_sys,uint16_t num_entities,Scene* target_scene,Camera2D* cam,bool draw_colliders_borders=false);
      void draw_tilemap(textureManager_system* texture_sys,System_TileMap* tilemap,Camera2D* cam);
      void draw_Particles(textureManager_system* testure_sys,Camera2D* cam);
      void draw_UI(textureManager_system* texture_sys,Scene* target_scene);
      void update_requests(float dt);
      SDL_Renderer* get_win_render();
      SDL_Window* get_win();
      void On_WindowResize(uint16_t new_w,uint16_t new_h);
      void request_text(Scene* target_scene,std::string text,uint16_t px,uint16_t py,uint8_t font_id,uint16_t color,uint8_t font_style=FONT_VARIANT_NORMAL,uint8_t align=LABEL_ALIGN_LEFT,float life_time=0.0,float desplz_x=0,float desplz_y=0);
      void request_icon(Scene* target_scene,uint16_t sprite_id,uint16_t px,uint16_t py,float life_time=0.0,float desplz_x=0,float desplz_y=0);
      void request_line(Scene* target_scene,uint16_t color,float x1,float x2,float y1,float y2,uint16_t thick=1);
      void request_ValueBar(Scene* target_scene,uint16_t bg_color,uint16_t bar_color,uint16_t bar_w,uint16_t bar_h,float val,uint16_t px,uint16_t py,uint8_t bar_orientation=UI_ORIENTATION_HORIZONTAL,float life_time=0.0f,float desplz_x=0,float desplz_y=0);
      void request_rectangle(Scene* target_scene,uint16_t color,bool only_border,uint16_t px,uint16_t py,uint16_t w,uint16_t h,float life_time=0.0f);
    private:
      void draw_scrollBar(ScrollBar* scroll,textureManager_system* texture_sys,SDL_Rect container_rect,int8_t z_index);
      void draw_UiSpriteComposer(Scene* target_scene,textureManager_system* texture_sys,uint16_t index_entity,int8_t z_index,float centerX,float centerY,SDL_Rect& rect_composer);
      void draw_spriteComposer(Scene* target_scene,textureManager_system* texture_sys,uint16_t index_entity,int8_t z_index,float centerX,float centerY,float angle,float initial_angle,bool flipped);
      bool draw_Valuebar_Sprites(SDL_Rect bg_rect,SDL_Rect bar_rect,textureManager_system* texture_sys,int8_t z_index,ValueBar* bar,UI_System* ui_sys);
      void draw_extrem_ValueBar(SDL_Texture* tex,SDL_Rect world_rect,SDL_Rect sprite_rect,int8_t z_index);

      SDL_Texture* draw_text(Fonts_System* font_sys,Label*lbl,SDL_Rect& rect,Bitmap_Font* target_font);
      SDL_Renderer* main_render;
      SDL_Renderer* splash_render;
      SDL_Window* main_win;
      SDL_Window* splash_win;
      SDL_Texture* bg_texture=nullptr;
      std::vector<RenderCommand> render_queue;
      std::vector<RenderRequest> user_UI_requests;
      std::vector<RenderLineCommand> lines_requests;
      uint8_t redraw;
};



#endif // RENDER_DRAWSYSTEM_H
