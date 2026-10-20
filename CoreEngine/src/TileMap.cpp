#include "TileMap.h"
#include<iostream>

//Functions Of TileLayer


//get the Indicated Tile from the Indicated Column and Row
uint16_t TileLayer::get_tile(uint32_t column ,uint32_t row){
   uint32_t index=(row*width)+column;
   if(index<0 || index>=tiles.size()){
      return 0;
   }
   return tiles[index];

}

//Set Sprite Id of Indicated Tile
void TileLayer::set_tile(uint32_t column,uint32_t row,uint16_t sprite_id){
   uint32_t index=(row*width)+column;
   if(index<0 || index>=tiles.size()){
      return ;
   }
   tiles[index]=sprite_id;
}

//Set the Sprite Id in the Indicated Tile by Id
void TileLayer::set_tile(uint32_t index_tile,uint16_t sprite_id){
  if(index_tile<0 || index_tile>=tiles.size()){
      return ;
  }
  tiles[index_tile]=sprite_id;
}

//Reserve Memory and Assign default Value of Layer Tiles
void TileLayer::reserve_memory(uint32_t w,uint32_t h){
   width=w;
   height=h;
   tiles.assign(w*h,0);
}

//Get the Width of Layer(Number of Columns)
uint32_t TileLayer:: get_width(){
   return width;
}

//Get the Sprite Index of Indicated Tile by Id
uint16_t TileLayer:: get_sprite_index(uint32_t tile_index){
    if(tile_index<0 || tile_index>=tiles.size()){
           return 0;
    }
    return tiles[tile_index];
}

//Get the Sprite Index In the Indicated Column and Row
uint16_t TileLayer:: get_sprite_index(uint32_t column,uint32_t row){

   uint32_t index=(row*width)+column;
   if(index<0 || index>=tiles.size()){
       return 0;
   }
   return tiles[index];
}


//System Tilemap Functions

//Reserve Memory for the System
void System_TileMap::reserve_memory(uint8_t layer_count,uint32_t w_map,uint32_t h_map,uint8_t sz_tiles){

   layers.resize(layer_count);
   for (auto& temp_layer:layers){

       temp_layer.reserve_memory(w_map,h_map);
   }
   tiles_size=sz_tiles;
}

//Set the Sprite Id of Tile In the Indicated Layer, Column and Row
void System_TileMap::set_tile(uint8_t layer_index,uint32_t column,uint32_t row,uint16_t sprite_id){
   if(layer_index<0 || layer_index>=layers.size()){return;}
   layers[layer_index].set_tile(column,row,sprite_id);
}

//Set the Sprite Id  in the Indicated Tile Index of Indicated Layer
void System_TileMap::set_tile(uint8_t layer_index,uint32_t index_tile,uint16_t sprite_id){
   if(layer_index<0 || layer_index>=layers.size()){return;}
   layers[layer_index].set_tile(index_tile,sprite_id);
}


//Get the Width (Column Count) of Tile Layer
uint32_t System_TileMap::get_width_layer(uint8_t layer_index){
   if(layer_index<0 || layer_index>=layers.size()){
       return 0;
   }
   return layers[layer_index].get_width();
}

//Get Sprite Id from the Indicated Tile Index of Tile Layer
uint16_t System_TileMap::get_sprite_tile(uint8_t layer_index,uint32_t index_tile){
   if(layer_index<0 || layer_index>=layers.size()){
       return 0;
   }
   return layers[layer_index].get_sprite_index(index_tile);
}

//Get the Sprite Id of Indicate Tile Layer , Column and Row
uint16_t System_TileMap::get_sprite_tile(uint8_t layer_index,uint32_t column,uint32_t row){
   if(layer_index<0 || layer_index>=layers.size()){
       return 0;
   }
   return layers[layer_index].get_sprite_index(column,row);
}

//Get the Size In pixels of Tiles
uint8_t System_TileMap::get_tiles_size(){
     return tiles_size;
}

//Get the Number of Layers of Tilemap
uint8_t System_TileMap::get_layers_count(){
   return layers.size();
}
