#include "Quatree.h"
#include "Scene.h"
#include <math_structs.h>

//Init the System and Assign max Depth
void Quatree_System::Init(uint8_t depth){
   max_depth=depth;
}

//Set the Quatree World for a new Scene
void Quatree_System::Set_World(Scene* target_scene,uint32_t w_map,uint32_t h_map,std::vector<uint16_t> entities,std::vector<Entity_TreeData> &entities_tree_dat){
    clear_World();
    count_leafs=0;
    std::unordered_map<uint16_t,std::vector<float>> dim_entities=get_Entities_Dimensions(entities,target_scene);
    float half_width=w_map*0.5;
    float half_height=h_map*0.5;

    float centerMap_x=half_width;
    float centerMap_y=-half_height;
    //Build Root Node
    Quatree_Leaf root;
    root.left=centerMap_x-half_width;
    root.right=centerMap_x+half_width;
    root.top=centerMap_y+half_height;
    root.bottom=centerMap_y-half_height;
    root.dept_node=max_depth+1;
    count_leafs+=1;
    list_nodes.push_back(root);

    //Build Root Childs
    std::vector<Quatree_Leaf> list_childs=build_Quatree_Childs(centerMap_x,centerMap_y,half_width,half_height);
    for(uint8_t i=0;i<list_childs.size();i++){
        list_nodes[0].childs.push_back(count_leafs);
        count_leafs+=1;
        list_nodes.push_back(list_childs[i]);
    }
    for (uint8_t i=0;i<list_nodes[0].childs.size();i++){
        build_tree(max_depth,list_nodes[0].childs[i],target_scene,entities,entities_tree_dat,dim_entities);
    }
    dim_entities.clear();
}

//Build the Tree of Quatree
void Quatree_System::build_tree(uint8_t actual_depth,uint16_t node_index,Scene*target_scene,std::vector<uint16_t> list_entities,std::vector<Entity_TreeData>&entities_quatree_dat,std::unordered_map<uint16_t,std::vector<float>>dim_entities){
   if(target_scene==NULL){
     return;
   }
   //Get the Entities Inside Node
   Transform_System* transf_sys=target_scene->get_transform_sys();
   uint16_t count_entities=target_scene->get_count_entities();
   std::vector<uint16_t> inside_entities;
   for(uint16_t index:list_entities){
      if(index>=count_entities){
          continue;
      }
      Transform_Data* transf=transf_sys->get_transform(index);
      std::vector<float> size_entity=dim_entities[index];
      float halfW_Entity=size_entity[0]*0.5;
      float halfH_Entity=size_entity[1]*0.5;
      float left_p=transf->x-halfW_Entity;
      float right_p=transf->x+halfW_Entity;
      float top_p=transf->y+halfH_Entity;
      float bottom_p=transf->y-halfH_Entity;
      if(inside_Node(left_p,right_p,top_p,bottom_p,list_nodes[node_index])){
         inside_entities.push_back(index);
      }
   }
   list_nodes[node_index].dept_node=actual_depth;
   //Process the Node
   if(actual_depth==0){
       for(uint16_t index:inside_entities){
          entities_quatree_dat[index].lastNodes_Inside.push_back(node_index);
          list_nodes[node_index].entities_Inside.push_back(index);
        }
   }
   else{
      Quatree_Leaf target=list_nodes[node_index];
      float halfw=Math_G::Abs(target.centerX-target.left);
      float halfh=Math_G::Abs(target.centerY-target.bottom);
      std::vector<Quatree_Leaf> childs=build_Quatree_Childs(target.centerX,target.centerY,halfw,halfh);
      for(uint8_t i=0;i<childs.size();i++){
         list_nodes[node_index].childs.push_back(count_leafs);
         list_nodes.push_back(childs[i]);
         count_leafs++;
         build_tree(actual_depth-1,list_nodes[node_index].childs[i],target_scene,inside_entities,entities_quatree_dat,dim_entities);
      }

   }

}

//Free Memory of Quatree
void Quatree_System::clear_World(){
   queue_reassign.clear();
   for(uint16_t i=0;i<list_nodes.size();i++){
      list_nodes[i].entities_Inside.clear();
      list_nodes[i].childs.clear();
   }
   list_nodes.clear();
}

//Request Reassign a Entity to the Quatree
void Quatree_System::request_reassignEntity(uint16_t id_entity){

   for(uint16_t index:queue_reassign){
       if(index==id_entity){
          return;
       }
   }
   queue_reassign.push_back(id_entity);
}

//Process to Reassign Entities if Exists Requests
bool Quatree_System::Process_Reassign(Scene* target_scene){
   if(target_scene==NULL){
         return false;
   }
   unordered_map <uint16_t ,std::vector<float>> sizes_entities=get_Entities_Dimensions(queue_reassign,target_scene);
   uint16_t entities_count=target_scene->get_count_entities();
   Transform_System* transf_sys=target_scene->get_transform_sys();
   bool entities_change_something=false;
   for(uint16_t id:queue_reassign){
      if(id>=entities_count){
         continue;
      }
      Transform_Data* transf=transf_sys->get_transform(id);
      std::vector<float> target_dim=sizes_entities[id];
      float w=target_dim[0];
      float h=target_dim[1];
      float x=transf->x;
      float y=transf->y;
      float half_w=w*0.5;
      float half_h=h*0.5;
      float left_pos=x-half_w;
      float right_pos=x+half_w;
      float top_pos=y+half_h;
      float bottom_pos=y-half_h;
      Entity_TreeData* tree_dat=target_scene->get_entity_treeData(id);
      if(tree_dat==NULL){
         continue;
      }
      std::vector<uint16_t> already_inside;
      uint8_t number_nodes_inside=tree_dat->lastNodes_Inside.size();
      for(uint8_t i=0;i<tree_dat->lastNodes_Inside.size();i++){
         Quatree_Leaf target_node=list_nodes[tree_dat->lastNodes_Inside[i]];
         if(inside_Node(left_pos,right_pos,top_pos,bottom_pos,target_node)==false){
               entities_change_something=true;
               for(uint8_t j=0;j<target_node.entities_Inside.size();j++){
                  if(target_node.entities_Inside[j]==id){
                     target_node.entities_Inside[j]=target_node.entities_Inside.back();
                      target_node.entities_Inside.pop_back();
                      break;
                  }
               }
               list_nodes[tree_dat->lastNodes_Inside[i]]=target_node;
               tree_dat->lastNodes_Inside[i]=std::move(tree_dat->lastNodes_Inside.back());
               tree_dat->lastNodes_Inside.pop_back();
         }
          else{
              already_inside.push_back(tree_dat->lastNodes_Inside[i]);
          }
      }
      ReassignEntity_Tree(left_pos,right_pos,top_pos,bottom_pos,id,tree_dat,already_inside,0);
      if(entities_change_something==false){
         if(number_nodes_inside<tree_dat->lastNodes_Inside.size()){
             entities_change_something=true;
         }
      }
   }
   sizes_entities.clear();
   queue_reassign.clear();
   return entities_change_something;
}


//Get the Entities Inside Indicated Bounding Box
std::vector<uint16_t> Quatree_System::get_entities_fromRect(float left,float right,float top,float bottom){

    std::vector<uint16_t> res;
    FindEntities_Tree(left,right,top,bottom,res,0);
    return res;
}

//Iterate the Tree and get the Entities Id On Depth 0 Nodes
void Quatree_System::FindEntities_Tree(float left,float right,float top,float bottom,std::vector<uint16_t>& inside_entities,uint16_t node_index){

  if(inside_Node(left,right,top,bottom,list_nodes[node_index])==false){
      return;
  }
  if(list_nodes[node_index].dept_node==0){
     for(uint16_t index:list_nodes[node_index].entities_Inside){
          bool repeated=false;
          for(uint16_t temp_index: inside_entities){
              if(index==temp_index){
                  repeated=true;
                  break;
              }
          }
          if(repeated){
             continue;
          }
          inside_entities.push_back(index);
     }
  }
  else{
      for(uint8_t i=0;i<4;i++){
          FindEntities_Tree(left,right,top,bottom,inside_entities,list_nodes[node_index].childs[i]);
      }
  }
}

//Explore the Tree and Reassign the entity Nodes
void Quatree_System::ReassignEntity_Tree(float left,float right,float top,float bottom,uint16_t id,Entity_TreeData* tree_dat,std::vector<uint16_t> already_inside,uint16_t node_index){

    if(inside_Node(left,right,top,bottom,list_nodes[node_index])==false){
         return;
    }
    if(list_nodes[node_index].dept_node==0){
        for(uint16_t id_node:already_inside){
              if(id_node==node_index){
                  return;
              }
        }
        tree_dat->lastNodes_Inside.push_back(node_index);
        list_nodes[node_index].entities_Inside.push_back(id);

    }
    else{
       for(uint8_t i=0;i<4;i++){
           ReassignEntity_Tree(left,right,top,bottom,id,tree_dat,already_inside,list_nodes[node_index].childs[i]);
       }

    }
}

//get the Max Depth of Quatree
uint8_t Quatree_System::get_max_depth(){
   return max_depth;
}

//Get The Size of Indicated Entities
std::unordered_map<uint16_t,std::vector<float>> get_Entities_Dimensions(std::vector<uint16_t> id_list,Scene* target_scene){
   std::unordered_map<uint16_t,std::vector<float>> res;
   if(target_scene==NULL){
     return res;
   }
   Transform_System* transf_sys=target_scene->get_transform_sys();
   Render_System* render_sys=target_scene->get_render_sys();
   Collider_System* collider_sys=target_scene->get_collider_sys();
   textureManager_system* tex_sys=target_scene->get_texture_system();
   float default_width=32;
   float default_heigth=32;
   uint16_t entities_count=target_scene->get_count_entities();
   for(uint16_t id:id_list){
      std::vector<float> target_dim={0,0};
      if(id>=entities_count){
         res[id]=target_dim;
         continue;
      }
      Transform_Data* transf=transf_sys->get_transform(id);
      float w=0;
      float h=0;
      float x=transf->x;
      float y=transf->y;
      std::bitset<64> mask= target_scene->get_mask(id);
      if(HasComponentBit(mask,RENDER_COMP)){
          //Try get Size from Sprite
          Render_data* rend_dat=render_sys->get_render(id);
          if(rend_dat!=NULL){
              SDL_Rect sprite_rect;
              SDL_Texture* tex=tex_sys->get_texture_fromSprite(sprite_rect,rend_dat->sprite_id);
              if(tex!=NULL){
                  w=sprite_rect.w;
                  h=sprite_rect.h;
              }
          }
      }
      if(HasComponentBit(mask,COLLIDER_COMP)){
          //Try get Size from Collider
          Collider_Data* collider=collider_sys->get_collider(id);
          if(collider!=NULL){
              float w_collider=0;
              float h_collider=0;
              if(collider->collider_type==FIGURE_CIRCLE){
                  w_collider=collider->shape.circle.radius*2;
                  h_collider=w_collider;
              }
              else{
                 w_collider=collider->shape.box.width;
                 h_collider=collider->shape.box.height;
              }
              if(w_collider>w){
                  w=w_collider;
              }
              if(h_collider>h){
                 h_collider=h;
              }
          }
      }
      if(w==0 || h==0){
         //Entity without Sprite and Collider
         w=default_width;
         h=default_heigth;
      }
      res[id]={w,h};
   }
}

//Return True if the Indicated Rect is Inside of Quatre node Rect
bool inside_Node(float left_target,float right_target,float top_target ,float bottom_target,Quatree_Leaf node){
    if(left_target<=node.right && right_target>=node.left){
        if(bottom_target<=node.top &&top_target>=node.bottom ){
            return true;
        }
    }
    return false;
}

//Build the Childs for a Parent Quatree Node
std::vector<Quatree_Leaf> build_Quatree_Childs(float centerParent_x,float centerParent_y,float half_widthParent,float half_heightParent){
   std::vector<Quatree_Leaf> res;
   res.reserve(4);
   float childs_halfWidth=half_widthParent*0.5;
   float childs_halfHeight=half_heightParent*0.5;
   for(uint8_t i=0;i<4;i++){
       Quatree_Leaf child;
       float child_cX=centerParent_x;
       float child_cY=centerParent_y;
       switch(i){
          case 0:
              child_cX-=childs_halfWidth;
              child_cY+=childs_halfHeight;
              break;
          case 1:
              child_cX+=childs_halfWidth;
              child_cY+=childs_halfHeight;
              break;
          case 2:
              child_cX-=childs_halfWidth;
              child_cY-=childs_halfHeight;
             break;
          case 3:
              child_cX+=childs_halfWidth;
              child_cY-=childs_halfHeight;
             break;
          default:
             break;

       }
       child.left=child_cX-childs_halfWidth;
       child.right=child_cX+childs_halfWidth;
       child.top=child_cY+childs_halfHeight;
       child.bottom=child_cY-childs_halfHeight;
       child.centerX=child_cX;
       child.centerY=child_cY;
       res.push_back(child);
   }
   return res;
}
