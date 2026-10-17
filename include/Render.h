#ifndef RENDER_H
#define RENDER_H
#include<cstdint>
#include<vector>
using namespace std;

//Indicate the Way to draw the Sprites
enum RENDER_MODES:uint8_t{
   NORMAL_MODE=0,MULTIPLY_MODE=1,ADD_MODE=2,
};

//Store for the Data of Render
struct Render_data{
   uint32_t sprite_id;
   int8_t z_index;
   bool visible;
   uint8_t draw_mode;
};

//System for Render Component
struct Render_System{
   public:
      Render_data* get_render(uint16_t id);
      void register_Entity(bool visibl,uint8_t draw_mod,uint32_t spr_id,int16_t z_indx,uint16_t id_entity);
      void free_system();
      void reserve_memory(uint16_t dense_count,uint16_t entities_count ,uint16_t fillEntities_value);
   private:
     vector<Render_data> list_renders;
     vector<uint16_t> EntitytoDense;

};

#endif // RENDER_H
