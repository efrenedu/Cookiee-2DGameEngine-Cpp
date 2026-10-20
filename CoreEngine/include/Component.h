#ifndef COMPONENT_H
#define COMPONENT_H
#include<cstdint>

//Indicate the Type of Component for the Mask of a Entity
enum components_types:uint8_t{
    TRANSFORM_COMP=0,
    RENDER_COMP=1,
    COLLIDER_COMP=2,
    RIGIDBODY_COMP=3,
    ANIMATOR_COMP=4,
    JOINT_COMP=5,
    BUTTON_COMP=6,
    LABEL_COMP=7,
    PANEL_COMP=8,
    CHECKBTN_COMP=9,
    RADIOBTN_COMP=10,
    VALUEBAR_COMP=11,
    SCROLLBAR_COMP=12,
    SLIDER_COMP=13,
    CANVAS_COMP=14,
    PLAYER_COMP=15,
    ENEMY_COMP=16,
    UKNOW_COMP=32,
    COMP_COUNT
};
using ComponentMask=uint32_t;

//Auxiliar Functions for Manage Bits of Component Mak
inline void Add_ComponentBit(ComponentMask& mask,components_types type){
   mask |=(1U<<type);
}
inline void RemoveComponentBit(ComponentMask& mask , components_types type){
  mask &=~(1U<<type);
}
inline bool HasComponentBit(ComponentMask mask,components_types type){
   return (mask &(1U<<type))!=0;
}



#endif // COMPONENT_H
