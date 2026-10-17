#include "Quatree.h"

//Init a Quatree Node
void Quatree_Node::Init(float centerX,float centerY,float half_width,float half_height,std::vector<uint16_t> parent_entities,uint16_t total_entities){
   left=centerX-half_width;
   right=centerX+half_width;
   EntitytoDense.assign(total_entities,0x0FFFF);
   for(uint16_t i=0;i<parent_entities.size();i++){

   }
}
//Clear the Quatree Node
void Quatree_Node::clear_node(){
  entities_inside.clear();
  EntitytoDense.clear();
}

//Init a Quatree with the Size of Map and Total of Entities
void Quatree_System::init_map(uint32_t w_map,uint32_t h_map,std::vector<uint16_t> entities,uint8_t depth){
   float center_mapX=w_map*0.5;
   float center_mapY=h_map*0.5;
   root.Init(center_mapX,center_mapY,w_map*0.5,h_map*0.5,entities,entities.size());

}
//Clear the Quatree
void Quatree_System::clear_map(){
    root.clear_node();

}
