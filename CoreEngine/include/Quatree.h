#ifndef QUATREE_H
#define QUATREE_H
#include <vector>
#include<cstdint>
#include<unordered_map>
class Scene;

//The Quatree Node
struct Quatree_Leaf{
    std::vector<uint16_t>childs;
    std::vector<uint16_t> entities_Inside;
    float left;
    float right;
    float top;
    float bottom;
    float centerX;
    float centerY;
    uint8_t dept_node;


};

//Data of Each Entity of Quatree
struct Entity_TreeData{
   std::vector<uint16_t> lastNodes_Inside;
   float last_x;
   float last_y;
};


//System for the Quatree of World
struct Quatree_System
{
   public:
       void Init(uint8_t depth);
       void Set_World(Scene* target_scene,uint32_t w_map,uint32_t h_map,std::vector<uint16_t> entities,std::vector<Entity_TreeData>& entities_quatree_dat);
       void clear_World();
       void request_reassignEntity(uint16_t id_entity);
       bool Process_Reassign(Scene* target_scene);
       std::vector<uint16_t> get_entities_fromRect(float left,float right,float top,float bottom);
       uint8_t get_max_depth();
   private:
       void build_tree(uint8_t actual_depth,uint16_t node_index,Scene*target_scene,std::vector<uint16_t> list_entities,std::vector<Entity_TreeData>& entities_quatree_dat,std::unordered_map<uint16_t,std::vector<float>>dim_entities);
       void FindEntities_Tree(float left,float right,float top,float bottom,std::vector<uint16_t> &inside_entities,uint16_t index_node);
       void ReassignEntity_Tree(float left,float right,float top,float bottom,uint16_t id,Entity_TreeData* tree_dat,std::vector<uint16_t> already_inside,uint16_t node_index);
       std::vector<uint16_t> queue_reassign;
       std::vector<Quatree_Leaf> list_nodes;
       uint8_t max_depth;
       uint16_t count_leafs;

};


//Support Functions for Quatree
std::unordered_map<uint16_t,std::vector<float>> get_Entities_Dimensions(std::vector<uint16_t> id_list,Scene* target_scene);
bool inside_Node(float left_target,float right_target,float top_target ,float bottom_target,Quatree_Leaf node);
std::vector<Quatree_Leaf> build_Quatree_Childs(float centerParent_x,float centerParent_y,float half_widthParent,float half_heightParent);




#endif // QUATREE_H
