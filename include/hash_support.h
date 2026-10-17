#ifndef HASH_SUPPORT_H
#define HASH_SUPPORT_H
#include<Physics.h>
#include<Component.h>
#include<cstdint>
#include<array>
#include<Input.h>
#include<Animation.h>

//Make a Hash Uint32 from a String
inline constexpr uint32_t hashString(const char* str,uint32_t hash_t=2166136261u){

   uint32_t val_multiply=16777619u;
   return (*str=='\0')?hash_t:hashString(str+1,(hash_t^static_cast<uint8_t>(*str))*val_multiply);

}
//operator _hash
inline constexpr uint32_t operator""_hash(const char* str,size_t len){
   return hashString(str);
}


//Get the Component Type from a String using the hash of it
inline constexpr components_types ComponentName_to_ComponentIndex(const char* name ){
   if(!name){
      return UKNOW_COMP;
   }
   switch(hashString(name)){
      case "transform"_hash: return TRANSFORM_COMP;
      case "render"_hash:return RENDER_COMP;
      case "collider"_hash:return COLLIDER_COMP;
      case "rigidbody"_hash:return RIGIDBODY_COMP;
      case "joint"_hash:return JOINT_COMP;
      case "animator"_hash:return ANIMATOR_COMP;
      case "player"_hash:return PLAYER_COMP;
      default: return UKNOW_COMP;
   }

}
//Get the List of Availables components Names
inline constexpr auto GetComponentKeysNames(){
    std::array<const char*,COMP_COUNT> keys={
      "transform","render","camera","collider","rigidbody","joint","animator","button","label","panel","checkbtn",
      "radiobtn","valuebar","scrollbar","slider","canvas","player","enemy",
    };
    return keys;
}

//get the Id of a Layer of Collision from the String Hash of it
inline constexpr uint16_t get_LayerCollider_mask(const char* name_layer){
   if(!name_layer){
      return Collision_Category::COLLISION_NONE;
   }
   switch(hashString(name_layer)){
      case "world"_hash:return Collision_Category::COLLISION_WORLD;
      case "ground"_hash:return Collision_Category::COLLISION_GROUND;
      case "player"_hash:return Collision_Category::COLLISION_PLAYER;
      case "enemy"_hash:return Collision_Category::COLLISION_ENEMY;
      case "player_bullet"_hash:return Collision_Category::COLLISION_PLAYER_BULLET;
      case "enemy_bullet"_hash:return Collision_Category::COLLISION_ENEMY_BULLET;
      case "trigger"_hash:return Collision_Category::COLLISION_TRIGGER;
      case "item"_hash:return Collision_Category::COLLISION_ITEM;
      case "all"_hash:return Collision_Category::COLLISION_EVERYTHING;
      default: return Collision_Category::COLLISION_NONE;
   }
}

//Get the Figure Type from the String Hash of It
inline constexpr uint8_t get_figure_type(const char* name_fig){
   if(!name_fig){
      return FIGURE_TYPE::FIGURE_NONE;
   }
   switch(hashString(name_fig)){
      case "circle"_hash:return FIGURE_TYPE::FIGURE_CIRCLE;
      case "square"_hash:return FIGURE_TYPE::FIGURE_SQUARE;
      case "polygon"_hash:return FIGURE_TYPE::FIGURE_POLYGON;
      default: return FIGURE_TYPE::FIGURE_NONE;
   }
}

//Get the Keyboard Id from the String Hash of the Player Action
inline constexpr uint8_t get_key_id(const char* name_key){
   if(!name_key){
      return KEY_INVALID;
   }
   switch(hashString(name_key)){
      case "Action_Key"_hash:return KEY_ACTION;
      case "Action_Second_Key"_hash:return KEY_SECOND_ACTION;
      case "Interact_Key"_hash:return KEY_INTERACT;
      case "Jump_Key"_hash:return KEY_JUMP;
      case "Left_Key"_hash:return KEY_LEFT;
      case "Right_Key"_hash:return KEY_RIGHT;
      case "Up_Key"_hash:return KEY_UP;
      case "Down_Key"_hash:return KEY_DOWN;

      default: return KEY_INVALID;
   }
}

//Get the Animator Parameter Id from a String
inline constexpr uint8_t get_AnimatorParam_Id(const char* param_name){
   if(!param_name){
      return CONDITION_VELOCITY_X;
   }
   switch(hashString(param_name)){
      case "Vel X"_hash:return CONDITION_VELOCITY_X;
      case "Vel Y"_hash:return CONDITION_VELOCITY_Y;
      case "Damage"_hash:return CONDITION_DAMAGED;
      case "Dead"_hash:return CONDITION_DEAD;
      case "Attack"_hash:return CONDITION_ATTACK;
      case "Second Attack"_hash:return CONDITION_SECOND_ATTACK;
      case "Ground"_hash: return CONDITION_GROUND;
      default: return CONDITION_VELOCITY_X;
   }
}
//Get the Type of Animation Condition Verification from a String
inline constexpr uint8_t get_AnimationCondition_VerificationType(const char* verification_name){
   if(!verification_name){
      return VERIFICATION_EQUALS;
   }
   switch(hashString(verification_name)){
      case "Equals"_hash:return VERIFICATION_EQUALS;
      case "Not Equals"_hash:return VERIFICATION_NOT_EQUALS;
      case "Greater"_hash:return VERIFICATION_GREATER;
      case "Lower"_hash:return VERIFICATION_LOWER;
      case "Lower or Equals"_hash:return VERIFICATION_LOWER_OR_EQUALS;
      case "Greater or Equals"_hash:return VERIFICATION_GREATER_OR_EQUALS;
      default: return VERIFICATION_EQUALS;
   }
}
#endif // HASH_SUPPORT_H
