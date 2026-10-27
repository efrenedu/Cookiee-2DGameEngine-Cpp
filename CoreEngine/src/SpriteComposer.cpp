#include "SpriteComposer.h"
#include<iostream>

//Reserve Memory for the System
void SpriteComposer_System::Reserve(uint16_t count_components,uint16_t count_entities,uint16_t fillval){
    EntitytoDense.assign(count_entities,fillval);
   spritesComposer_list.reserve(count_components);
}

//Free Memory of the System
void SpriteComposer_System::Free_Memory(){
   EntitytoDense.clear();
   spritesComposer_list.clear();
}

//Register a Entity with SpriteComposer
void SpriteComposer_System::Regiser_Entity(uint16_t id,std::vector<SpriteComposer_Item> list_sprites){
   uint16_t next_index=spritesComposer_list.size();
   EntitytoDense[id]=next_index;
   SpriteComposer_Data composer_spr;
   composer_spr.sprites=list_sprites;
   spritesComposer_list.push_back(composer_spr);
}

//get the Sprite Composer by Id
SpriteComposer_Data* SpriteComposer_System::get_spriteComposer(uint16_t id){
   return &spritesComposer_list[EntitytoDense[id]];
}
