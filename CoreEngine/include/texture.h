#ifndef TEXTURE_H
#define TEXTURE_H
#include<SDL.h>
#include<SDL_image.h>
#include<cstdint>
#include<vector>
#include<string>

//The data of a Sprite
struct Sprite_Source{
   public:
      uint8_t texture_index;
      uint16_t x;
      uint16_t y;
      uint16_t w;
      uint16_t h;
};

//Store the List of Sprites of Scene
struct Sprite_Catalog{
   public:
       std::vector<Sprite_Source> list_sprites;
       void reserve_memory(uint16_t num_sprites);
       void free_memory();

};

//The Data of a Texture
struct Texture{
  public:
      bool load_fromTileset(Sprite_Catalog& catalog, uint8_t tex_id,const char* src,SDL_Renderer* render_win,uint16_t firstgid,std::string& image_src);
      void set_sprites(Sprite_Catalog& catalog,uint8_t tex_id,uint16_t w_spr,uint16_t h_spr,uint8_t margin,uint8_t spacing,uint16_t columns,uint16_t spr_count);
      void free_memory();
      bool is_loaded();
      SDL_Texture* get_texture();
      uint16_t getId_fromGrid(uint16_t row,uint16_t column,uint16_t& column_number);
  private:
      SDL_Texture * tex;
      bool loaded;
      uint16_t width;
      uint16_t height;
      uint16_t firstid;
      uint16_t lastid;
      uint16_t column_count;

};

//System for Manage the Textures and Sprites of Scenes
struct textureManager_system{
  public:
    void add_fromTileset(const char* src,SDL_Renderer* render_win,uint16_t firstgid);
    void free_memory();
    void reserve_memory(uint8_t number_textures);
    SDL_Texture* get_texture(uint8_t tex_id);
    SDL_Texture* get_texture_fromSprite(SDL_Rect& rect_spr,uint16_t sprite_id);
    uint8_t get_textureId_fromNameSource(const char* src);
    uint16_t get_spriteby_grid(uint8_t text_id,uint16_t row,uint16_t column,uint16_t& number_columns);

  private:
    std::vector<Texture> list_textures;
    std::vector<uint32_t> hash_textures;
    Sprite_Catalog catalog;
};
#endif // TEXTURE_H
