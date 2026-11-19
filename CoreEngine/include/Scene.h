#ifndef SCENE_H
#define SCENE_H
#include<Scene_Loader.h>
#include<Audio_System.h>
#include<Input.h>
#include<unordered_map>
#include<map>
#include<vector>
#include <Screen.h>
#include<Quatree.h>

class Render_DrawSystem;
class Physic_Engine_System;


//Indicate the way to Load the New Scene File
enum LOADSCENE_MODES:uint8_t{
   LOAD_APPEND=0,
   LOAD_REPLACE=1

};

//The Active Scene
struct Scene{
   public:
       bool Init(ma_engine * engine);
       void free_memory();
       void reserve_memory(int16_t num_elements,map<uint8_t,uint16_t> components_count);
       void update_scene(float dt);
       void load_sceneSource();
       void load_scene(int8_t index);
       bool require_loadScene();
       void register_entity(int16_t entity_Id,std::bitset<64> mask,const Entity_Props& props);
       void transform_change(uint16_t id);
       void Execute_Collision_Ev(uint16_t entityA,uint16_t entityB,float normal_x,float normal_y,bool begin_ev);
       void Execute_Trigger_Ev(uint16_t entityA,uint16_t entityB,bool beginEv);
       void set_camera(Camera2D cam);
       System_TileMap* get_system_tilemap();
       Camera2D* get_camera();
       textureManager_system* get_texture_system();
       Fonts_System* get_font_sys();
       Audio_System* get_Audio_sys();
       Physic_Material_System* get_physic_material_sys();
       uint16_t get_count_entities();
       template<typename T> T* get_component_byName(const char* name,uint16_t id);
       Transform_System* get_transform_sys();
       Render_System* get_render_sys();
       Collider_System* get_collider_sys();
       RigidBody_System* get_rigidbody_sys();
       Joint_System* get_joint_sys();
       UI_System* get_UI_sys();
       Player_System* get_player_sys();
       Animator_System* get_animator_sys();
       SpriteComposer_System* get_SpriteComposer_sys();
       Script_System* get_script_system();
       std::bitset<64> get_mask(uint16_t id);
       void remove_Jointcomponent(uint16_t id,uint8_t joint_type,bool require_destroy=false);
       Entity_TreeData* get_entity_treeData(uint16_t id);
    private:
        void process_target_component(components_types comp_type,const Entity_Props& props,uint16_t id_entity);
        void process_basic_component(components_types comp_type,const Entity_Props& props,uint16_t id_entity);
        void process_ui_component(components_types comp_type,const Entity_Props& props,uint16_t id_entity);
        void process_script_component(components_types comp_type,const Entity_Props& props,uint16_t id_entity);
        Render_System render_sys;
        Transform_System transf_sys;
        Collider_System collider_sys;
        RigidBody_System rigidbd_sys;
        Animator_System anim_sys;
        SpriteComposer_System spriteComposerSys;
        Joint_System join_sys;
        UI_System UI_sys;
        Player_System player_sys;
        System_TileMap sys_tilemap;
        Camera2D main_camera;
        textureManager_system sys_textures;
        Audio_System audio_sys;
        Script_System script_sys;
        Fonts_System fonts_sys;
        Physic_Material_System physic_material_sys;
        Quatree_System quatree_sys;
        std::vector<std::bitset<64>> components_masks;
        std::vector<std::string> list_scenes;
        std::vector<Entity_TreeData> entities_quatree_dat;
        int16_t last_entity;
        float threshold_updateQuatree_Entity;
        bool entities_changes_transform;
        int8_t next_scene;

};

#endif // SCENE_H
