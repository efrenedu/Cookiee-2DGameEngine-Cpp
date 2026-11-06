#include "Fonts.h"
#include<iostream>


//Bitmap Font Methods
void  Bitmap_Font::load_font(const char* src,uint32_t hash_name,Font_params params,SDL_Renderer* target_render){
   Glyph fill_val;
   hash_id=hash_name;
   uint16_t size_total=FONT_MAX_VARIANT*FONTCHAR_LIMIT;
   letters_list.assign(size_total,fill_val);

   SDL_Surface* loadedSurface =IMG_Load(src);
   if(loadedSurface!=NULL ){
      SDL_Surface* optimized=SDL_ConvertSurfaceFormat(loadedSurface,SDL_PIXELFORMAT_RGBA8888,0);
      if(optimized!=NULL){
          uint16_t w_tex=optimized->w;
          uint16_t h_tex=optimized->h;

          font_texture=SDL_CreateTextureFromSurface(target_render,optimized);
          if(font_texture!=NULL){
               if(params.custom_Font_dat.size()<=0){
                  set_glyphs(params,w_tex,h_tex);
               }
               else{
                  set_glyphs_Customized(params,w_tex,h_tex);
               }

          }
          SDL_FreeSurface(optimized);
      }
      SDL_FreeSurface(loadedSurface);
  }

}


//Get the Glyphs of Texture Calculated By Maths
void Bitmap_Font::set_glyphs(Font_params params,uint16_t w_texture,uint16_t h_texture){
   if(w_texture<=0 || h_texture<=0){
      return;
   }
   uint8_t columns=params.columns_font;
   uint8_t rows_count=params.rows_font;
   uint16_t letter_count=0;
   for (uint8_t i=0;i<FONT_MAX_VARIANT;i++){
      if(params.exist_variant[i]==false){
         //Variant Not Available On Font
         letter_count+=FONTCHAR_LIMIT;
         continue;
      }

      uint16_t offsetX_variant=params.offsetX_variants[i];
      uint16_t offsetY_variant=params.offsetY_variants[i];
      uint8_t count_letters_variant=0;
      for(uint8_t row=0;row<rows_count;row++){
         for(uint8_t col=0;col<columns;col++){
            uint16_t px=(col*(params.width_chars+params.separation_letters))+params.margin_texture;
            uint16_t py=(row*(params.height_chars+params.separation_letters))+params.margin_texture;
            uint8_t index_letter=0;
            Glyph letter;
            letter.x=px+offsetX_variant;
            letter.y=py+offsetY_variant;
            letter.w=params.width_chars;
            letter.h=params.height_chars;
            if(letter.x+letter.w>w_texture || letter.y+letter.h>h_texture){
                 //Invalid Coord On Texture
                 letter_count++;
                 continue;
            }
            if(count_letters_variant>=FONTCHAR_LIMIT){
                 row=rows_count;
                 break;
            }
            letters_list[letter_count]=letter;
            letter_count++;
            count_letters_variant++;
        }

      }
   }

}

//Set the Glyphs of Fonts by a Custom Data
void Bitmap_Font::set_glyphs_Customized(Font_params params,uint16_t w_texture,uint16_t h_texture){
   if(w_texture<=0 || h_texture<=0){
      return;
   }
   uint16_t limit=FONTCHAR_LIMIT*FONT_MAX_VARIANT;
   for(uint16_t i=0;i<params.custom_Font_dat.size();i++){
       customFont_Data temp_dat=params.custom_Font_dat[i];
       uint16_t index=temp_dat.id_letter;

       if(index>=limit){
          continue;
       }
       uint16_t px=temp_dat.pos_x;
       uint16_t py=temp_dat.pos_y;
       Glyph letter;
       letter.x=px;
       letter.y=py;
       letter.w=params.width_chars;
       letter.h=params.height_chars;
       if(letter.x+letter.w>w_texture || letter.y+letter.h>h_texture){
          //Invalid Coord On Texture
           continue;
       }

       letters_list[index]=letter;
   }
}


//Get the Texture of the Font
SDL_Texture*  Bitmap_Font::get_texture(){
   return font_texture;
}

//Get a Letter by Id
Glyph*  Bitmap_Font::get_letter(uint8_t letter_id,uint8_t variant_id){
   if(letter_id>=FONTCHAR_LIMIT || variant_id>=FONT_MAX_VARIANT){
       return NULL;
   }
   uint16_t offset=0;
   if(variant_id!=0){
     offset=(FONTCHAR_LIMIT*variant_id);
   }
   uint16_t id_target=offset+letter_id;
   return &letters_list[id_target];
}

//Get a Letter by the char
Glyph* Bitmap_Font:: get_letter(char c,uint8_t variant_id){
   int8_t code=static_cast<int8_t>(c)-32;
   if(code<0 || code>=FONTCHAR_LIMIT){
       return NULL;
   }
   if(variant_id>=FONT_MAX_VARIANT){
       return NULL;
   }
   uint16_t offset=0;
   if(variant_id!=0){
     offset=(FONTCHAR_LIMIT*variant_id);
   }
   uint16_t id_target=offset+code;
   return &letters_list[id_target];
}

//get the Hash Id of Font
uint32_t Bitmap_Font::get_hash(){
   return hash_id;
}

//Get the Expected width in Pixels for the Required str
 uint16_t Bitmap_Font::get_expected_width(std::string str){
    uint16_t total=0;
    if(letters_list.size()<=0){
        return 0;
    }
    uint16_t w_letters=letters_list[0].w;
    for(uint8_t i=0;i<str.length();i++){
        total+=w_letters;
    }
    return total;
 }

//Reserve Memory for the List of Fonts
void Fonts_System:: reserve_memory(uint8_t num_fonts){
   list_fonts.reserve(num_fonts);
}

//Free the Font System
void Fonts_System::free_System(){
  list_fonts.clear();
  for(uint8_t i=0;i<renderedTexts_catalog.size();i++){
      SDL_DestroyTexture(renderedTexts_catalog[i].texture_text);
  }
  renderedTexts_catalog.clear();
}

//Add a Font to the System from a Image Source
void Fonts_System::add_font(const char* src,uint32_t hash_name,Font_params params,SDL_Renderer* target_render){
  Bitmap_Font next_font;
  next_font.load_font(src,hash_name,params,target_render);
  list_fonts.push_back(next_font);

}
//Get the Font by Id
Bitmap_Font* Fonts_System:: get_font(uint8_t id){
   if(id>=list_fonts.size()){
       return NULL;
   }
   return &list_fonts[id];
}

//Get the Id of Font On System with the hash of the Name
uint8_t Fonts_System::get_FontId(uint32_t hash_name){
   for(uint8_t i=0;i<list_fonts.size();i++){
       if(hash_name==list_fonts[i].get_hash()){
           return i;
       }
   }
   return 0xFF;
}

//Get the Saved Rendered Text by the Index
Rendered_Text* Fonts_System:: get_Rendered_Text(uint8_t index){
   if(index>=renderedTexts_catalog.size()){
       return NULL;
   }
   return &renderedTexts_catalog[index];
}

uint8_t Fonts_System::save_text(SDL_Texture* tex,uint16_t w,uint16_t h){
   uint8_t next_index=renderedTexts_catalog.size();
   Rendered_Text next_text;
   next_text.texture_text=tex;
   next_text.width=w;
   next_text.height=h;
   renderedTexts_catalog.push_back(next_text);
   return next_index;
}


//Support Function definition
std::string wrap_text(std::string text,uint16_t max_width,Bitmap_Font* font){
   if(font==NULL){
      return text;
   }
   std::string current_token="";
   std::vector<text_token> tokens;;
   for(uint16_t i=0;i<text.length();i++){
      char c=text[i];
      if(c==' ' || c=='\n'){
        if(!current_token.empty()){
            tokens.push_back({current_token,false});
            current_token="";
        }
        if(c==' '){
            tokens.push_back({" ",true});
        }
        else if(c=='\n'){
            tokens.push_back({"\n",true});
        }

      }
      else{
        current_token+=c;
      }
   }
  if(!current_token.empty()){
        tokens.push_back({current_token,false});
  }
  std::string final_wrappedtext="";
  std::string current_line="";
  uint16_t current_w=0;
  for(text_token& temp_token:tokens){
    if(temp_token.text=="\n"){
        if(!final_wrappedtext.empty()){
            final_wrappedtext+="\n";
        }
        final_wrappedtext+=current_line;
        current_line="";
        continue;
    }
    std::string test_line=current_line+temp_token.text;
    uint16_t w_line=font->get_expected_width(test_line);
    if(w_line>max_width && (!current_line.empty())){
       if(!final_wrappedtext.empty()){
           final_wrappedtext+="\n";
       }
       if(current_line.back()==' '){
          current_line.pop_back();
       }
       final_wrappedtext+=current_line;
       current_line=temp_token.text;
    }
    else{
        current_line=test_line;
    }
  }
  if(!current_line.empty()){
     if(!final_wrappedtext.empty()){
         final_wrappedtext+="\n";
     }
     final_wrappedtext+=current_line;
  }
  return final_wrappedtext;
}
