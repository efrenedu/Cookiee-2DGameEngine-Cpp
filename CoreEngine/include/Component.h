#ifndef COMPONENT_H
#define COMPONENT_H
#include<cstdint>
#include<bitset>

//Indicate the Type of Component for the Mask of a Entity
enum components_types:uint8_t{
    TRANSFORM_COMP=0,
    RENDER_COMP=1,
    COLLIDER_COMP=2,
    RIGIDBODY_COMP=3,
    ANIMATOR_COMP=4,
    SPRITECOMPOSER_COMP=5,
    JOINT_COMP=6,
    UI_COMP=7,
    PLAYER_COMP=8,
    ENEMY_COMP=9,
    UKNOW_COMP=30,
    COMP_COUNT
};
using ComponentMask=uint64_t;

//Auxiliar Functions for Manage Bits of Component Mak
inline void Add_ComponentBit(std::bitset<64>&mask,components_types type){
   mask.set(type);
}
inline void RemoveComponentBit(std::bitset<64>&mask , components_types type){
  mask.reset(type);
}
inline bool HasComponentBit(std::bitset<64>&mask,components_types type){
   return mask.test(type);
}



#endif // COMPONENT_H
