#ifndef QUATREE_H
#define QUATREE_H
#include <vector>
#include<cstdint>

//The Node of a Quatree
struct Quatree_Node{
  public:
       void Init(float centerX,float centerY,float half_width,float half_height,std::vector<uint16_t> parent_entities,uint16_t total_entities);
       void clear_node();
       void reserve_memory(uint16_t total_entities);
       void add_entity(uint16_t id);
       void remove_entity(uint16_t id);
       void exist_entity(uint16_t id);
       bool inside(float left_point,float right_point,float top_point,float bottom_point);
       std::vector<uint16_t> get_entities();

  private:
      std::vector<uint16_t> entities_inside;
      std::vector<uint16_t> EntitytoDense;
      float left;
      float right;
      float bottom;
      float top;

};

//System for the Quatree of World
struct Quatree_System
{
   public:
       void init_map(uint32_t w_map,uint32_t h_map,std::vector<uint16_t> entities,uint8_t depth=3);
       void clear_map();
       std::vector<uint16_t> get_entities();

   private:
       Quatree_Node root;
};

#endif // QUATREE_H
