#include "Render.h"

//Register a New Render On the System
void Render_System::register_Entity(bool visibl,uint8_t draw_mod,uint32_t spr_id,int16_t z_indx,uint16_t id_entity){

  Render_data dat_render;
  dat_render.visible=visibl;
  dat_render.sprite_id=spr_id;
  dat_render.z_index=z_indx;
  dat_render.draw_mode=draw_mod;
  uint16_t next_id=list_renders.size();
  list_renders.push_back(dat_render);
  EntitytoDense[id_entity]=next_id;
}

//Reserve Memory for the System
void Render_System::reserve_memory(uint16_t dense_count,uint16_t entities_count,uint16_t fillEntities_value){
   list_renders.reserve(dense_count);
   EntitytoDense.assign(entities_count,fillEntities_value);

}

//Free the System
void Render_System::free_system(){
   EntitytoDense.clear();
   list_renders.clear();
}

//Get the Render Data by Id
Render_data* Render_System::get_render(uint16_t id){
    return &list_renders[EntitytoDense[id]];
}
