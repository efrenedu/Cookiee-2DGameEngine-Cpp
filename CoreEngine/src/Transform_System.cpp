#include "Transform_System.h"

//Reserve Memory for the Transforms
void Transform_System::reserve_memory(int16_t num_elements){
    transform_list.reserve(num_elements);
}

//Free the system
void Transform_System:: free_system(){
    transform_list.clear();
}

//Register a Transform New Entity
void Transform_System:: register_transform(float px,float py,int16_t t_scale,float angle,bool flip_val){
   Transform_Data data;
   data.angle=angle;
   data.scale=t_scale;
   data.x=px;
   data.y=py;
   data.lastRadians=0;
   data.flipped=flip_val;
   data.accumlated_radians=0;
   transform_list.push_back(data);
}

//get the Transform Data
Transform_Data* Transform_System::get_transform(uint16_t id){
   return &transform_list[id];
}
