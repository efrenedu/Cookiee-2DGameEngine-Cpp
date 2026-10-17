#include "Animation.h"
#include "json.hpp"
#include<iostream>
#include<fstream>
#include<hash_support.h>
#include<string>
#include<texture.h>

//Free Memory of Animator System
void Animator_System::Free_System(){
    for(Animator_Data& anim:list_animators){
         anim.params_animator.clear();
         anim.params_types.clear();
         for(Animation& temp_animation:anim.list_animations){
            temp_animation.transition_Animations.clear();
            temp_animation.conds_transitions.clear();
            temp_animation.frames.clear();
         }
         anim.list_animations.clear();
    }
    list_animators.clear();
    EntityToDense.clear();
}

//Reserve Memory for the Animators
void Animator_System::Reserve_Memory(uint16_t count_animators,uint16_t count_entities,uint16_t fill_val){
    list_animators.reserve(count_animators);
    EntityToDense.assign(count_entities,fill_val);
}

//Register Animator
void Animator_System::register_Entity(uint16_t id,std::string source,textureManager_system* texture_sys){
   Animator_Data new_animator;
   new_animator.loaded=false;
   new_animator.params_animator.assign(COUNT_CONDITIONS,0.0);
   std::vector<uint8_t> type_params_values={TYPE_NUMERIC,TYPE_NUMERIC,TYPE_BOOLEAN,TYPE_BOOLEAN,TYPE_BOOLEAN,TYPE_BOOLEAN,TYPE_BOOLEAN};
   new_animator.params_types.reserve(COUNT_CONDITIONS);
   for(uint8_t val:type_params_values){
     new_animator.params_types.push_back(val);
   }
   source=source.substr(source.find_last_of("/\\") + 1);
   source="assets\\animations\\"+source;
   read_animationSource(new_animator,source.c_str(),texture_sys);
   uint8_t new_index=list_animators.size();
   list_animators.push_back(new_animator);
   EntityToDense[id]=new_index;

}
//Read Animator JSON Source and Assign Values
void Animator_System::read_animationSource(Animator_Data& anim,const char* src,textureManager_system* texture_sys){
  std::ifstream file(src);
  if(!file.is_open()){
    return ;
  }
  anim.animation_actual=0;
  nlohmann::json json_dat;
  file>>json_dat;
  uint8_t number_anims=0;
  std::string texture_source="";
  uint8_t animation_direction=ANIMATION_DIR_HORIZONTAL;
  if(json_dat.contains("Direction_Animations")){
     if(json_dat["Direction_Animations"]=="Vertical"){
        animation_direction=ANIMATION_DIR_VERTICAL;
      }
  }
  if(!json_dat.contains("Texture_Source")){
      std::cout<<"Error Reading Animator: No Texture Source found"<<std::endl;
      return;
  }
  texture_source=json_dat.value("Texture_Source","");
  if(texture_source==""){
     std::cout<<"Error Reading Animator: Texture Source Not Indicated"<<std::endl;
     return;
  }
  uint8_t tex_id=texture_sys->get_textureId_fromNameSource(texture_source.c_str());
  if(!json_dat.contains("Animations_Count")){
      std::cout<<"Error Reading Animator: Animations Count Not Found"<<std::endl;
      return;
  }
  number_anims=json_dat.value("Animations_Count",0);
  if(number_anims<=0){
      std::cout<<"Error Reading Animator: Number of Animations Invalid"<<std::endl;
      return;
  }
  anim.list_animations.reserve(number_anims);
  if(json_dat.contains("Animations")){
     //Animations of Animator
     auto animations_node=json_dat["Animations"];
     if(animations_node.is_array()==false){
        std::cout<<"Error Reading Animator: Animations Format Invalid"<<std::endl;
        return;
     }
     for(auto anim_node:animations_node){
        Animation next_anim;
        next_anim.actual_frame=0;
        next_anim.time_elapsed_last_frame=0.0;

        uint8_t num_frames=0;
        if(!anim_node.contains("Frame_Count")){
           std::cout<<"Error Reading Animation of Animator: Frame Count Not Found"<<std::endl;
           continue;
        }
        num_frames=anim_node.value("Frame_Count",0);
        if(!anim_node.contains("Frames") || num_frames<=0){
            std::cout<<"Error Reading Animation of Animator: No Frames"<<std::endl;
            continue;
        }
        if(!anim_node.contains("First_Sprite")){
            std::cout<<"Error Reading Animation of Animator: Error Reading Sprite Source of First Frame"<<std::endl;
            continue;
        }
        auto sprite_node=anim_node["First_Sprite"];
        uint16_t row_sprite=0;
        uint16_t column_sprite=0;
        if(!sprite_node.contains("Row") || !sprite_node.contains("Column")){
            std::cout<<"Error Reading Animation of Animator: Error Reading Row and Column of Sprite Source of First Frame"<<std::endl;
            continue;
        }
        row_sprite=sprite_node.value("Row",0);
        column_sprite=sprite_node.value("Column",0);
        uint16_t column_count;
        uint16_t sprite_id=texture_sys->get_spriteby_grid(tex_id,row_sprite,column_sprite,column_count);
        next_anim.frames.reserve(num_frames);
        auto frames_node=anim_node["Frames"];
        bool frames_correct=true;
        uint16_t frame_index=0;
        for(uint8_t i=0;i<num_frames;i++){
             std::string target_frame="Frame_"+std::to_string(i);
             if(!frames_node.contains(target_frame)){
                 frames_correct=false;
                 break;
             }
             Frame_Source temp_frame;
             uint16_t target_sprite=sprite_id;
             if(animation_direction==ANIMATION_DIR_HORIZONTAL){
                  target_sprite+=frame_index;
             }
             else{
                target_sprite+=(frame_index*column_count);
             }
             temp_frame.duration=frames_node.value(target_frame,0.0);
             temp_frame.sprite_id=target_sprite;
             next_anim.frames.push_back(temp_frame);
             frame_index++;
        }
        if(frames_correct==false){
             std::cout<<"Error Reading Animation of Animator "<<"Some Frames are Missed"<<std::endl;
             continue;
        }
        std::vector<Conditions_Transition> list_cond_animation;

        if(anim_node.contains("Transitions")){
             auto transition_node=anim_node["Transitions"];
             for(auto target_transition:transition_node){
                if(!target_transition.contains("Next_Animation")){
                     std::cout<<"Error Adding Transition to Animation: Next Animation not Found"<<std::endl;
                     continue;
                }
                uint8_t id_transitionAnim=target_transition.value("Next_Animation",0);
                next_anim.transition_Animations.push_back(id_transitionAnim);
                Conditions_Transition conds_target_transition;
                if(target_transition.contains("Count_Conditions")){
                     uint8_t number_conds=target_transition.value("Count_Conditions",0);
                     conds_target_transition.list_conditions.reserve(number_conds);
                }
                if(target_transition.contains("Conditions")){
                    auto node_conditions=target_transition["Conditions"];
                    if(node_conditions.is_array()==false){
                         std::cout<<"Error Adding Conditions to Animation Transition:Bad Format of Conditions"<<std::endl;
                         continue;
                    }
                    std::vector<std::string> target_condParams={"Param_Id","Value_Condition","Verification_Type"};
                    std::vector<Condition_Animation> target_conds;
                    for(auto node_cond:node_conditions){
                        Condition_Animation cond;
                        bool data_correct=true;
                        for(std::string target:target_condParams){
                            if(!node_cond.contains(target)){
                                data_correct=false;
                                break;
                            }
                        }
                        if(data_correct==false){
                            std::cout<<"Error Adding Conditions to Animation Transition:Some Conditions Params Not Found"<<std::endl;
                            continue;
                        }
                        std::string param_target=node_cond.value("Param_Id","");
                        cond.param_id=get_AnimatorParam_Id(param_target.c_str());
                        cond.value_condition=node_cond.value("Value_Condition",0.0);
                        std::string verification_str=node_cond.value("Verification_Type","");
                        cond.verification=get_AnimationCondition_VerificationType(verification_str.c_str());
                        target_conds.push_back(cond);
                    }
                    conds_target_transition.list_conditions=target_conds;

                }
                list_cond_animation.push_back(conds_target_transition);
             }
        }
        next_anim.conds_transitions=list_cond_animation;
        if(anim_node.contains("Loop")){
             next_anim.loop=anim_node.value("Loop",false);
        }
        anim.list_animations.push_back(next_anim);
     }
  }
  if(anim.list_animations.size()<=0){
    std::cout<<"no animations"<<std::endl;

     return;
  }
  anim.loaded=true;

}

//Update Animator System
void Animator_System::update_system(float dt){
   for(uint16_t i=0;i<list_animators.size();i++){
       update_animator(i,dt);
   }
}


//Update an Animator Component
void Animator_System::update_animator(uint16_t animator_index,float dt){
   Animator_Data target=list_animators[animator_index];
   uint8_t index_anim=target.animation_actual;
   if(index_anim>=target.list_animations.size()){
      return;
   }
   Animation anim_target=target.list_animations[index_anim];
   uint8_t index_frame=anim_target.actual_frame;
   if(index_frame>=anim_target.frames.size()){
       return;
   }

   //update frames of Animation
   Frame_Source frame_actual=anim_target.frames[index_frame];
   float elapsed_time=anim_target.time_elapsed_last_frame;
   float target_time=frame_actual.duration;
   if(elapsed_time<target_time){
       elapsed_time+=dt;
   }
   if(elapsed_time>=target_time){
       elapsed_time=0.0f;
       uint8_t next_index=anim_target.actual_frame;
       if(next_index+1>=anim_target.frames.size()){
           if(anim_target.loop==true){
              next_index=0;
           }
       }
       else{
          next_index++;
       }
       anim_target.actual_frame=next_index;
   }

   //verify Transitions
   int8_t change_anim=-1;
   for(uint8_t i=0;i<anim_target.transition_Animations.size();i++){
      bool all_clear=true;
      uint8_t target_next_anim=anim_target.transition_Animations[i];
      std::vector<Condition_Animation>list_cond=anim_target.conds_transitions[i].list_conditions;
      for(Condition_Animation& cond:list_cond){
          float val_param=target.params_animator[cond.param_id];
          if(verify_condition(val_param,cond)==false){
             all_clear=false;
             break;
          }
      }
      if(all_clear && target_next_anim<target.list_animations.size()){

          std::cout<<"Change Animation"<<std::endl;
          change_anim=anim_target.transition_Animations[i];
          anim_target.actual_frame=0;
          anim_target.time_elapsed_last_frame=0.0f;
          elapsed_time=0.0f;
          break;
      }

   }

   anim_target.time_elapsed_last_frame=elapsed_time;
   target.list_animations[index_anim]=anim_target;
   if(change_anim!=-1){
      target.animation_actual=change_anim;
   }
   list_animators[animator_index]=target;



}

//Verify if the Param Condition Clear the Condition
bool Animator_System::verify_condition(float value_param,Condition_Animation target_condition){
    bool res=false;
    switch(target_condition.verification){
        case VERIFICATION_EQUALS:{
               if(value_param==target_condition.value_condition){
                  res=true;
               }
               break;
        }
        case VERIFICATION_NOT_EQUALS:{
               if(value_param!=target_condition.value_condition){
                  res=true;
               }
               break;
        }
        case VERIFICATION_GREATER:{
               if(value_param>target_condition.value_condition){
                  res=true;
               }
               break;
        }
        case VERIFICATION_GREATER_OR_EQUALS:{
              if(value_param>=target_condition.value_condition){
                   res=true;
                   break;
              }
        }
        case VERIFICATION_LOWER:{
               if(value_param<target_condition.value_condition){
                  res=true;
               }
               break;
        }
        case VERIFICATION_LOWER_OR_EQUALS:{
              if(value_param<=target_condition.value_condition){
                   res=true;
                   break;
              }
        }
        default:break;
    }
   return res;
}

//Get the Animator by Id
Animator_Data* Animator_System:: get_animator(uint16_t id){
   return &list_animators[EntityToDense[id]];
}


//Set an Animator Numeric Parameter Value
void Animator_System::set_Numericparam_value(uint16_t id,uint8_t param_id,float value){
  Animator_Data* anim=get_animator(id);
  if(anim==NULL){
     return;
  }
  if(param_id>=COUNT_CONDITIONS){
     return;
  }
  if(anim->params_types[param_id]==TYPE_BOOLEAN){
      return;
  }
  anim->params_animator[param_id]=value;
}

//Set an Animator BooleanParameter Value
void Animator_System::set_Boolparam_value(uint16_t id,uint8_t param_id,bool value){
  Animator_Data* anim=get_animator(id);
  if(anim==NULL){
     return;
  }
  if(param_id>=COUNT_CONDITIONS){
     return;
  }
  if(anim->params_types[param_id]!=TYPE_BOOLEAN){
      return;
  }
  float next_val=(value==true)?1:0;
  anim->params_animator[param_id]=next_val;

}
//Get the Animator Parameter Value
float Animator_System::get_param_value(uint16_t id,uint8_t param_id){
  Animator_Data* anim=get_animator(id);
  if(anim==NULL){
     return 0.0;
  }
  if(param_id>=COUNT_CONDITIONS){
     return 0.0;
  }
  return anim->params_animator[param_id];
}
