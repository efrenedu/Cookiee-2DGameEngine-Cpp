#include "Scene.h"
#include "EngineContext.h"
#include "math_structs.h"

//Init the Scene  and Systems of It using Config Game Data
bool Scene::Init(ma_engine* engine){
   std::unordered_map<std::string,std::unordered_map<std::string,std::string>>game_config=EngineContext::get().get_config().get_allConfig();
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
   if(list_scenes.size()<=0){
      std::cout<<"Fail Getting Scenes"<<std::endl;
      return false;
   }
   if(script_sys.Init()==false){
      return false;
   }
   fonts_sys.Init();
   next_scene=0;
   audio_sys.Init_System(engine,music_volume,audio_volume);
   quatree_sys.Init(3);
   last_entity=0;
   entities_changes_transform=true;
   threshold_updateQuatree_Entity=0;
   return true;
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
   join_sys.free_system();
   spriteComposerSys.Free_Memory();
   UI_sys.Free_Memory();
   quatree_sys.clear_World();
   fonts_sys.free_System();
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
           case components_types::SPRITECOMPOSER_COMP:{
              spriteComposerSys.Reserve(components_count[i],num_elements,0xFFFF);
              break;
           }
           case components_types::UI_COMP:{
              UI_sys.Reserve_Memory(components_count[i],num_elements,0xFFFF);
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
void Scene::update_scene(float dt){
   if(dt>1.0){
      return;
   }
   UI_sys.update(dt,ui_entities);
   audio_sys.update_system();
   anim_sys.update_system(dt);
   UI_sys.update_Widgets_Sprites(this,ui_entities);

   uint8_t draw_request=REDRAW_NONE;
   //update entities scripts
   std::vector<uint16_t> near_camera=main_camera.get_NearEntities();
   for(uint16_t index:near_camera){
      std::bitset<64> mask=components_masks[index];
      if(HasComponentBit(mask,RIGIDBODY_COMP)){
          RigidBody_Data*rb=rigidbd_sys.get_body(index);
          if(rb->b2bodyPtr!=NULL){
            if(rb->active==true && rb->b2bodyPtr->IsEnabled()==false){
                 rb->b2bodyPtr->SetEnabled(true);
            }
          }

      }
      if(HasComponentBit(mask,RENDER_COMP)){
          Render_data* render_dat=render_sys.get_render(index);
          if(render_dat->shadow_data.use_shadow){

          }
      }
      if(HasComponentBit(mask,PLAYER_COMP)){
           player_sys.update_player(mask,index,this,dt);
      }
      if(HasComponentBit(mask,ANIMATOR_COMP) && HasComponentBit(mask,RENDER_COMP)){
            Render_data* render=render_sys.get_render(index);
            Animator_Data* anim=anim_sys.get_animator(index);
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
   if(quatree_sys.Process_Reassign(this)==true){
       main_camera.get_entities_Quatree(&quatree_sys);
   }
   main_camera.follow_target(this);
   if(main_camera.hasMoved_significantly()){
       main_camera.update_renderPos();
       draw_request=REDRAW_ALL;
       entities_changes_transform=false;
   }
   else if(entities_changes_transform){
      draw_request=REDRAW_ENTITIES;
      entities_changes_transform=false;
   }
   EngineContext::get().get_drawContext().request_redraw(draw_request);
}

//Register the Entity in each Component System Requerid
void Scene::register_entity(int16_t entity_Id,std::bitset<64> mask,const Entity_Props& props){
     components_masks.push_back(mask);
     bool is_ui=false;
     for (uint8_t i=0;i<COMP_COUNT;i++){
           components_types type_target=static_cast<components_types>(i);
           if(HasComponentBit(mask,type_target)){
               process_target_component(type_target,props,entity_Id);
           }
     }
     Entity_TreeData tree_dat;
     Transform_Data* transf=transf_sys.get_transform(last_entity);
     tree_dat.last_x=transf->x;
     tree_dat.last_y=transf->y;
     entities_quatree_dat.push_back(tree_dat);
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
    else if(comp_type==UI_COMP){
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
          shadow_projection shadow_data;
          int16_t z_index=props.get_int("z_index",0);
          uint8_t draw_mode=props.get_uint("draw_mode",0);
          bool visible=props.get_bool("visible",true);
          uint16_t gid=props.get_uint("gid",0xFFFF)-1;
          render_sys.register_Entity(visible,draw_mode,gid,z_index,id_entity,shadow_data);
          break;
       }
       case components_types::COLLIDER_COMP:{
          uint8_t count_addiontalFixtures=props.get_uint("Count_Add_Fixtures",0);
          std::vector<fixture_Data> additional_fixtures;
          additional_fixtures.reserve(count_addiontalFixtures);
          for(uint8_t i=0;i<count_addiontalFixtures;i++){
             fixture_Data new_fixture;
             std::string index_str=to_string(static_cast<uint16_t>(i));
             std::vector<std::string> bit_fixture_list=Converter::str_toVect(props.get_string("Fixture_Category_Bits_"+index_str,""));
             new_fixture.mask_fixture= props.get_uint("Fixture_Mask_"+index_str,0);
             uint16_t category_bits_fixture=0;
             for(std::string value:bit_fixture_list){
                  category_bits_fixture|=Converter::strTo_UInt(value,0);
             }
             new_fixture.category_bits_fixture=category_bits_fixture;
             new_fixture.radius=props.get_float("Fixture_Radius_"+index_str,0);
             new_fixture.offsetX=props.get_float("Fixture_OffsetX_"+index_str,0);
             new_fixture.offsetY=props.get_float("Fixture_OffsetY_"+index_str,0);
             new_fixture.is_sensor=props.get_bool("Fixture_Sensor_"+index_str,false);
             additional_fixtures.push_back(new_fixture);
          }
          float radius=props.get_float("radius_collider",0.0);
          float w=props.get_float("width_collider",0.0);
          float h=props.get_float("height_collider",0.0);
          float offsetx=props.get_float("offset_x_collider",0.0);
          float offsety=props.get_float("offset_y_collider",0.0);
          float padding=props.get_float("padding_collider",0.0);
          int8_t mat_id=props.get_int("physic_material",0);
          bool is_trigger=props.get_bool("is_trigger",false);
          uint16_t mask_collision=props.get_uint("collider_mask",0);
          uint16_t bits=0;
          std::vector<std::string> bit_list=Converter::str_toVect(props.get_string("collider_categoryBits",""));
          for(std::string val:bit_list){
             bits|=Converter::strTo_UInt(val,0);
          }
          uint8_t fig_type=props.get_uint("figure_type",FIGURE_NONE);
          std::vector<Vertex_Polygon> vertexs;
          uint8_t vertex_count=props.get_uint("Vertex_Collider_Count",0);
          vertexs.reserve(vertex_count);
          for(uint8_t i=0;i<vertex_count;i++){
            std::string index_str=to_string(static_cast<uint16_t>(i));
            Vertex_Polygon vertex;
            vertex.point_x=props.get_float("Vertex_Collider_PointX_"+index_str,0.0);
            vertex.point_y=props.get_float("Vertex_Collider_PointY_"+index_str,0.0);
            vertexs.push_back(vertex);
          }
          collider_sys.register_collider(id_entity,mat_id,is_trigger,mask_collision,bits,fig_type,radius,w,h,offsetx,offsety,padding,vertexs,additional_fixtures);
          break;
       }
        case components_types::RIGIDBODY_COMP:{
          float mass=props.get_float("mass",1.0);
          float grav_scale=props.get_float("gravity_scale",1.0);
          float damp=props.get_float("damping_body",0.0);
          float angular_damping=props.get_float("angular_damping",0.0);
          uint8_t bd_type=props.get_uint("body_type",BODY_DINAMIC);
          bool bd_active=props.get_bool("active_body",true);
          bool can_sleep=props.get_bool("bd_can_sleep",true);
          bool lock_rot=props.get_bool("lock_rotation",false);
          rigidbd_sys.register_body(id_entity,mass,grav_scale,damp,angular_damping,bd_active,bd_type,can_sleep,lock_rot);
          break;
       }
       case components_types::ANIMATOR_COMP:{
          std::string source_anim=props.get_string("source_animation","");
          anim_sys.register_Entity(id_entity,source_anim,&sys_textures);
          break;
       }
       case components_types::SPRITECOMPOSER_COMP:{
          uint16_t number_sprites=props.get_uint("SpriteComposer_Sprites_Count",0);
          std::vector<SpriteComposer_Item> list_sprites;
          list_sprites.reserve(number_sprites);
          for(uint16_t i=0;i<number_sprites;i++){
             std::string index_str=to_string(i);
             uint16_t sprite_id=props.get_uint("SpriteComposer_Id_Sprite_"+index_str,0xFFFF);
             if(sprite_id>0){
                sprite_id=sprite_id-1;
             }
             float offsetX=props.get_float("SpriteComposer_OffsetX_Sprite_"+index_str,0);
             float offsetY=props.get_float("SpriteComposer_OffsetY_Sprite_"+index_str,0);
             uint8_t draw_mod=props.get_uint("SpriteComposer_DrawMode_Sprite_"+index_str,RENDER_MODES::NORMAL_MODE);
             SpriteComposer_Item next_sprite;
             next_sprite.offsetX=offsetX;
             next_sprite.offsetY=offsetY;
             next_sprite.id_sprite=sprite_id;
             next_sprite.draw_mode=draw_mod;
             list_sprites.push_back(next_sprite);

          }
          spriteComposerSys.Regiser_Entity(id_entity,list_sprites);
          break;
       }
       case components_types::JOINT_COMP:{
           uint8_t joint_count=props.get_uint("Joint_Count",0);
           for(uint8_t i=0;i<joint_count;i++){
              std::string index_str=to_string(static_cast<uint16_t>(i));
              uint8_t joint_t=get_JointType(props.get_string("joint_type_"+index_str,"").c_str());
              switch(joint_t){
                 case DISTANCE_JOINT:{
                    float min_dist=props.get_float("Min_Distance_Joint_"+index_str,0.0);
                    float max_dist=props.get_float("Max_Distance_Joint_"+index_str,0.0);
                    float stiff=props.get_float("Stiffness_Joint_"+index_str,0.0);
                    float damp=props.get_float("Damping_Joint_"+index_str,0.0);
                    float anchorA_x=props.get_float("AnchorA_X_Joint_"+index_str,0.0);
                    float anchorA_y=props.get_float("AnchorA_Y_Joint_"+index_str,0.0);
                    float anchorB_x=props.get_float("AnchorB_X_Joint_"+index_str,0.0);
                    float anchorB_y=props.get_float("AnchorB_Y_Joint_"+index_str,0.0);
                    bool collider_connect=props.get_bool("Collide_Connect_Join_"+index_str,true);
                    bool removeOn_Build=props.get_bool("RemoveOn_Build_Joint_"+index_str,true);
                    uint16_t target=props.get_uint("Joint_Target_"+index_str,0xFFFF);
                    //Convert AnchorA from Anchor Direction Vector Normalized to HalfSize Direction Vector
                    join_sys.register_distance_joint(id_entity,target,removeOn_Build,collider_connect,min_dist,max_dist,stiff,damp,anchorA_x,anchorA_y,anchorB_x,anchorB_y);
                    break;
                 }
                 case REVOLUTE_JOINT:{
                    bool collider_connect=props.get_bool("Collide_Connect_Join_"+index_str,true);
                    float ref_angle=props.get_float("Angle_Reference_Joint_"+index_str,0.0f);
                    float motor_speed=props.get_float("Motor_Speed_Joint_"+index_str,0.0);
                    float max_torque=props.get_float("Max_Torque_Joint_"+index_str,0.0);
                    float lower_angle=props.get_float("Lower_Angle_Joint_"+index_str,0.0);
                    float upper_angle=props.get_float("Upper_Angle_Joint_"+index_str,0.0);
                    bool enable_limits=props.get_bool("Enable_Limits_Joint_"+index_str,true);
                    bool enable_motor=props.get_bool("Enable_Motor_Joint_"+index_str,true);
                    uint16_t target=props.get_uint("Joint_Target_"+index_str,0xFFFF);
                    bool removeOn_Build=props.get_bool("RemoveOn_Build_Joint_"+index_str,true);
                    join_sys.register_revolute_joint(id_entity,target,removeOn_Build,collider_connect,ref_angle,lower_angle,upper_angle,motor_speed,max_torque,enable_motor,enable_limits);
                    break;
                 }
                 case PRISMATIC_JOINT:{
                    float angle_ref=props.get_float("Angle_Reference_Joint_"+index_str,0.0);
                    bool collider_connect=props.get_bool("Collide_Connect_Join_"+index_str,true);
                    float motor_speed=props.get_float("Motor_Speed_Joint_"+index_str,0.0);
                    float max_force=props.get_float("Max_Force_Joint_"+index_str,0.0);
                    float lower_translation=props.get_float("Lower_Translation_Joint_"+index_str,0.0);
                    float upper_translation=props.get_float("Upper_Translation_Joint_"+index_str,0.0);
                    float axis_x=props.get_float("AxisX_Joint_"+index_str,0.0);
                    float axis_y=props.get_float("AxisY_Joint_"+index_str,0.0);
                    bool enable_limits=props.get_bool("Enable_Limits_Joint_"+index_str,true);
                    bool enable_motor=props.get_bool("Enable_Motor_Joint_"+index_str,true);
                    uint16_t target=props.get_uint("Joint_Target_"+index_str,0xFFFF);
                    bool removeOn_Build=props.get_bool("RemoveOn_Build_Joint_"+index_str,true);

                    join_sys.register_prismatic_joint(id_entity,target,removeOn_Build,collider_connect,axis_x,axis_y,angle_ref,lower_translation,upper_translation,motor_speed,max_force,enable_motor,enable_limits);
                    break;
                 }
                 case WELD_JOINT:{
                    bool collider_connect=props.get_bool("Collide_Connect_Join_"+index_str,true);
                    float stiffnes=props.get_float("Stiffnes_Joint_"+index_str,0.0);
                    float damping=props.get_float("Damping_Joint_"+index_str,0.0);
                    float angle_ref=props.get_float("Angle_Reference_Joint_"+index_str,0.0);
                    bool removeOn_Build=props.get_bool("RemoveOn_Build_Joint_"+index_str,true);
                    uint16_t target=props.get_uint("Joint_Target_"+index_str,0xFFFF);
                    join_sys.register_weld_joint(id_entity,target,removeOn_Build,collider_connect,stiffnes,damping,angle_ref);
                    break;
                 }
                 case PULLEY_JOINT:{
                    bool collider_connect=props.get_bool("Collide_Connect_Join_"+index_str,true);
                    uint16_t target=props.get_uint("Joint_Target_"+index_str,0xFFFF);
                    float groundA_offsetY=props.get_float("GroundA_OffsetY_Joint_"+index_str,0.0);
                    float groundB_offsetY=props.get_float("GroundB_OffsetY_Joint_"+index_str,0.0);
                    float groundA_offsetX=props.get_float("GroundA_OffsetX_Joint_"+index_str,0.0);
                    float groundB_offsetX=props.get_float("GroundB_OffsetX_Joint_"+index_str,0.0);
                    bool removeOn_Build=props.get_bool("RemoveOn_Build_Joint_"+index_str,true);
                    float ratio_joint=props.get_float("Ratio_Joint_"+index_str,1.0);
                    join_sys.register_pulley_joint(id_entity,target,removeOn_Build,collider_connect,ratio_joint,groundA_offsetX,groundA_offsetY,groundB_offsetX,groundB_offsetY);
                    break;
                 }
                 default:break;
              }
           }

        }
       default:{
          break;
       }
    }
}

//Process only UI Components
void Scene::process_ui_component(components_types comp_type,const Entity_Props& props,uint16_t id_entity){
   uint8_t ui_count=props.get_uint("UI_Count",0);
   UI_header header;

   header.active_state=props.get_uint("UI_Active_State",UI_ACTIVE_STATE_ACTIVE);
   header.centerX=props.get_int("UI_CenterX",0.0);
   header.centerY=props.get_int("UI_CenterY",0.0);
   header.half_width=props.get_uint("UI_HalfWidth",0.0);
   header.half_height=props.get_uint("UI_HalfHeight",0.0);
   header.id_panel=props.get_uint("UI_Parent_Panel",0xFFFF);

   uint8_t main_type=UI_NONE;
   for(uint8_t i=0;i<ui_count;i++){
      std::string str_index=to_string(i);
      uint8_t ui_type=props.get_uint("UI_type_"+str_index,UI_NONE);

      switch(ui_type){
          case UI_BUTTON:{
               main_type=UI_BUTTON;
               std::vector<uint16_t> colors;
               std::vector<uint16_t> colors_labels;
               std::vector<uint16_t> sprites;
               std::vector<float> count_timers;
               std::vector<float> max_wait_timers;
               bool is_animated=props.get_bool("UI_Button_Animated",false);;
               uint8_t states_count=4;
               uint8_t num_timers=4;
               colors.assign(states_count,0);
               sprites.assign(states_count,0xFFFF);
               colors_labels.assign(states_count,0);
               count_timers.assign(num_timers,0.0);
               max_wait_timers.assign(num_timers,0.0);
               for(uint8_t i=0;i<num_timers;i++){
                  std::string id_str=to_string(i);
                  max_wait_timers[i]=props.get_float("UI_Button_Timer_"+id_str,0.0);
               }
               Button_Event button_evs;
               button_evs.callfunction=props.get_string("UI_Button_CallFunction","");
               button_evs.use_keyInput=props.get_bool("UI_Button_Use_Keys_Input",false);
               button_evs.trigger_key=props.get_uint("UI_Button_Trigger_Key",0xFF);
               button_evs.selected=props.get_bool("UI_Button_Initial_Selected",false);
               button_evs.arg_func=props.get_float("UI_Button_Arg_Function",0);
               button_evs.always_selected=props.get_bool("UI_Button_Always_Selected",false);
               button_evs.recent_interact=false;
               button_evs.wait_timers=max_wait_timers;
               button_evs.count_timers=count_timers;
               button_evs.show_cooldown_time=props.get_bool("UI_Button_Show_Cooldown_Time",false);

               for(uint8_t i=0;i<states_count;i++){
                   std::string id_str=to_string(i);
                   colors[i]=Converter::str_to_colorPacked(props.get_string("UI_Button_Color_"+id_str,""));
                   colors_labels[i]=Converter::str_to_colorPacked(props.get_string("UI_Button_Label_Color_"+id_str,""));
                   sprites[i]=props.get_uint("UI_Button_Sprite_"+id_str,0xFFFF);
               }
               UI_sys.register_button(id_entity,colors,sprites,colors_labels,is_animated,button_evs);
               break;
          }
          case UI_LABEL:{
             if(main_type>=0xFF){
                 main_type=UI_LABEL;
             }
             int16_t offset_x=props.get_int("UI_Label_Offset_X",0);
             int16_t offset_y=props.get_int("UI_Label_Offset_Y",0);
             uint8_t font_index=props.get_uint("Font_Id",0xFF);
             bool single_line=props.get_bool("UI_Label_Single_Line",true);
             uint8_t align=props.get_uint("UI_Label_Align",LABEL_ALIGN_LEFT);
             uint16_t text_box_w=props.get_uint("UI_Label_Text_Box_Width",header.half_width*2);
             uint16_t text_box_h=props.get_uint("UI_Label_Text_Box_Height",header.half_height*2);
             std::string text=props.get_string("UI_Label_Text","");
             uint16_t color_text=0;
             std::vector<std::string> color_vect=Converter::str_toVect(props.get_string("UI_Labet_Text_Color",""));
             if(color_vect.size()>0){

                uint8_t r=Converter::strTo_UInt(color_vect[0]);
                uint8_t g=Converter::strTo_UInt(color_vect[1]);
                uint8_t b=Converter::strTo_UInt(color_vect[2]);
                uint8_t a=Converter::strTo_UInt(color_vect[3]);
                color_text=Converter::packRGBA4444(r,g,b,a);

             }
             UI_sys.register_label(id_entity,text,color_text,align,text_box_w,text_box_h,single_line,offset_x,offset_y,font_index);
             break;
          }
          case UI_VALUEBAR:{
             main_type=UI_VALUEBAR;
             uint8_t orientation=props.get_uint("UI_ValueBar_Orientation",UI_ORIENTATION_HORIZONTAL);
             uint16_t max_width=props.get_uint("UI_ValueBar_Max_Width",0);
             uint16_t max_height=props.get_uint("UI_ValueBar_Max_Height",0);
             uint16_t width_bar=props.get_uint("UI_ValueBar_Width",max_width);
             uint16_t heigh_bar=props.get_uint("UI_ValueBar_Height",max_height);
             float initial_val=props.get_float("UI_ValueBar_Value",1.0);
             bool use_bar_colors=props.get_bool("UI_ValueBar_Use_Bar_Colors",true);
             uint16_t border_val=props.get_uint("UI_ValueBar_Border",0);
             uint16_t bg_color;
             uint8_t bar_states_count=3;
             uint8_t states_count=4;
             std::vector<uint16_t> bar_sprites;
             std::vector<uint16_t> bar_colors;
             std::vector<uint16_t> bg_sprites;
             bar_sprites.assign(bar_states_count,0xFFFF);
             bar_colors.assign(bar_states_count,0);
             bg_sprites.assign(bar_states_count,0xFFFF);
             for(uint8_t i=0;i<states_count;i++){
                  std::string id_str=to_string(i);
                  std::vector<std::string> color_vect=Converter::str_toVect(props.get_string("UI_ValueBar_Color_"+id_str,""));
                  uint16_t temp_color=0;
                  if(i<3){
                      uint16_t temp_sprite=props.get_uint("UI_ValueBar_Bar_Sprite_"+id_str,0xFFFF);
                      bar_sprites[i]=temp_sprite;
                      uint16_t temp_bg_sprite=props.get_uint("UI_ValueBar_Bg_Sprite_"+id_str,0xFFFF);
                      bg_sprites[i]=temp_bg_sprite;
                  }
                  if(color_vect.size()>=4){
                      uint8_t r,g,b,a;
                      r=Converter::strTo_UInt(color_vect[0]);
                      g=Converter::strTo_UInt(color_vect[1]);
                      b=Converter::strTo_UInt(color_vect[2]);
                      a=Converter::strTo_UInt(color_vect[3]);
                      temp_color=Converter::packRGBA4444(r,g,b,a);
                  }
                  switch(i){
                     case 0: bar_colors[0]=temp_color;break;
                     case 1: bar_colors[1]=temp_color;break;
                     case 2: bar_colors[2]=temp_color;break;
                     case 3: bg_color=temp_color;break;
                     default:break;
                  }

             }
             UI_sys.register_valueBar(id_entity,initial_val,bar_colors,bar_sprites,border_val,bg_color,bg_sprites,max_width,max_height,orientation,use_bar_colors,width_bar,heigh_bar);
             break;
          }
          case UI_SLIDER:{
             main_type=UI_SLIDER;
             uint8_t orientation=props.get_uint("UI_Slider_Orientation",UI_ORIENTATION_HORIZONTAL);
             uint16_t max_width=props.get_uint("UI_Slider_Max_Width",0);
             uint16_t max_height=props.get_uint("UI_Slider_Max_Height",0);
             float initial_val=props.get_float("UI_Slider_Value",1.0);
             std::vector<uint16_t> bg_colors;
             std::vector<uint16_t> slider_colors;
             std::vector<uint16_t> bar_colors;
             std::vector<uint16_t>slider_sprites;
             bg_colors.assign(2,0);
             bar_colors.assign(2,0);
             slider_colors.assign(4,0);
             slider_sprites.assign(4,0);
             uint8_t colors_count=8;

             std::string callFunction=props.get_string("UI_Slider_CallFunction","");
             uint8_t upValue_key=props.get_uint("UI_Slider_UpValue_key",0xFF);
             uint8_t downValue_key=props.get_uint("UI_Slider_DownValue_key",0xFF);
             bool useKey_Inputs=props.get_bool("UI_Slider_Use_Keys_Inputs",false);
             bool initial_selected=props.get_bool("UI_Slider_Initial_Selected",false);
             float step_val=props.get_float("UI_Slider_Step",1.0);

             Slider_Event slider_ev;
             slider_ev.callFunction=callFunction;
             slider_ev.key_downValue=downValue_key;
             slider_ev.key_upValue=upValue_key;
             slider_ev.selected=initial_selected;
             slider_ev.useKeys=useKey_Inputs;
             slider_ev.step_val=step_val;
             slider_ev.dragging=false;

             for(uint8_t i=0;i<colors_count;i++){
                  std::string id_str=to_string(i);
                  std::vector<std::string> color_vect=Converter::str_toVect(props.get_string("UI_Slider_Color_"+id_str,""));
                  uint16_t temp_color=0;
                  if(i<4){
                       slider_sprites[i]=props.get_uint("UI_Slider_Slider_Sprite_"+id_str,0xFFFF);
                  }
                  if(color_vect.size()>=4){
                      uint8_t r,g,b,a;
                      r=Converter::strTo_UInt(color_vect[0]);
                      g=Converter::strTo_UInt(color_vect[1]);
                      b=Converter::strTo_UInt(color_vect[2]);
                      a=Converter::strTo_UInt(color_vect[3]);
                      temp_color=Converter::packRGBA4444(r,g,b,a);
                  }
                  switch(i){
                     case 0: bg_colors[0]=temp_color;break;
                     case 1: bar_colors[0]=temp_color;break;
                     case 2: slider_colors[0]=temp_color;break;
                     case 3: slider_colors[1]=temp_color;break;
                     case 4: slider_colors[2]=temp_color;break;
                     case 5: slider_colors[3]=temp_color;break;
                     case 6: bg_colors[1]=temp_color;break;
                     case 7: bar_colors[1]=temp_color;break;

                     default:break;
                  }
             }
             UI_sys.register_slider(id_entity,initial_val,bar_colors,bg_colors,slider_colors,slider_sprites,max_width,max_height,orientation,slider_ev);
             break;
          }
          case UI_SCROLLBAR:{
             uint16_t default_w=header.half_width*2;
             uint16_t default_h=header.half_height*2;
             uint16_t area_w=props.get_uint("UI_ScrollBar_Area_Width",default_w);
             uint16_t area_h=props.get_uint("UI_ScrollBar_Area_Height",default_h);
             uint16_t bg_w=props.get_uint("UI_ScrollBar_Bg_Width",default_w);
             uint16_t bg_h=props.get_uint("UI_ScrollBar_Bg_Height",default_h);
             float porcentSz_buttons=props.get_float("UI_ScrollBar_Porcent_Buttons",1.0);
             uint8_t orientation=props.get_uint("UI_ScrollBar_Orientation",UI_ORIENTATION_VERTICAL);

             uint8_t upValue_key=props.get_uint("UI_ScrollBar_UpValue_key",0xFF);
             uint8_t downValue_key=props.get_uint("UI_ScrollBar_DownValue_key",0xFF);
             bool useKey_Inputs=props.get_bool("UI_ScrollBar_Use_Keys_Inputs",false);
             bool initial_selected=props.get_bool("UI_ScrollBar_Initial_Selected",false);
             float step_val=props.get_float("UI_ScrollBar_Step",1.0);
             ScrollBar_Event scroll_ev;
             scroll_ev.downValue_key=downValue_key;
             scroll_ev.upValue_key=upValue_key;
             scroll_ev.is_selected=initial_selected;
             scroll_ev.step=step_val;
             scroll_ev.use_keysInput=useKey_Inputs;
             scroll_ev.dragging=false;
             uint8_t color_count=6;
             std::vector<uint16_t> colors_bar;
             colors_bar.assign(color_count-2,0);
             uint16_t color_bg=0;
             uint16_t color_btns=0;
             for(uint8_t i=0;i<color_count;i++){
                std::string str_index=to_string(i);
                std::vector<std::string> colors_vect=Converter::str_toVect(props.get_string("UI_ScrollBar_Color_"+str_index,""));
                if(colors_vect.size()>=4){
                    uint8_t r=Converter::strTo_UInt(colors_vect[0]);
                    uint8_t g=Converter::strTo_UInt(colors_vect[1]);
                    uint8_t b=Converter::strTo_UInt(colors_vect[2]);
                    uint8_t a=Converter::strTo_UInt(colors_vect[3]);
                    uint16_t temp_color=Converter::packRGBA4444(r,g,b,a);
                    if(i==0){
                        color_bg=temp_color;
                    }
                    else if(i==1){
                        color_btns=temp_color;
                    }
                    else{
                        colors_bar[i-2]=temp_color;
                    }
                }
             }
             uint16_t sprite_btns=props.get_uint("UI_ScrollBar_Buttons_Sprite",0xFFFF);

             UI_sys.register_scrollBar(id_entity,bg_w,bg_h,area_w,area_h,color_bg,colors_bar,orientation,porcentSz_buttons,sprite_btns,color_btns,scroll_ev);
             break;
          }
          case UI_RADIOBUTTON:{
             main_type=UI_RADIOBUTTON;
             Radio_Button_Visual visual;
             Radio_Button_Data data;
             Radio_Button_Event ev_data;
             data.state=props.get_uint("UI_Radio_Button_State",OPTION_UNCHECKED);
             std::vector<uint16_t> sprites;
             std::vector<uint16_t> colors;
             std::vector<uint16_t> text_colors;
             sprites.assign(4,0xFFFF);
             colors.assign(4,0);
             text_colors.assign(4,0);
             uint8_t states_count=4;
             for(uint8_t i=0;i<states_count;i++){
                std::string index_str=to_string(i);
                uint16_t spr_state=props.get_uint("UI_Radio_Button_SpriteState_"+index_str,0xFFFF);
                uint16_t temp_color=Converter::str_to_colorPacked(props.get_string("UI_Radio_Button_Color_"+index_str,""));
                uint16_t temp_color_text=Converter::str_to_colorPacked(props.get_string("UI_Radio_Button_Text_Color_"+index_str,""));
                text_colors[i]=temp_color_text;
                colors[i]=temp_color;
                sprites[i]=spr_state;

             }
             visual.colors=colors;
             visual.text_colors=text_colors;
             visual.sprites_states=sprites;

             data.radioGroup_id=props.get_uint("UI_Radio_Button_RadioGroup_Id",0);
             ev_data.nextBtn_key=props.get_uint("UI_Radio_Button_Next_Option_Key",0xFF);
             ev_data.previewBtn_key=props.get_uint("UI_Radio_Preview_Option_Key",0xFF);
             ev_data.callfunction=props.get_string("UI_Radio_Button_CallFunction","");
             ev_data.selected=props.get_bool("UI_Radio_Button_Initial_Selected",false);
             ev_data.useKeys_Input=props.get_bool("UI_Radio_Button_Use_Key_Inputs",false);

             UI_sys.register_RadioButton(id_entity,visual,data,ev_data);

             break;
          }
          case UI_CHECKBUTTON:{
              main_type=UI_CHECKBUTTON;
              uint8_t initial_state=props.get_uint("UI_Check_Button_State",OPTION_UNCHECKED);
              std::vector<uint16_t> sprites;
              std::vector<uint16_t> colors;
              std::vector<uint16_t> colors_text;
              CheckButton_Event check_evs;
              check_evs.callfunction=props.get_string("UI_Check_Button_CallFunction","");
              check_evs.nextBtn_key=props.get_uint("UI_Check_Button_NextOption_Key",0xFF);
              check_evs.previewBtn_key=props.get_uint("UI_Check_Button_PreviewOption_Key",0xFF);
              check_evs.trigget_key=props.get_uint("UI_Check_Button_Trigger_Key",0xFF);
              check_evs.useKeys_Input=props.get_bool("UI_Check_Button_Use_Inputs_Keys",false);
              check_evs.selected=props.get_bool("UI_Check_Button_Initial_Selected",false);
              check_evs.preview_focus=props.get_uint("UI_Check_Button_Preview_FocusId",0xFFFF);
              check_evs.next_focus=props.get_uint("UI_Check_Button_Next_FocusId",0xFFFF);

              sprites.assign(4,0xFFFF);
              colors.assign(4,0);
              colors_text.assign(4,0);
              uint8_t states_count=4;
              for(uint8_t i=0;i<states_count;i++){
                std::string index_str=to_string(i);
                uint16_t spr_state=props.get_uint("UI_Check_Button_SpriteState_"+index_str,0xFFFF);
                uint16_t temp_color=Converter::str_to_colorPacked(props.get_string("UI_Check_Button_Color_"+index_str,""));
                uint16_t temp_color_text=Converter::str_to_colorPacked(props.get_string("UI_Check_Button_Color_Text_"+index_str,""));
                colors[i]=temp_color;
                sprites[i]=spr_state;
                colors_text[i]=temp_color_text;
              }
              UI_sys.register_checkButton(id_entity,colors,colors_text,sprites,initial_state,check_evs);
              break;
          }
          case UI_PANEL:{
              main_type=UI_PANEL;
              std::vector<std::string> bg_color_vect=Converter::str_toVect(props.get_string("UI_Panel_Bg_Color",""));
              std::vector<std::string> border_color_vect=Converter::str_toVect(props.get_string("UI_Panel_Border_Color",""));
              uint16_t bg_color=0;
              uint16_t border_color=0;
              Panel_Events panel_ev;
              panel_ev.callfunction=props.get_string("UI_Panel_CallFunction","");
              panel_ev.closeCallFunction=props.get_string("UI_Panel_Close_CallFunction","");
              panel_ev.close_key=props.get_uint("UI_Panel_Close_Key",0xFF);
              panel_ev.open_key=props.get_uint("UI_Panel_Open_Key",0xFF);
              panel_ev.recent_open=false;
              panel_ev.recent_close=false;
              if(header.active_state==UI_ACTIVE_STATE_ACTIVE){
                 panel_ev.recent_open=true;
              }
              else{
                 panel_ev.recent_close=true;
              }
              panel_ev.useKey_Inputs=props.get_bool("UI_Panel_Use_Keys_Input",false);
              panel_ev.arg_function=props.get_float("UI_Panel_Event_Arg",0);
              panel_ev.isSelected=props.get_bool("UI_Panel_Initial_Selected",false);

              if(bg_color_vect.size()>=4){
                uint8_t r=Converter::strTo_UInt(bg_color_vect[0]);
                uint8_t g=Converter::strTo_UInt(bg_color_vect[1]);
                uint8_t b=Converter::strTo_UInt(bg_color_vect[2]);
                uint8_t a=Converter::strTo_UInt(bg_color_vect[3]);
                bg_color=Converter::packRGBA4444(r,g,b,a);
              }
              if(border_color_vect.size()>=4){
                  uint8_t r=Converter::strTo_UInt(border_color_vect[0]);
                  uint8_t g=Converter::strTo_UInt(border_color_vect[1]);
                  uint8_t b=Converter::strTo_UInt(border_color_vect[2]);
                  uint8_t a=Converter::strTo_UInt(border_color_vect[3]);
                  border_color=Converter::packRGBA4444(r,g,b,a);

              }
              bool draw_border=props.get_bool("UI_Panel_Draw_Border",false);
              UI_sys.register_panel(id_entity,draw_border,bg_color,border_color,panel_ev);

              break;
          }
          case UI_ICON:{
             main_type=UI_ICON;
             uint8_t num_sprites=props.get_uint("UI_Icon_Sprites_Count",0);
             std::vector<uint16_t> list_sprites;
             for(uint8_t i=0;i<num_sprites;i++){
                  std::string str_index=to_string(i);
                  uint16_t next_spr=props.get_uint("UI_Icon_Sprite_"+str_index,0xFFFF);
                  list_sprites.push_back(next_spr);
             }
             bool can_modify_sprite=props.get_bool("UI_Icon_Modify_Sprite",true);
             UI_sys.register_Icon(id_entity,list_sprites,can_modify_sprite);
             break;
          }
          case UI_COMBOBOX:{
              main_type=UI_COMBOBOX;
              ComboBox_Visual visual;
              ComboBox_layout layout;
              ComboBox_Event box_ev;

              std::vector<std::string> items;
              std::vector<uint16_t> colors_items;
              std::vector<uint16_t> colors_bg;
              std::vector<uint16_t> colors_buttons;
              std::vector<uint16_t> sprites_buttons;
              colors_items.assign(4,0);
              sprites_buttons.assign(2,0xFFFF);
              colors_bg.assign(2,0);
              colors_buttons.assign(2,0);

              layout.max_showItems=props.get_uint("UI_ComboxBox_Max_Items",10);
              layout.width_box=props.get_uint("UI_ComboBox_Width",header.half_width*2);
              layout.min_height=props.get_uint("UI_ComboBox_Min_Height",header.half_height*2);
              layout.max_height=props.get_uint("UI_ComboBox_Max_Height",header.half_height*2);
              layout.porceSize_btn=props.get_float("UI_ComboBox_Button_Size_Porcent",1.0);
              uint8_t items_count=props.get_uint("UI_ComboBox_Items_Count",0);

              box_ev.callfunction=props.get_string("UI_ComboBox_CallFunction","");
              box_ev.useKeys_Input=props.get_bool("UI_ComboBox_Use_Keys_Inputs",false);
              box_ev.selected=props.get_bool("UI_ComboBox_Initial_Selected",false);
              box_ev.Up_key=props.get_uint("UI_ComboBox_Up_key",0xFF);
              box_ev.Down_key=props.get_uint("UI_ComboBox_Down_key",0xFF);
              box_ev.trigget_key=props.get_uint("UI_ComboBox_Trigger_key",0xFF);

              for(uint8_t i=0;i<items_count;i++){
                  std::string str_index=to_string(i);
                  std::string val_item=props.get_string("UI_ComboBox_Item_"+str_index,"");
                  items.push_back(val_item);
              }

              uint8_t color_count=9;
              for(uint8_t i=0;i<color_count;i++){
                 std::string str_index=to_string(i);
                 uint16_t temp_color=0;
                 std::vector<std::string> color_vect=Converter::str_toVect(props.get_string("UI_ComboBox_Color_"+str_index,""));
                 if(i<2){
                      sprites_buttons[i]=props.get_uint("UI_ComboBox_Button_Sprite_"+str_index,0xFFFF);
                 }
                 if(color_vect.size()>=4){
                    uint8_t r=Converter::strTo_UInt(color_vect[0]);
                    uint8_t g=Converter::strTo_UInt(color_vect[1]);
                    uint8_t b=Converter::strTo_UInt(color_vect[2]);
                    uint8_t a=Converter::strTo_UInt(color_vect[3]);
                    temp_color=Converter::packRGBA4444(r,g,b,a);
                 }
                 switch(i){
                    case 0: colors_bg[0]=temp_color;break;
                    case 1: colors_buttons[0]=temp_color;break;
                    case 2: colors_items[0]=temp_color;break;
                    case 3:colors_items[1]=temp_color;break;
                    case 4:colors_items[2]=temp_color;break;
                    case 5:colors_items[3]=temp_color;break;
                    case 6:colors_bg[1]=temp_color;break;
                    case 7:colors_buttons[1]=temp_color;break;
                    case 8: visual.color_style.itemgBg_color=temp_color;break;
                 }
              }
              visual.sprite_btn=sprites_buttons;
              visual.color_style.bg_colors=colors_bg;
              visual.color_style.button_colors=colors_buttons;
              visual.color_style.items_colors=colors_items;
              visual.font_id=props.get_uint("Font_Id",0xFF);

              UI_sys.register_comboBox(id_entity,visual,layout,items,box_ev);
              break;
          }
          case UI_LIST:{
              main_type=UI_LIST;
              ListBox_Events listbox_evs;
              ListBox_Layout layout;
              ListBox_Visual visual;
              std::vector<std::string> items;
              std::vector<uint8_t> items_states;
              layout.max_item_show=props.get_uint("UI_ListBox_Max_Items_Show",10);
              layout.heigth_items=props.get_uint("UI_ListBox_Items_Height",header.half_height*2);
              layout.width_items=props.get_uint("UI_ListBox_Items_Width",header.half_width*2);
              layout.Separation_ItemsX=props.get_int("UI_ListBox_Separation_Items_X",0);
              layout.Separation_ItemsY=props.get_int("UI_ListBox_Separation_Items_Y",0);
              layout.direction_items=props.get_uint("UI_ListBox_Items_Direction",UI_ORIENTATION_VERTICAL);

              uint16_t items_count=props.get_uint("UI_ListBox_Items_Count",0);
              std::vector<uint16_t> colors_items;
              std::vector<uint16_t> colors_bg;
              colors_bg.assign(2,0);
              colors_items.assign(4,0);

              listbox_evs.callfunction=props.get_string("UI_ListBox_CallFunction","");
              listbox_evs.callHoverFunction=props.get_string("UI_ListBox_CallHoverFunction","");
              listbox_evs.down_key=props.get_uint("UI_ListBox_Down_Key",0xFF);
              listbox_evs.up_key=props.get_uint("UI_ListBox_Up_Key",0xFF);
              listbox_evs.trigger_key=props.get_uint("UI_ListBox_Trigger_Key",0xFF);
              listbox_evs.isSelected=props.get_bool("UI_ListBox_Initial_Selected",false);
              listbox_evs.useKey_Inputs=props.get_bool("UI_ListBox_Use_Keys_Input",false);

              items.reserve(items_count);
              items_states.assign(items_count,UI_STATE_NORMAL);
              for(uint16_t i=0;i<items_count;i++){
                  std::string str_index=to_string(i);
                  std::string val_item=props.get_string("UI_ListBox_Item_"+str_index,"");
                  uint8_t state_item=props.get_uint("UI_ListBox_Item_State_"+str_index,UI_STATE_NORMAL);
                  items.push_back(val_item);
                  items_states[i]=state_item;
              }
              uint8_t color_count=7;
              for(uint8_t i=0;i<color_count;i++){
                 std::string str_index=to_string(i);
                 uint16_t temp_color=0;
                 std::vector<std::string> color_vect=Converter::str_toVect(props.get_string("UI_ListBox_Color_"+str_index,""));
                 if(color_vect.size()>=4){
                    uint8_t r=Converter::strTo_UInt(color_vect[0]);
                    uint8_t g=Converter::strTo_UInt(color_vect[1]);
                    uint8_t b=Converter::strTo_UInt(color_vect[2]);
                    uint8_t a=Converter::strTo_UInt(color_vect[3]);
                    temp_color=Converter::packRGBA4444(r,g,b,a);
                 }
                 switch(i){
                    case 0: colors_bg[0]=temp_color;
                    case 1: colors_items[0]=temp_color;
                    case 2: colors_items[1]=temp_color;
                    case 3:colors_items[2]=temp_color;
                    case 4: colors_items[3]=temp_color;
                    case 5:colors_bg[1]=temp_color;
                    case 6:visual.item_bg_color=temp_color;
                 }
              }
              visual.items_colors=colors_items;
              visual.bg_color=colors_bg;
              visual.font_id=props.get_uint("Font_Id",0xFF);
              visual.icon_sprite=props.get_uint("UI_ListBox_Sprite_Icon",0xFFFF);

              UI_sys.register_listBox(id_entity,layout,visual,items,items_states,listbox_evs);
              break;
          }
           default:break;

      }
   }

   header.main_type=main_type;
   UI_sys.register_Header(id_entity,header);
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
void Scene::load_sceneSource(){
   if(next_scene<0){
      return;
   }
   std::string next_scene_path=list_scenes[next_scene];
   Scene_Loader loader;
   float w_map;
   float h_map;
   loader.LoadSceneTiledXML(next_scene_path.c_str(),this,w_map,h_map,ui_entities);
   next_scene=-1;
   std::vector<uint16_t> entities_send;
   entities_send.reserve(last_entity);
   for(uint16_t i=0;i<last_entity;i++){
      entities_send.push_back(i);
   }
   quatree_sys.Set_World(this,w_map,h_map,entities_send,entities_quatree_dat);
   audio_sys.play_music(0,true,0);
   main_camera.get_entities_Quatree(&quatree_sys);
   float best_dim=(w_map>h_map)?w_map:h_map;
   uint8_t max_depth=quatree_sys.get_max_depth();
   float porcent_distance=0.4;
   threshold_updateQuatree_Entity=porcent_distance*best_dim*Math_G::Pow(0.5,max_depth+1);
}

//the Transform of a Entity Change
void Scene::transform_change(uint16_t id){

   std::vector<uint16_t> near_cam=main_camera.get_NearEntities();
   bool is_near=false;
   for(uint16_t index:near_cam){
      if(index==id){
         Transform_Data* transf=transf_sys.get_transform(id);
         is_near=true;
         float distX=Math_G::Abs(entities_quatree_dat[id].last_x-transf->x);
         float distY=Math_G::Abs(entities_quatree_dat[id].last_y-transf->y);

         if(distX>=threshold_updateQuatree_Entity || distY>=threshold_updateQuatree_Entity){
            entities_quatree_dat[id].last_x=transf->x;
            entities_quatree_dat[id].last_y=transf->y;
            quatree_sys.request_reassignEntity(id);
         }

         break;
      }
   }
   if(HasComponentBit(components_masks[id],RIGIDBODY_COMP) && is_near==false){
       RigidBody_Data* rb=rigidbd_sys.get_body(id);

       if(rb->b2bodyPtr!=NULL){
          rb->b2bodyPtr->SetEnabled(false);
       }
   }
   if(is_near && entities_changes_transform==false){

      entities_changes_transform=true;
   }
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
    std::bitset<64> mask_A=components_masks[entityA];
    std::bitset<64> mask_B=components_masks[entityB];
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
    std::bitset<64> mask_A=components_masks[entityA];
    std::bitset<64> mask_B=components_masks[entityB];
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

//Remove a Unused Joint Component
void Scene::remove_Jointcomponent(uint16_t id,uint8_t joint_type,bool require_destroy){

   if(id>=last_entity){
      return;
   }
   int8_t count_joints=0;
   if(HasComponentBit(components_masks[id],JOINT_COMP)){
       join_sys.unregister_joint(id,joint_type,count_joints,require_destroy);
       if(count_joints<=0){
           RemoveComponentBit(components_masks[id],JOINT_COMP);
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

//Get Font System
Fonts_System* Scene::get_font_sys(){
   return &fonts_sys;
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
std::bitset<64> Scene::get_mask(uint16_t id){
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

//Get the Joint System
Joint_System* Scene::get_joint_sys(){
   return &join_sys;
}

//Get the Sprite Composer System
SpriteComposer_System* Scene::get_SpriteComposer_sys(){
  return &spriteComposerSys;
}

//Get the UI System
UI_System* Scene::get_UI_sys(){
   return &UI_sys;
}


//Get Player System
Player_System* Scene::get_player_sys(){
   return &player_sys;
}

//Get the Script System
Script_System* Scene::get_script_system(){
    return &script_sys;
}

//Get the Entity Tree Data Required for Quatree Updates
Entity_TreeData* Scene:: get_entity_treeData(uint16_t id){
   if(id>=entities_quatree_dat.size()){
      return NULL;
   }
   return &entities_quatree_dat[id];
}
