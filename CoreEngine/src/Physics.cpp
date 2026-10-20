#include "Physics.h"
#include "Timer.h"
#include "Component.h"
#include "hash_support.h"
#include<iostream>
#include<Converter.h>
#include<math_structs.h>

//Physic Material System Methods

//Clear Physic Material System
void Physic_Material_System::free_system(){
   list_materials.clear();
}

//Reserve Memory for the list of materials
void Physic_Material_System::reserve_memory(uint8_t count_materials){
   list_materials.reserve(count_materials);
}

//Register a Physic Material On the System
void Physic_Material_System::register_material(float e_friction,float d_friction,float retitut,uint32_t hash_name){
   Physic_Material_Data data;
   data.dinamic_friction=d_friction;
   data.friction=e_friction;
   data.restitution=retitut;
   data.hash_name=hash_name;
   list_materials.push_back(data);

}

//Get the Id of Physic Materual with a verification of String Hash
int8_t Physic_Material_System::get_material_id(uint32_t hash_name){
  for(uint8_t i=0;i<list_materials.size();i++){
      Physic_Material_Data temp_mat=list_materials[i];
      if(temp_mat.hash_name==hash_name){
          return i;
      }
  }
  return -1;
}

//Get the Physic Material by Id
Physic_Material_Data* Physic_Material_System::get_material(int8_t id){
    if(id<0 || id>=list_materials.size()){
          return NULL;
    }
    return &list_materials[id];
}

//Collider System Methods

//Reserve Memory for the Colliders
void Collider_System::reserve_memory(uint16_t count_colliders,uint16_t count_entitites,uint16_t fillval){
    list_colliders.reserve(count_colliders);
    EntitytoDense.assign(count_entitites,fillval);
}

//Free Collider System
void Collider_System::free_system(){
   list_colliders.clear();
   EntitytoDense.clear();
}

//Register a Collider
void Collider_System::register_collider(uint16_t id,uint8_t mat_id,bool trigger, uint16_t mask_col,uint16_t bits,std::string fig_t,float radius, float w,float h,float offsetX,float offsetY,float padding,std::vector<Vertex_Polygon> vertex_list,std::vector<fixture_Data> add_fixtures){
   Collider_Data collider;
   collider.material_id=mat_id;
   collider.vertexs=vertex_list;
   collider.collider_mask=mask_col;
   collider.category_bits=bits;
   collider.is_trigger=trigger;
   collider.offset_x=offsetX;
   collider.offset_y=offsetY;
   collider.padding=padding;
   collider.Additional_fixtures=add_fixtures;
   uint8_t new_figure=get_figure_type(fig_t.c_str());

   switch(new_figure){
      case FIGURE_TYPE::FIGURE_CIRCLE:{
         collider.shape.circle.radius=radius;
         break;
      }
      case FIGURE_TYPE::FIGURE_SQUARE:{
         collider.shape.box.height=h;
         collider.shape.box.width=w;
         break;
      }
      case FIGURE_TYPE::FIGURE_POLYGON:{
          collider.shape.box.height=h;
          collider.shape.box.width=w;
          break;
      }

      default:{
         break;
      }
   }
   collider.collider_type=new_figure;
   uint16_t next_id=list_colliders.size();
   EntitytoDense[id]=next_id;
   list_colliders.push_back(collider);
}

//get the collider
Collider_Data* Collider_System::get_collider(uint16_t id){
   return &list_colliders[EntitytoDense[id]];
}

//Joint System Methods

//Reserve Memory for the System
void Joint_System::reserve_memory(uint16_t count_joint,uint16_t count_entities,uint16_t fillval){
     EntitytoDenseDistance.assign(count_entities,fillval);
     EntitytoDenseRevolute.assign(count_entities,fillval);
     EntitytoDensePrismatic.assign(count_entities,fillval);
     EntitytoDensePulley.assign(count_entities,fillval);
     EntitytoDenseWeld.assign(count_entities,fillval);
     distance_joint_list.reserve(count_joint);
     revolute_joint_list.reserve(count_joint);
     prismatic_joint_list.reserve(count_joint);
     weld_joint_list.reserve(count_joint);
     pulley_joint_list.reserve(count_joint);
}

//clear the joint system
void Joint_System::free_system(){
   for(Distance_Join_Data& temp_joint: distance_joint_list){
       b2Joint* temp_joint_ptr=temp_joint.header.b2jointPtr;
       world_physics->DestroyJoint(temp_joint_ptr);
   }
   for(Revolute_Join_Data& temp_joint: revolute_joint_list){
       b2Joint* temp_joint_ptr=temp_joint.header.b2jointPtr;
       world_physics->DestroyJoint(temp_joint_ptr);
   }
   for(Prismatic_Join_Data& temp_joint: prismatic_joint_list){
       b2Joint* temp_joint_ptr=temp_joint.header.b2jointPtr;
       world_physics->DestroyJoint(temp_joint_ptr);
   }
   for(Weld_Join_Data& temp_joint: weld_joint_list){
       b2Joint* temp_joint_ptr=temp_joint.header.b2jointPtr;
       world_physics->DestroyJoint(temp_joint_ptr);
   }
    for(Pulley_Join_Data& temp_joint: pulley_joint_list){
       b2Joint* temp_joint_ptr=temp_joint.header.b2jointPtr;
       world_physics->DestroyJoint(temp_joint_ptr);
   }
   pulley_joint_list.clear();
   prismatic_joint_list.clear();
   distance_joint_list.clear();
   revolute_joint_list.clear();
   weld_joint_list.clear();
}

//Register a Distance Joint
void Joint_System::register_distance_joint(uint16_t id,float min_dist,float max_dist,float stiff,float damp,float anchorA_X,float anchorA_Y,float anchorB_X,float anchorB_Y){
   Distance_Join_Data joint;
   joint.anchorAX=anchorA_X;
   joint.anchorAY=anchorA_Y;
   joint.anchorBX=anchorB_X;
   joint.anchorBY=anchorB_Y;
   joint.damping=damp;
   joint.stiffness=stiff;
   joint.maxdistance=max_dist;
   joint.mindistance=min_dist;
   joint.header.mask_joint=JointFlag_Distance;
   uint16_t next_joint=distance_joint_list.size();
   EntitytoDenseDistance[id]=next_joint;
   distance_joint_list.push_back(joint);
}

//Register a Prismatic Joint
void Joint_System::register_prismatic_joint(uint16_t id,float anchor_x,float anchor_y,float axis_x,float axis_y,float lower_translation,float upper_translation,float motor_spd,float max_motor_Force,bool use_motor,bool use_limits){

   Prismatic_Join_Data joint;
   joint.anchorX=anchor_x;
   joint.anchorY=anchor_y;
   joint.axisX=axis_x;
   joint.axisY=axis_y;
   joint.enableLimits=use_limits;
   joint.enableMotor=use_motor;
   joint.lowerTranslation=lower_translation;
   joint.upperTranslation=upper_translation;
   joint.maxMotorForce=max_motor_Force;
   joint.motorSpeed=motor_spd;
   joint.header.mask_joint=JointFlag_Prismatic;
   uint16_t next_joint=prismatic_joint_list.size();
   EntitytoDensePrismatic[id]=next_joint;
   prismatic_joint_list.push_back(joint);
}

//Register a Revolute Joint
void Joint_System::register_revolute_joint(uint16_t id,float anchor_x,float anchor_y,float lower_angle,float upper_angle,float motor_spd,float max_motor_torque,bool use_motor,bool use_limits){
   Revolute_Join_Data joint;
   joint.anchorX=anchor_x;
   joint.anchorY=anchor_y;
   joint.enableLimits=use_limits;
   joint.enableMotor=use_motor;
   joint.lowerAngle=lower_angle;
   joint.upperAngle=upper_angle;
   joint.maxMotorTorque=max_motor_torque;
   joint.header.mask_joint=JointFlag_Revolute;
   uint16_t next_joint=revolute_joint_list.size();
   EntitytoDenseRevolute[id]=next_joint;
   revolute_joint_list.push_back(joint);
}

//Register a Weld Joint
void Joint_System::register_weld_joint(uint16_t id){
   Weld_Join_Data joint;
   joint.header.mask_joint=JointFlag_Weld;
   uint16_t next_joint=weld_joint_list.size();
   EntitytoDenseWeld[id]=next_joint;
   weld_joint_list.push_back(joint);
}

//Register a Pulley Joint
void Joint_System::register_pulley_joint(uint16_t id){
   Pulley_Join_Data joint;
   joint.header.mask_joint=JointFlag_Pulley;
   uint16_t next_joint=pulley_joint_list.size();
   EntitytoDensePulley[id]=next_joint;
   pulley_joint_list.push_back(joint);
}

//RigidBody System Methods

//Reserve Memory for RigidBodies
void RigidBody_System::reserve_memory(uint16_t count_bodies,uint16_t count_entities,uint16_t fillval){
    EntitytoDense.assign(count_entities,fillval);
    list_bodies.reserve(count_bodies);
}

//Free System
void RigidBody_System::free_system(){
   EntitytoDense.clear();
   list_bodies.clear();
}

//Register a Body
void RigidBody_System::register_body(uint16_t id,float mss,float grav_scale,float damp,float angular_damping,bool activ,bool kinematic,bool sleep,bool lock_rot){
   RigidBody_Data body;
   body.mass=mss;
   body.gravity_scale=grav_scale;
   body.last_Angular_Force=0;
   body.Last_ForceX=0;
   body.Last_ForceY=0;
   body.offsetCenterX=0;
   body.offsetCenterY=0;
   body.active=activ;
   body.can_sleep=sleep;
   body.lock_rotations=lock_rot;
   body.is_kinematic=kinematic;
   body.damping=damp;
   body.angular_damping=angular_damping;
   uint16_t next_id=list_bodies.size();
   EntitytoDense[id]=next_id;
   list_bodies.push_back(body);
}

//Add Force to the Indicated Body by Id
void RigidBody_System::add_force(uint16_t id,float x_force,float y_force){
   RigidBody_Data* rb=get_body(id);
   if(rb==NULL){
      return;
   }
   if(rb->b2bodyPtr==nullptr){
      return;
   }
   if(rb->lock_rotations){
     b2Vec2 force(x_force,y_force);
      rb->b2bodyPtr->ApplyLinearImpulseToCenter(force,true);
   }
   else{
      b2Vec2 offset(rb->offsetCenterX,rb->offsetCenterY);
      b2Vec2 center=rb->b2bodyPtr->GetWorldCenter()+offset;
      b2Vec2 impulse_force(x_force,y_force);
      rb->b2bodyPtr->ApplyLinearImpulse(impulse_force,center,true);
   }
}

//Set the Velocity of Indicated Body by Id
void RigidBody_System::set_velocity(uint16_t id,float x_vel,float y_vel){
   RigidBody_Data* rb=get_body(id);
   if(rb==NULL){
      return;
   }
   if(rb->b2bodyPtr==nullptr){
      return;
   }
   b2Vec2 new_vel(x_vel,y_vel);
   rb->b2bodyPtr->SetLinearVelocity(new_vel);
}

//Set the Angular Velocity of Indicated Body by Id
void RigidBody_System::set_AngularVelocity(uint16_t id,float angular_vel){
   RigidBody_Data* rb=get_body(id);
   if(rb==NULL){
      return;
   }
   if(rb->b2bodyPtr==nullptr){
      return;
   }
   rb->b2bodyPtr->SetAngularVelocity(angular_vel);
}

//Add Angular Force to the Indicated Body by Id
void RigidBody_System::add_AngularForce(uint16_t id,float angular_Force){
   RigidBody_Data* rb=get_body(id);
   if(rb==NULL){
      return;
   }
   if(rb->b2bodyPtr==nullptr){
      return;
   }
   rb->b2bodyPtr->ApplyAngularImpulse(angular_Force,true);
}

//Set transform of Indicated Body by Id
void RigidBody_System::set_transform(uint16_t id,float new_x,float new_y,float new_angle,bool change_angle){

   RigidBody_Data* rb=get_body(id);
   if(rb==NULL){
      return;
   }
   if(rb->b2bodyPtr==nullptr){
      return;
   }
   float angle=rb->b2bodyPtr->GetAngle();
   if(change_angle){
      angle=Math_G::Wrap_Angle(Math_G::to_rad(new_angle));
   }
   b2Vec2 old_pos=rb->b2bodyPtr->GetPosition();
   new_x=Converter::pixelToMetter(new_x);
   new_y=Converter::pixelToMetter(new_y);
   b2Vec2 offset_center(rb->offsetCenterX,rb->offsetCenterY);
   b2Vec2 new_pos(new_x+offset_center.x,new_y-offset_center.y);
   rb->b2bodyPtr->SetTransform(new_pos,angle);
   rb->b2bodyPtr->SetAwake(true);
   old_pos=rb->b2bodyPtr->GetPosition();
}

//Set Active flag of Indicated Body by Id
void RigidBody_System::set_activeBody(uint16_t id,bool new_state){
    RigidBody_Data* rb=get_body(id);
   if(rb==NULL){
      return;
   }
   if(rb->b2bodyPtr==nullptr){
      return;
   }
   rb->b2bodyPtr->SetEnabled(new_state);
}

//get the RigidBody
RigidBody_Data* RigidBody_System::get_body(uint16_t id){
   return &list_bodies[EntitytoDense[id]];
}

/*
Joint_Component::Joint_Component(){
   child="";
   parent="";
   type_joint=-1;
   anchorOffset_Child=Vector2(0,0);
   anchorOffset_Parent=Vector2(0,0);
   joint_collide=false;
   ratio_val=0.0f;
   axis=Vector2(0,0);
   stiffness=0.0f;
   damping=0.0f;
   min_limit=0.0f;
   max_limit=0.0f;
   angle_ref=0.0f;
   speed=0.0f;
   max_speed=0.0f;
   use_limits=false;
   groundOffset_Child=Vector2(0,0);
   groundOffset_Parent=Vector2(0,0);
   update_data=false;
}
//Build the Joint
Joint_Component::Joint_Component(string chld,string parnt,int typ,Vector2 anchor_Chld,Vector2 anchor_Parnt,bool collide){
   child=chld;
   parent=parnt;
   type_joint=typ;
   anchorOffset_Child=anchor_Chld;
   anchorOffset_Parent=anchor_Parnt;
   joint_collide=collide;
   ratio_val=0.0f;
   axis=Vector2(0,0);
   stiffness=0.0f;
   damping=0.0f;
   min_limit=0.0f;
   max_limit=0.0f;
   angle_ref=0.0f;
   speed=0.0f;
   max_speed=0.0f;
   use_limits=false;
   groundOffset_Child=Vector2(0,0);
   groundOffset_Parent=Vector2(0,0);
   update_data=false;
}

//Getter Methods
Vector2 Joint_Component::get_anchorOffset_Child(){
    return anchorOffset_Child;
}
Vector2 Joint_Component::get_anchorOffset_Parent(){
    return anchorOffset_Parent;
}

int Joint_Component::get_type(){
   return type_joint;
}
string Joint_Component::get_child(){
   return child;
}
string Joint_Component::get_parent(){
  return parent;
}
bool Joint_Component::get_collide(){
   return joint_collide;
}

//set the data for Distance Joint
void Joint_Component::set_DistanceJoint_data(float min_distance,float max_distance,float dampng,float stiffnss){

   min_limit=min_distance;
   max_limit=max_distance;
   damping=dampng;
   stiffness=stiffnss;
   use_limits=true;
}
//set the data for Revolute Joint
void Joint_Component::set_RevolutionJoint_data(float min_angle,float max_angle,float angle_reference,float max_spd,float spd,bool apply_limit){
     angle_ref=angle_reference;
     min_limit=min_angle;
     max_limit=max_angle;
     use_limits=apply_limit;
     speed=spd;
     max_speed=max_spd;

}
//set the data for Pulley Joint
void Joint_Component::set_PulleyJoint_data(Vector2 chld_groundOffset,Vector2 parntGround_offset,float distance_child,float distance_parent,float ratio_v){
   groundOffset_Child=chld_groundOffset;
   groundOffset_Parent=parntGround_offset;
   ratio_val=ratio_v;
   min_limit=distance_child;
   max_limit=distance_parent;
   use_limits=true;
}
//set the data for Prismatic Joint
void Joint_Component::set_PrismaticJoint_data(Vector2 axs,float min_translation,float max_translation,float spd,float max_spd,bool apply_limit,float angl){
    axis=axs;
    speed=spd;
    max_speed=max_spd;
    use_limits=apply_limit;
    min_limit=min_translation;
    max_limit=max_translation;
    angle_ref=angl;
}
//set the data for Weld Joint
void Joint_Component::set_WeldJoint_data(float angle_reference,float dampng,float stiffnss){
    angle_ref=angle_reference;
    stiffness=stiffnss;
    damping=dampng;
    use_limits=false;
}

//setters Methods
void Joint_Component::change_maxLimit(float val){
    max_limit=val;
    if(max_limit==min_limit){
        max_limit+=1;
    }
    else if(max_limit<min_limit){
        max_limit=min_limit;
        min_limit=val;
    }
    update_data=true;
}
void Joint_Component::change_minLimit(float val){
   min_limit=val;
   if(min_limit==max_limit){
      max_limit+=1;
   }
   else if(min_limit>max_limit){
      min_limit=max_limit;
      max_limit=val;
   }
   update_data=true;

}
void Joint_Component::change_damping(float val){
   damping=val;
   update_data=true;
}
void Joint_Component::change_stiffness(float val){
   stiffness=val;
   update_data=true;
}

//clear request for update in box2d
void Joint_Component::clear_requests(){
   update_data=false;
}
//return True if is Neccesary update the data in box2d
bool Joint_Component::require_update(){
   return update_data;
}

//getter Methods
float Joint_Component::get_damping(){
   return damping;
}
float Joint_Component::get_stiffness(){
   return stiffness;
}
float Joint_Component::get_maxLimit(){
   return max_limit;
}
float Joint_Component::get_minLimit(){
   return min_limit;
}
float Joint_Component::get_ratio(){
   return ratio_val;
}
Vector2 Joint_Component::get_axis(){
   return axis;
}
float Joint_Component::get_speed(){
   return speed;
}
float Joint_Component::get_maxSpeed(){
    return max_speed;
}
float Joint_Component::get_Angle(){
   return angle_ref;
}
Vector2 Joint_Component::get_groundOffset_Child(){
    return groundOffset_Child;
}
Vector2 Joint_Component::get_groundOffset_Parent(){
    return groundOffset_Parent;
}
bool Joint_Component::apply_limits(){
    return use_limits;
}

*/
