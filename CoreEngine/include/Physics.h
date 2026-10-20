#ifndef PHYSICS_H
#define PHYSICS_H
#include<cstdint>
#include<vector>
#include<box2d/box2d.h>
#include<string>

//Figure Type of Colliders
enum FIGURE_TYPE:uint8_t{
   FIGURE_CIRCLE=0,
   FIGURE_SQUARE=1,
   FIGURE_POLYGON=2,
   FIGURE_NONE=3,
};

//Collision Layers Ids for the Mask of Collision
enum Collision_Category:uint16_t{
   COLLISION_NONE=0,
   COLLISION_WORLD=1<<0,
   COLLISION_GROUND=1<<1,
   COLLISION_PLAYER=1<<2,
   COLLISION_ENEMY=1<<3,
   COLLISION_PLAYER_BULLET=1<<4,
   COLLISION_ENEMY_BULLET=1<<5,
   COLLISION_TRIGGER=1<<6,
   COLLISION_ITEM=1<<7,
   COLLISION_EVERYTHING=0XFFFF

};


//Type of Joints
enum JOINT_TYPES:uint8_t{
  DISTANCE_JOINT=0,REVOLUTE_JOINT=1,PRISMATIC_JOINT=2,PULLEY_JOINT=3,WELD_JOINT=4,JOINT_INVALID=5
};
enum Joint_Flags:uint8_t{
   JointFlag_None=0,
   JointFlag_Distance=1<<0,
   JointFlag_Revolute=1<<1,
   JointFlag_Prismatic=1<<2,
   JointFlag_Weld=1<<3,
   JointFlag_Pulley=1<<4
};

//Store the Physicis Material Data
struct Physic_Material_Data{
    float friction;
    float dinamic_friction;
    float restitution;
    uint32_t hash_name;
};

//System for the Manage of Physic Materials
struct Physic_Material_System{
  public:
    void free_system();
    void reserve_memory(uint8_t count_materials);
    void register_material(float e_friction,float d_friction,float retitut,uint32_t hash_name);
    Physic_Material_Data* get_material(int8_t id);
    int8_t get_material_id(uint32_t hash_name);

  private:
    std::vector<Physic_Material_Data> list_materials;

};

//Data of a Circle Shape
struct CircleGeometry{
   float radius;
   float unused;
};
//Data of a Box Shape
struct BoxGeometry{
   float width;
   float height;
};

struct Vertex_Polygon{
   float point_x;
   float point_y;
};

struct fixture_Data{
   uint16_t mask_fixture;
   uint16_t category_bits_fixture;
   float offsetX;
   float offsetY;
   float radius;
   bool is_sensor;
};

//Store the data of Collider
struct Collider_Data{
     union{
        BoxGeometry box;
        CircleGeometry circle;
     }shape;
     std::vector<Vertex_Polygon> vertexs;
     std::vector<fixture_Data> Additional_fixtures;
     uint16_t collider_mask;
     uint16_t category_bits;
     float offset_x;
     float offset_y;
     uint8_t material_id;
     uint8_t padding;
     uint8_t collider_type;
     bool is_trigger;
};

//Collider System of Entities
struct Collider_System{
public:
    void free_system();
    void reserve_memory(uint16_t count_colliders,uint16_t count_entities,uint16_t fillval);
    void register_collider(uint16_t id,uint8_t mat_id,bool trigger, uint16_t mask_col_str,uint16_t bits_str,std::string fig_t,float radius,float w,float h,float offsetX,float offsetY,float padding,std::vector<Vertex_Polygon> vertex_list,std::vector<fixture_Data> add_fixtures );
    Collider_Data* get_collider(uint16_t id);
private:
    std::vector<Collider_Data>list_colliders;
    std::vector<uint16_t> EntitytoDense;


};

//Esscential Data of Joints
struct Joint_Header{
  b2Joint* b2jointPtr=nullptr;
  uint8_t mask_joint;
  uint16_t entityA;
  uint16_t entityB;
  uint16_t jointId;
};

//Distance Joint Data
struct Distance_Join_Data{
   Joint_Header header;
   float anchorAX;
   float anchorAY;
   float anchorBX;
   float anchorBY;
   float mindistance;
   float maxdistance;
   float stiffness;
   float damping;

};

//Revolute Joint Data
struct Revolute_Join_Data{
   Joint_Header header;
   float anchorX;
   float anchorY;
   float upperAngle;
   float lowerAngle;
   float motorSpeed;
   float maxMotorTorque;
   bool enableLimits;
   bool enableMotor;
};

//Pulley Joint Data
struct Pulley_Join_Data{
   Joint_Header header;
};

//Weld Joint Data
struct Weld_Join_Data{
   Joint_Header header;
};

//Prismatic Joint Data
struct Prismatic_Join_Data{
    Joint_Header header;
    float anchorX;
    float anchorY;
    float axisX;
    float axisY;
    float lowerTranslation;
    float upperTranslation;
    float motorSpeed;
    float maxMotorForce;
    bool enableLimits;
    bool enableMotor;
};

//System  of Joint Component
class Joint_System{
public:
    void free_system();
    void reserve_memory(uint16_t count_joints,uint16_t count_entities,uint16_t fillval);
    void unregister_joint(uint16_t id,uint8_t joint_type);
    void register_distance_joint(uint16_t id,float min_dist,float max_dist,float stiff,float dampm,float anchorA_X,float anchorA_Y,float anchorB_X,float anchorB_Y);
    void register_prismatic_joint(uint16_t id,float anchor_x,float anchor_y,float axis_x,float axis_y,float lower_translation,float upper_translation,float motor_spd,float max_motor_Force,bool use_motor,bool use_limits);
    void register_revolute_joint(uint16_t id,float anchor_x,float anchor_y,float lower_angle,float upper_angle,float motor_spd,float max_motor_torque,bool use_motor,bool use_limits);
    void register_pulley_joint(uint16_t id);
    void register_weld_joint(uint16_t id);
    Distance_Join_Data& get_distance_joint(uint16_t id);
    Revolute_Join_Data& get_revolute_joint(uint16_t id);
    Prismatic_Join_Data& get_prismatic_joint(uint16_t id);
    Weld_Join_Data& get_weld_joint(uint16_t id);
    Pulley_Join_Data& get_pulley_joint(uint16_t id);
private:
    b2World * world_physics=nullptr;
    std::vector<Distance_Join_Data> distance_joint_list;
    std::vector<Revolute_Join_Data> revolute_joint_list;
    std::vector<Weld_Join_Data> weld_joint_list;
    std::vector<Prismatic_Join_Data>prismatic_joint_list;
    std::vector<Pulley_Join_Data> pulley_joint_list;
    std::vector<uint16_t> EntitytoDenseDistance;
    std::vector<uint16_t> EntitytoDensePrismatic;
    std::vector<uint16_t> EntitytoDensePulley;
    std::vector<uint16_t> EntitytoDenseRevolute;
    std::vector<uint16_t> EntitytoDenseWeld;
};

//RigidBody Data
struct RigidBody_Data{
  b2Body* b2bodyPtr=nullptr;
  float mass;
  float gravity_scale;
  float offsetCenterX;
  float offsetCenterY;
  float Last_ForceX;
  float Last_ForceY;
  float last_Angular_Force;
  float damping;
  float angular_damping;
  bool is_kinematic;
  bool lock_rotations;
  bool can_sleep;
  bool active;
};

//System for RigidBody Component
struct RigidBody_System{
public:
    void free_system();
    void reserve_memory(uint16_t count_bodies,uint16_t count_entities,uint16_t fillval);
    void register_body(uint16_t id,float mss,float grav_scale,float damp,float angular_damp,bool activ,bool kinematic,bool sleep,bool lock_rot);
    void add_force(uint16_t id,float x_force,float y_force);
    void set_velocity(uint16_t id,float x_vel,float y_vel);
    void set_AngularVelocity(uint16_t id,float angular_vel);
    void add_AngularForce(uint16_t id,float angular_Force);
    void set_activeBody(uint16_t id,bool new_state);
    void set_transform(uint16_t id,float new_x,float new_y,float new_angle=0,bool change_angle=false);
    RigidBody_Data* get_body(uint16_t id);

private:
    std::vector<RigidBody_Data>list_bodies;
    std::vector<uint16_t> EntitytoDense;

};



#endif // PHYSICS_H
