#ifndef ANIMATION_H
#define ANIMATION_H
#include<cstdint>
#include<vector>
#include<unordered_map>
#include<string>
class textureManager_system;

//Available Conditions for Animation Transitions
enum CONDITIONS_IDS:uint8_t{
    CONDITION_VELOCITY_X=0,
    CONDITION_VELOCITY_Y=1,
    CONDITION_GROUND=2,
    CONDITION_ATTACK=3,
    CONDITION_DAMAGED=4,
    CONDITION_DEAD=5,
    CONDITION_SECOND_ATTACK=6,
    COUNT_CONDITIONS

};

//Type of Condition Verification
enum ANIMATION_VERIFICATIONS:uint8_t{
   VERIFICATION_EQUALS=0,
   VERIFICATION_NOT_EQUALS=1,
   VERIFICATION_GREATER=2,
   VERIFICATION_LOWER=3,
   VERIFICATION_GREATER_OR_EQUALS=4,
   VERIFICATION_LOWER_OR_EQUALS=5
};

//Type of Condition Value
enum ANIMATION_CONDITION_TYPE:uint8_t{
   TYPE_NUMERIC=0,
   TYPE_BOOLEAN=1
};

//Direction of Animation
enum ANIMATION_DIRECTION:uint8_t{
   ANIMATION_DIR_HORIZONTAL=0,
   ANIMATION_DIR_VERTICAL=1,
};

//Frame Data
struct Frame_Source{
   float duration;
   uint16_t sprite_id;

};

//Data of an Animation Condition
struct Condition_Animation{
   float value_condition;
   uint8_t verification;
   uint8_t param_id;

};

//Conditions for a Transition
struct Conditions_Transition{
   std::vector<Condition_Animation> list_conditions;
};

//Animation Data
struct Animation{
   std::vector<Frame_Source> frames;
   std::vector<Conditions_Transition> conds_transitions;
   std::vector<uint8_t> transition_Animations;
   float time_elapsed_last_frame;
   bool loop;
   uint8_t actual_frame;

};

//Data of Animator Entity
struct Animator_Data{
   std::vector<Animation> list_animations;
   std::vector<float> params_animator;
   std::vector<uint8_t> params_types;
   uint8_t animation_actual;
   bool loaded;
};

//Animator System
struct Animator_System{
   public:
       void register_Entity(uint16_t id,std::string source,textureManager_system* texture_sys);
       void Free_System();
       void Reserve_Memory(uint16_t count_animators,uint16_t count_entities,uint16_t fill_val);
       Animator_Data* get_animator(uint16_t id);
       void update_system(float dt);
       void set_Numericparam_value(uint16_t id,uint8_t param_id,float value);
       void set_Boolparam_value(uint16_t id,uint8_t param_id,bool value);
       float get_param_value(uint16_t id,uint8_t param_id);
   private:
       bool verify_condition(float value_param,Condition_Animation target_condition);
       void update_animator(uint16_t animator_index,float dt);
       void read_animationSource(Animator_Data& anim,const char* src,textureManager_system* texture_sys);
       std::vector<Animator_Data> list_animators;
       std::vector<uint16_t> EntityToDense;
};

#endif // ANIMATION_H
