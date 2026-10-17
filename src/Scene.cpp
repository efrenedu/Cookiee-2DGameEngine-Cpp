#include "Scene.h"
#include "Render_DrawSystem.h"
#include "math_structs.h"

//Init the Scene  and Systems of It using Config Game Data
void Scene::Init(ma_engine* engine,std::unordered_map<std::string,std::unordered_map<std::string,std::string>> game_config){
   next_scene=-1;
   if(game_config.find("Scenes")!=game_config.end()){
      std::unordered_map<std::string,std::string> scenes_config=game_config["Scenes"];
      uint8_t num_scenes=0;
      if(scenes_config.find("Scene_Count")!=scenes_config.end()){
         num_scenes=Converter::strTo_UInt(scenes_config["Scene_Count"]);
      }
      list_scenes.reserve(num_scenes);
      for(uint8_t i=0;i<num_scenes;i++){
        std::string target_id="Scene_"+to_string(i);
        if(scenes_config.find(target_id)!=scenes_config.end()){
            list_scenes.push_back(scenes_config[target_id]);
         }
      }
   }
   float music_volume=1.0;
   float audio_volume=1.0;
   if(game_config.find("Audio_Config")!=game_config.end()){
      std::unordered_map<std::string,std::string> audio_config=game_config["Audio_Config"];
      if(audio_config.find("Music_Volume")!=audio_config.end()){
         music_volume=Math_G::Clamp(Converter::strTo_Float(audio_config["Music_Volume"]),0,1.0);
      }
      if(audio_config.find("Sfx_Volume")!=audio_config.end()){
        audio_volume=Math_G::Clamp(Converter::strTo_Float(audio_config["Sfx_Volume"]),0,1.0);
      }
   }
   if(list_scenes.size()>0){
      next_scene=0;
   }
   audio_sys.Init_System(engine,music_volume,audio_volume);
   last_entity=0;
   entities_changes_transform=true;
}

//Free Memory of Scene and Systems
void Scene::free_memory(){

   sys_textures.free_memory();
   audio_sys.Free_System();
   render_sys.free_system();
   transf_sys.free_system();
   collider_sys.free_system();
   rigidbd_sys.free_system();
   anim_sys.Free_System();
   components_masks.clear();
   list_scenes.clear();
}

//Reserve Memory for Each Component System
void Scene::reserve_memory(int16_t num_elements,map<uint8_t,uint16_t> components_count){
    components_masks.reserve(num_elements);
    for (uint8_t i=0;i<COMP_COUNT;i++){
        switch(i){
            case components_types::TRANSFORM_COMP:{
                transf_sys.reserve_memory(components_count[i]);
                break;
            }
            case components_types::RENDER_COMP:{
                render_sys.reserve_memory(components_count[i],num_elements,0xFFFF);
                break;
           }
           case components_types::COLLIDER_COMP:{
               collider_sys.reserve_memory(components_count[i],num_elements,0xFFFF);
               break;
           }
           case components_types::RIGIDBODY_COMP:{
               rigidbd_sys.reserve_memory(components_count[i],num_elements,0xFFFF);
               break;
           }
           case components_types::ANIMATOR_COMP:{
              anim_sys.Reserve_Memory(components_count[i],num_elements,0xFFFF);
              break;
           }
           case components_types::JOINT_COMP:{
              join_sys.reserve_memory(components_count[i],num_elements,0xFFFF);
              break;
           }
           case components_types::PLAYER_COMP:{
              player_sys.reserve_memory(components_count[i],num_elements,0xFFFF);
              break;
           }
           default:{
               break;
            }
        }
    }

}

//update the Scene
void Scene::update_scene(Render_DrawSystem* draw_sys,float dt,Input_System* input_sys,Game_Config_System* game_config_sys){
   if(dt>1.0){
      return;
   }
   audio_sys.update_system(game_config_sys);
   anim_sys.update_system(dt);
   uint8_t draw_request=REDRAW_NONE;
   //update entities scripts
   for(uint16_t i=0;i<last_entity;i++){
      uint16_t mask=components_masks[i];
      if(HasComponentBit(mask,PLAYER_COMP)){
           player_sys.update_player(mask,i,this,dt,input_sys);
      }
      if(HasComponentBit(mask,ANIMATOR_COMP) && HasComponentBit(mask,RENDER_COMP)){
            Render_data* render=render_sys.get_render(i);
            Animator_Data* anim=anim_sys.get_animator(i);
            if(anim->loaded==false){
                 continue;
            }
            uint8_t anim_target=anim->animation_actual;
            uint8_t frame_target=anim->list_animations[anim_target].actual_frame;
            uint16_t target_sprite=anim->list_animations[anim_target].frames[frame_target].sprite_id;
            if(target_sprite!=render->sprite_id){
                draw_request=REDRAW_ENTITIES;
                render->sprite_id=target_sprite;
            }
      }

   }
   if(main_camera.hasMoved_significantly()){
       main_camera.update_renderPos();
       draw_request=REDRAW_ALL;
       entities_changes_transform=false;
   }
   else if(entities_changes_transform){
      draw_request=REDRAW_ENTITIES;
      entities_changes_transform=false;
   }
   draw_sys->request_redraw(draw_request);
}

//Register the Entity in each Component System Requerid
void Scene::register_entity(int16_t entity_Id,ComponentMask mask,const Entity_Props& props){
     components_masks.push_back(mask);
     for (uint8_t i=0;i<COMP_COUNT;i++){
           components_types type_target=static_cast<components_types>(i);
           if(HasComponentBit(mask,type_target)){
               process_target_component(type_target,props,entity_Id);
           }
     }
     last_entity++;
}

// Process the Register of a Component On the Requerid System
void Scene::process_target_component(components_types comp_type,const Entity_Props& props,uint16_t id_entity){
    if(comp_type==UKNOW_COMP){
         return;
    }
    if(comp_type<=JOINT_COMP){
         process_basic_component(comp_type,props,id_entity);
    }
    else if(comp_type>JOINT_COMP && comp_type<=CANVAS_COMP){
         process_ui_component(comp_type,props,id_entity);
    }
    else{
         process_script_component(comp_type,props,id_entity);
    }


}

//Proces Basic Component (Transform,Render,Collider,RigidBody,Camera,Joint)
void Scene::process_basic_component(components_types comp_type,const Entity_Props& props,uint16_t id_entity){
   switch(comp_type){
       case components_types::TRANSFORM_COMP:{
          float px=props.get_float("position_x",0.0);
          float py=props.get_float("position_y",0.0);
          uint16_t scale=props.get_uint("scale",1);
          bool flip=props.get_bool("flip",false);
          float rot=props.get_float("rotation",0.0);
          transf_sys.register_transform(px,py,scale,rot,flip);
          break;
       }
       case components_types::RENDER_COMP:{
          int16_t z_index=props.get_int("z_index",0);
          uint8_t draw_mode=props.get_uint("draw_mode",0);
          bool visible=props.get_bool("visible",true);
          uint16_t gid=props.get_uint("gid",0);
          render_sys.register_Entity(visible,draw_mode,gid,z_index,id_entity);
          break;
       }
       case components_types::COLLIDER_COMP:{
          float radius=props.get_float("radius_collider",0.0);
          float radius_sensor=props.get_float("radius_sensor",0.0);
          float w=props.get_float("width_collider",0.0);
          float h=props.get_float("height_collider",0.0);
          float offsetx=props.get_float("offset_x_collider",0.0);
          float offsety=props.get_float("offset_y_collider",0.0);
          float padding=props.get_float("padding_collider",0.0);
          int8_t mat_id=props.get_int("physic_material",0);
          bool is_trigger=props.get_bool("is_trigger",false);
          bool has_sensor=props.get_bool("has_collider_sensor",false);
          float sensor_radious=props.get_float("sensor_radious",0.0);
          float offsetx_sensor=props.get_float("offset_x_sensor",0.0);
          float offsety_sensor=props.get_float("offset_y_sensor",0.0);
          std::string mask_sensor=props.get_string("sensor_mask","");
          std::string bits_sensor=props.get_string("sensor_categoryBits","");
          std::string mask_collision=props.get_string("collider_mask","");
          std::string bit_collision=props.get_string("collider_categoryBits","");
          std::string fig_type=props.get_string("figure_type","");
          collider_sys.register_collider(id_entity,mat_id,has_sensor,is_trigger,mask_collision,mask_sensor,bit_collision,bits_sensor,fig_type,radius,radius_sensor,w,h,offsetx,offsety,offsetx_sensor,offsety_sensor,padding);
          break;
       }
        case components_types::RIGIDBODY_COMP:{
          float mass=props.get_float("mass",1.0);
          float grav_scale=props.get_float("gravity_scale",1.0);
          float damp=props.get_float("damping_body",0.0);
          float angular_damping=props.get_float("angular_damping",0.0);
          bool is_kinematic=props.get_bool("is_kinematic",false);
          bool bd_active=props.get_bool("active_body",true);
          bool can_sleep=props.get_bool("bd_can_sleep",true);
          bool lock_rot=props.get_bool("lock_rotation",false);
          rigidbd_sys.register_body(id_entity,mass,grav_scale,damp,angular_damping,bd_active,is_kinematic,can_sleep,lock_rot);
          break;
       }
       case components_types::ANIMATOR_COMP:{
          std::string source_anim=props.get_string("source_animation","");
          anim_sys.register_Entity(id_entity,source_anim,&sys_textures);
          break;
       }
       case components_types::JOINT_COMP:{
           std::cout<<"joint detected"<<std::endl;
        }
       default:{
          break;
       }
    }
}

//Process only UI Components
void Scene::process_ui_component(components_types comp_type,const Entity_Props& props,uint16_t id_entity){

}

//Process Only Script Components
void Scene::process_script_component(components_types comp_type,const Entity_Props& props,uint16_t id_entity){
    switch(comp_type){
       case PLAYER_COMP:{
          player_sys.regiser_player(id_entity);
          break;
       }
       default:break;
    }
}

//Request Load a New Scene
void Scene::load_scene(int8_t index){
   if(index<0 || index>=list_scenes.size()){
       return;
   }
   next_scene=index;
}

//Load a Scene by Source
void Scene::load_sceneSource(SDL_Renderer* render_win,uint8_t load_mode){
   if(next_scene<0){
      return;
   }
   std::string next_scene_path=list_scenes[next_scene];
   Scene_Loader loader;
   loader.LoadSceneTiledXML(next_scene_path.c_str(),render_win,this,sys_tilemap,sys_textures,audio_sys,physic_material_sys);
   next_scene=-1;
   audio_sys.play_music(0,true,0);
}

//the Transform of a Entity Change
void Scene::transform_change(uint16_t id){
   if(entities_changes_transform==true){
        return;
   }
   entities_changes_transform=true;
}

//Return True if is Necessary Load a New Scene
bool Scene::require_loadScene(){
   if(next_scene<0){
       return false;
   }
   return true;
}

//call Collision Event On Script Components
void Scene::Execute_Collision_Ev(uint16_t entityA,uint16_t entityB,float normal_x,float normal_y,bool begin_ev){
    uint16_t mask_A=components_masks[entityA];
    uint16_t mask_B=components_masks[entityB];
    vector<components_types> list_targets={PLAYER_COMP};
    for(components_types target:list_targets){
        if(HasComponentBit(mask_A,target)==false){
            continue;
        }
        switch(target){
          case PLAYER_COMP:{
              player_sys.On_Collision(entityA,entityB,normal_x,normal_y,begin_ev);
              break;
          }
           default:break;
        }
    }
}

//call Trigger Event On Script Components
void Scene::Execute_Trigger_Ev(uint16_t entityA,uint16_t entityB,bool beginEv){
    uint16_t mask_A=components_masks[entityA];
    uint16_t mask_B=components_masks[entityB];
    vector<components_types> list_targets={PLAYER_COMP};
    for(components_types target:list_targets){
        if(HasComponentBit(mask_A,target)==false){
            continue;
        }

        switch(target){
           case PLAYER_COMP:{
              player_sys.On_Trigger(entityA,entityB,beginEv);
              break;
           }
           default:break;
        }
    }


}

//Set the Camera
void Scene::set_camera(Camera2D cam){
  main_camera=cam;
}

//Get Tilemap System
System_TileMap* Scene::get_system_tilemap(){
   return &sys_tilemap;
}

//Get the Camera of Scene
Camera2D* Scene::get_camera(){
  return &main_camera;
}

//Get Texture Manager System
textureManager_system* Scene::get_texture_system(){
   return &sys_textures;
}

//Get Audio System
Audio_System* Scene::get_Audio_sys(){
   return &audio_sys;
}

//Get Physic Material System
Physic_Material_System* Scene::get_physic_material_sys(){
  return &physic_material_sys;
}

//get the Number of Entities
uint16_t Scene:: get_count_entities(){
   return last_entity;
}

//get a component by the Name
template<typename T> T* Scene::get_component_byName(const char* name_comp,uint16_t id){
   components_types target_comp=ComponentName_to_ComponentIndex(name_comp);
   if(target_comp==UKNOW_COMP){
       return NULL;
   }
   if(id<0 || id>=last_entity){
         return NULL;
    }
   if(HasComponentBit(components_masks[id],target_comp)==false){
      return NULL;
   }
   switch(target_comp){
       case TRANSFORM_COMP:return &transf_sys;
       case RENDER_COMP:return &render_sys;
       default: return NULL;
   }
}

//get the mask of a Entity
uint32_t Scene::get_mask(uint16_t id){
    if(id<0 || id>=last_entity){
         return 0;
    }
    return components_masks[id];
}

//Get Transform System
Transform_System* Scene:: get_transform_sys(){
   return &transf_sys;
}

//Get Render System
Render_System* Scene::get_render_sys(){
    return &render_sys;
}

//Get Collider System
Collider_System* Scene::get_collider_sys(){
   return &collider_sys;
}

//Get Rigid Body System
RigidBody_System* Scene::get_rigidbody_sys(){
  return &rigidbd_sys;
}

//Get Animator System
Animator_System* Scene::get_animator_sys(){
   return &anim_sys;
}

//Get Player System
Player_System* Scene::get_player_sys(){
   return &player_sys;
}
