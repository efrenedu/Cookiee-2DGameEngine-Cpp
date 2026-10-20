#ifndef TILEMAP_H
#define TILEMAP_H
#include<cstdint>
#include<SDL.h>
#include<vector>

//The data of a Tile Layer
struct TileLayer{
    public:
        uint16_t get_tile(uint32_t column,uint32_t row);
        void set_tile(uint32_t column,uint32_t row,uint16_t sprite_id);
        void set_tile(uint32_t index_tile,uint16_t sprite_id);
        void reserve_memory(uint32_t w,uint32_t h);
        uint32_t get_width();
        uint16_t get_sprite_index(uint32_t tile_index);
        uint16_t get_sprite_index(uint32_t column,uint32_t row);
    private:
      std::vector<uint16_t> tiles;
      uint32_t width;
      uint32_t height;
};


//System for Manage the Data of Tilemap
struct System_TileMap
{
    public:
        void reserve_memory(uint8_t layer_count,uint32_t w_map,uint32_t h_map,uint8_t sz_tiles);
        void render_tile(SDL_Renderer* sdl_render);
        void set_tile(uint8_t layer_index,uint32_t column,uint32_t row,uint16_t sprite_id);
        void set_tile(uint8_t layer_index,uint32_t index_tile,uint16_t sprite_id);
        uint8_t get_tiles_size();
        uint32_t get_width_layer(uint8_t layer_index);
        uint16_t get_sprite_tile(uint8_t layer_index,uint32_t index_tile);
        uint16_t get_sprite_tile(uint8_t layer_index,uint32_t column,uint32_t row);
        uint8_t get_layers_count();
    private:
      std::vector<TileLayer> layers;
      uint8_t tiles_size;
};

#endif // TILEMAP_H
