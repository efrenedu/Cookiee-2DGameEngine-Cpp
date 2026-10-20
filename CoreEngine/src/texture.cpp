#include "texture.h"
#include<iostream>
#include<fstream>
#include "pugixml.hpp"
#include<hash_support.h>

//Sprite Catalog Functions

//Reserve Memory for The Sprite Catalog
void Sprite_Catalog::reserve_memory(uint16_t num_sprites){
   list_sprites.reserve(num_sprites);
}

//Free Memory of Sprite Catalog
void Sprite_Catalog::free_memory(){
    list_sprites.clear();
}

//Texture Functions

//Load a Texture from Tileset of Tiled
bool Texture::load_fromTileset(Sprite_Catalog& catalog,uint8_t tex_id,const char* src,SDL_Renderer* render_win,uint16_t firstgid,std::string& image_src){
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(src);
  if (!result) {
     return false;
  }
  uint8_t num_images=0;
  column_count=0;
  pugi::xml_node tilesetNode = doc.child("tileset");
  if(!tilesetNode ){
      return false;
  }
  pugi::xml_node img_node=tilesetNode.child("image");
  if(!img_node ){
      return false;
  }

  std::string src_tempImg=img_node.attribute("source").as_string();
  src_tempImg=src_tempImg.substr(src_tempImg.find_last_of("/\\") + 1);
  image_src=src_tempImg;
  std::string srcImg="assets\\images\\"+src_tempImg;
  width=img_node.attribute("width").as_uint(16);
  height=img_node.attribute("height").as_uint(16);
  firstid=firstgid;
  uint16_t sprites_count=tilesetNode.attribute("tilecount").as_uint(16);
  lastid=firstgid+(sprites_count-1);
  uint16_t spr_w=tilesetNode.attribute("tilewidth").as_uint(16);
  uint16_t spr_h=tilesetNode.attribute("tileheight").as_uint(16);
  uint8_t spacing=tilesetNode.attribute("spacing").as_uint(8);
  uint8_t margin=tilesetNode.attribute("margin").as_uint(8);
  uint16_t columns=tilesetNode.attribute("columns").as_uint(8);
  if(columns==0 && width>0 && spr_w>0){
     columns=width/spr_w;
  }
  column_count=columns;
  loaded=false;

  SDL_Surface* loadedSurface = IMG_Load(srcImg.c_str());
  if(loadedSurface!=NULL ){
      SDL_Surface* optimized=SDL_ConvertSurfaceFormat(loadedSurface,SDL_PIXELFORMAT_RGBA8888,0);
      if(optimized!=NULL){
          tex=SDL_CreateTextureFromSurface(render_win,optimized);
          if(tex!=NULL){
               loaded=true;
               set_sprites(catalog,tex_id,spr_w,spr_h,margin,spacing,columns,sprites_count);
          }
          SDL_FreeSurface(optimized);
      }
      SDL_FreeSurface(loadedSurface);
  }
  return loaded;
}

//Add the Sprites of a Texture to the Sprite Catalog
void Texture::set_sprites(Sprite_Catalog& catalog,uint8_t tex_id,uint16_t w_spr,uint16_t h_spr,uint8_t margin,uint8_t spacing,uint16_t columns,uint16_t spr_count){
  for(uint16_t i=0;i<spr_count;i++){
    uint16_t col=i% columns;
    uint16_t row=i/columns;
    Sprite_Source sprite;
    uint16_t tempx=static_cast<uint16_t>(col*w_spr);
    uint16_t tempy=static_cast<uint16_t>(row*h_spr);
    uint16_t offsetx=col*spacing;
    uint16_t offsety=row*spacing;
    sprite.x=tempx+margin+offsetx;
    sprite.y=tempy+margin+offsety;
    sprite.w=w_spr;
    sprite.h=h_spr;
    sprite.texture_index=tex_id;

    catalog.list_sprites.push_back(sprite);

  }
}

//Return True if the Texture Load Succesfull
bool Texture::is_loaded(){
  return loaded;
}

//Free Memory of Texture
void Texture::free_memory(){
   if(tex!=NULL){
       SDL_DestroyTexture(tex);
       tex=NULL;
   }
}

//Get the SDL Texture
SDL_Texture* Texture::get_texture(){
   return tex;
}

//Get the Index of Sprite by the Row and Column
uint16_t Texture::getId_fromGrid(uint16_t row,uint16_t column,uint16_t& column_number){

    uint16_t res=firstid-1;
    res=res+(row*column_count)+column;
    column_number=column_count;
    return res;
}

//Texture Manager System Functions


//Free Memory of System
void textureManager_system::free_memory(){
    for (Texture &temp_tex:list_textures){
           temp_tex.free_memory();
    }
    catalog.free_memory();
    hash_textures.clear();
    list_textures.clear();
}

//Reserve Memory for The System
void textureManager_system::reserve_memory(uint8_t number_textures){
   list_textures.reserve(number_textures);
   hash_textures.reserve(number_textures);
}

//Add a New Texture from a Tileset of Tiled
void textureManager_system::add_fromTileset(const char* src,SDL_Renderer* render_win,uint16_t firstgid){
   Texture temp_tex;
   std::string image_src="";
   if(temp_tex.load_fromTileset(catalog,list_textures.size(),src,render_win,firstgid,image_src)==false){
      std::cout<<"fail load texture from"<<src<<std::endl;
      return;
   }
   hash_textures.push_back(hashString(image_src.c_str()));
   list_textures.push_back(temp_tex);
}

//Get the Texture of Indicated Texture Index
SDL_Texture* textureManager_system::get_texture(uint8_t tex_id){
    if( tex_id>=list_textures.size()){
           return NULL;
    }

    return list_textures[tex_id].get_texture();
}

//Get the Texture from Indicate Sprite Id
SDL_Texture* textureManager_system ::get_texture_fromSprite(SDL_Rect& rect_spr,uint16_t sprite_id){
    if( sprite_id>=catalog.list_sprites.size()){
        return NULL;
    }
    Sprite_Source spr=catalog.list_sprites[sprite_id];
    uint8_t index=spr.texture_index;
    if(index>=list_textures.size()){
        return NULL;
    }
    rect_spr.x=spr.x;
    rect_spr.y=spr.y;
    rect_spr.w=spr.w;
    rect_spr.h=spr.h;
    return list_textures[index].get_texture();
}

//Get the Texture By Name of Source
uint8_t textureManager_system::get_textureId_fromNameSource(const char* src){
  uint32_t hash_name=hashString(src);
  for(uint8_t i=0;i<hash_textures.size();i++){
      if(hash_name==hash_textures[i]){
         return i;
      }
  }
  return list_textures.size();
}

//Get the Sprite Index of Indicated Texture,Row and Column  plus the Number Columns of Texture
uint16_t textureManager_system::get_spriteby_grid(uint8_t text_id,uint16_t row,uint16_t column,uint16_t& number_columns){
   if(text_id>=list_textures.size()){
      return catalog.list_sprites.size();
   }
   Texture target=list_textures[text_id];
   return target.getId_fromGrid(row,column,number_columns);
}
