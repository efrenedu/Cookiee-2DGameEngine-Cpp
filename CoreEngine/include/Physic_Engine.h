#ifndef PHYSIC_ENGINE_H
#define PHYSIC_ENGINE_H
#include<Scene.h>
#include <cstdint>
class contactListener;

//Store a RayCast Hit data
struct RayCastHit {
    bool hit;
    b2Vec2 normal;
    uint16_t hit_entity;
    b2Vec2 hit_point;

};

//For Detect RayCast
struct RaycastDetection:public b2RayCastCallback{
     bool hit=false;
     b2Vec2 hit_point;
     b2Vec2 hit_normal;
     b2Fixture* hit_fixture=NULL;
     float ReportFixture(b2Fixture* fixture,const b2Vec2& point,const b2Vec2& normal,float fraction) override {
         hit=true;
         hit_point=point;
         hit_normal=normal;
         hit_fixture=fixture;
         return fraction;
    }

};

//Store the data of a Contact Collision Event
struct contactCollisionData{
   uint16_t entityA;
   uint16_t entityB;
   float normal_x;
   float normal_y;
   bool begin_ev;
};

//Store the data of a Contact Trigger Event
struct contactTriggerData{
   uint16_t entityA;
   uint16_t entityB;
   bool begin_ev;
};


//Engine of Physic System
struct Physic_Engine_System{
  public:
      void Init(Scene* target_scene,float gravity=9.8,float grav_dirX=0.0,float gravity_dirY=-1.0);
      void loadWorld(Scene* target_scene);
      void reset_world();
      void clear_world();
      void update(Scene* target_scene);
      void update_world(float dt,Scene* target_scene);
      RayCastHit RayCast(float x ,float y ,float dir_x,float dir_y,float magnitude,uint16_t bits_raycast=0xFFFF,float last_x=0,float last_y=0,bool autocalled=false);
      void add_CollisionEvent(uint16_t entityA,uint16_t entityB,float x_norm,float y_norm,bool begin_ev);
      void add_TriggerEvent(uint16_t entityA,uint16_t entityB,bool begin_ev);
      void request_destroy_joint(b2Joint* jnt);
      void change_gravityDir(float x_grav,float y_grav,float gravity_val=9.8);
  private:
    void add_jointToworld(Distance_Join_Data* joint_data,b2Body* bdA,b2Body* bdB);
    void add_jointToworld(Revolute_Join_Data* joint_data,b2Body* bdA,b2Body* bdB);
    void add_jointToworld(Prismatic_Join_Data* joint_data,b2Body* bdA,b2Body* bdB);
    void add_jointToworld(Weld_Join_Data* joint_data,b2Body* bdA,b2Body* bdB);
    void add_jointToworld(Pulley_Join_Data* joint_data,b2Body* bdA,b2Body* bdB);
    void verify_destroy_joints();
    void build_fixture(b2Body* bd,Physic_Material_System* physyic_material_sys,Collider_Data* col,float mass_body,float angl,uint16_t index_entity);
    void update_joinData(uint16_t index,Joint_System* joint_sys);
    b2World* world=nullptr;
    contactListener* listener=nullptr;
    std::vector<contactCollisionData> collisions_queue;
    std::vector<contactTriggerData> triggers_queue;
    std::vector<b2Joint*> destroy_joint_queue;

    float time_steps;
    float accumul_steps;
    uint8_t velIterations;
    uint8_t posIterations;
    float limit_forceBodies=20.0f;
    float limit_forceSquare=limit_forceBodies*limit_forceBodies;

};

//for Collision Events
class contactListener:public b2ContactListener{
   public:
      contactListener(Scene* scen,Physic_Engine_System* engine):target_scene(scen),engine_sys(engine){}
      void BeginContact(b2Contact* contact);
      void EndContact(b2Contact * contact);
      void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
   private:
     Scene *target_scene=nullptr;
     Physic_Engine_System* engine_sys=nullptr;
};

#endif // PHYSIC_ENGINE_H
