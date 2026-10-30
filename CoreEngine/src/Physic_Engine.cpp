#include "Physic_Engine.h"
#include "Physics.h"
#include "Component.h"
#include "Timer.h"
#include "box2d/box2d.h"
#include "Converter.h"
#include<iostream>
#include "math_structs.h"


/*Methods for Contact Listener*/

//Begin Contact Event
void contactListener::BeginContact(b2Contact* contact){
   b2Fixture* fixtureA=contact->GetFixtureA();
   b2Fixture* fixtureB=contact->GetFixtureB();
   uint32_t data_fixtureA=static_cast<uint32_t>(fixtureA->GetUserData().pointer);
   uint32_t data_fixtureB=static_cast<uint32_t>(fixtureB->GetUserData().pointer);
   int8_t id_fixtureA=((data_fixtureA>>16)&0xFF);
   int8_t id_fixtureB=((data_fixtureB>>16)&0xFF);
   bool sensorA=(id_fixtureA==-1)?false:true;
   bool sensorB=(id_fixtureB==-1)?false:true;
   uint16_t entityA= static_cast<uint16_t>(data_fixtureA &0xFFFF);
   uint16_t entityB= static_cast<uint16_t>(data_fixtureB & 0xFFFF);
   b2Filter filterA=fixtureA->GetFilterData();
   b2Filter filterB=fixtureB->GetFilterData();

   if(sensorA || sensorB ){
      engine_sys->add_TriggerEvent(entityA,entityB,true);
   }
   else{
      b2WorldManifold worldManifold;
      contact->GetWorldManifold(&worldManifold);
      b2Vec2 norm=worldManifold.normal;
      engine_sys->add_CollisionEvent(entityA,entityB,norm.x,norm.y,true);
   }
}

//EndContact Evemt
void contactListener::EndContact(b2Contact* contact){
   b2Fixture* fixtureA=contact->GetFixtureA();
   b2Fixture* fixtureB=contact->GetFixtureB();
   uint32_t data_fixtureA=static_cast<uint32_t>(fixtureA->GetUserData().pointer);
   uint32_t data_fixtureB=static_cast<uint32_t>(fixtureB->GetUserData().pointer);
   int8_t id_fixtureA=((data_fixtureA>>16)&0xFF);
   int8_t id_fixtureB=((data_fixtureB>>16)&0xFF);
   bool sensorA=(id_fixtureA==-1)?false:true;
   bool sensorB=(id_fixtureB==-1)?false:true;
   uint16_t entityA= static_cast<uint16_t>(data_fixtureA &0xFFFF);
   uint16_t entityB= static_cast<uint16_t>(data_fixtureB & 0xFFFF);

   if(sensorA || sensorB ){
       engine_sys->add_TriggerEvent(entityA,entityB,false);
   }
   else{
      b2WorldManifold worldManifold;
      contact->GetWorldManifold(&worldManifold);
      b2Vec2 norm=worldManifold.normal;
      engine_sys->add_CollisionEvent(entityA,entityB,norm.x,norm.y,false);
   }
}

//Pre Solve Event
void contactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold){
    b2Fixture* fixtureA=contact->GetFixtureA();
    b2Fixture* fixtureB=contact->GetFixtureB();
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    uint32_t data_fixtureA=static_cast<uint32_t>(fixtureA->GetUserData().pointer);
    uint32_t data_fixtureB=static_cast<uint32_t>(fixtureB->GetUserData().pointer);
    int8_t id_fixtureA=((data_fixtureA>>16)&0xFF);
    int8_t id_fixtureB=((data_fixtureB>>16)&0xFF);
    bool sensorA=(id_fixtureA==-1)?false:true;
    bool sensorB=(id_fixtureB==-1)?false:true;
    uint16_t entityA= static_cast<uint16_t>(data_fixtureA &0xFFFF);
    uint16_t entityB= static_cast<uint16_t>(data_fixtureB & 0xFFFF);

}


//Physic Engine System methods

//init the System
void Physic_Engine_System::Init(Scene* target_scene,float gravity,float grav_dirX,float grav_dirY){
   b2Vec2 gravity_dir(gravity*grav_dirX,gravity*grav_dirY);
   world=new b2World(gravity_dir);
   listener=new contactListener(target_scene,this);
   world->SetContactListener(listener);
   target_scene->get_joint_sys()->Init(this);
   accumul_steps=0;
   time_steps=1.0/60.0f;
   posIterations=3;
   velIterations=8;

}

//Load and Buildthe Bodies of Physic World from the Scene
void Physic_Engine_System::loadWorld(Scene* target_scene){
   if(target_scene==NULL){
      std::cout<<"Scene Invalid"<<std::endl;
      return;
   }
   if(world==NULL){
       std::cout<<"World Invalid"<<std::endl;
       return;
   }
   reset_world();
   destroy_joint_queue.clear();
   Physic_Material_System* physic_materials=target_scene->get_physic_material_sys();
   Transform_System* transform_sys=target_scene->get_transform_sys();
   Collider_System* collider_sys=target_scene->get_collider_sys();
   RigidBody_System* rigidbody_sys=target_scene->get_rigidbody_sys();

   uint16_t count_entities=target_scene->get_count_entities();
   std::vector<uint16_t> has_joint;
   for(uint16_t i=0;i<count_entities;i++){
       Transform_Data* transform_entity=transform_sys->get_transform(i);
       std::bitset<64> mask_entity=target_scene->get_mask(i);
       if(HasComponentBit(mask_entity,components_types::COLLIDER_COMP)==false){
            continue;
       }
       Collider_Data* collider_entity=collider_sys->get_collider(i);
       if(collider_entity==NULL){
          continue;
       }
       float half_sz_x=0;
       float half_sz_y=0;
       switch(collider_entity->collider_type){
            case FIGURE_SQUARE:{
                half_sz_x=collider_entity->shape.box.width*0.5;
                half_sz_y=collider_entity->shape.box.height*0.5;
                break;
            }
            case FIGURE_CIRCLE:{
                half_sz_x=collider_entity->shape.circle.radius;
                half_sz_y=collider_entity->shape.circle.radius;
                break;
            }
            default:break;
       }
       half_sz_x=Converter::pixelToMetter(half_sz_x);
       half_sz_y=Converter::pixelToMetter(half_sz_y);
       b2BodyDef bddefs;
       float angle=Math_G::Wrap_Angle(Math_G::to_rad(transform_entity->angle));
       float mass=0.0;
       bddefs.type=b2BodyType::b2_staticBody;
       RigidBody_Data* rigidbody_dat=NULL;
       if(HasComponentBit(mask_entity,components_types::RIGIDBODY_COMP)==true){

          rigidbody_dat=rigidbody_sys->get_body(i);
          switch(rigidbody_dat->bd_type){
              case BODY_DINAMIC: bddefs.type=b2BodyType::b2_dynamicBody;break;
              case BODY_KINEMATIC: bddefs.type=b2BodyType::b2_kinematicBody;
          }

          bddefs.allowSleep=rigidbody_dat->can_sleep;
          bddefs.gravityScale=rigidbody_dat->gravity_scale;
          bddefs.angle=angle;
          std::cout<<angle<<std::endl;
          bddefs.bullet=false;
          bddefs.fixedRotation=rigidbody_dat->lock_rotations;
          mass=rigidbody_dat->mass;
          bddefs.linearDamping=rigidbody_dat->damping;
          bddefs.angularDamping=rigidbody_dat->angular_damping;
          bddefs.enabled=rigidbody_dat->active;
          if(HasComponentBit(mask_entity,JOINT_COMP)){
              has_joint.push_back(i);
          }
       }

       float x=Converter::pixelToMetter(transform_entity->x);
       float y=Converter::pixelToMetter(transform_entity->y);
       bddefs.position=b2Vec2(x,y);
       b2Body* bd=world->CreateBody(&bddefs);
       if(bd==NULL){
          std::cout<<"fail Init Body"<<i<<std::endl;
          continue;
       }
       if(rigidbody_dat!=NULL){
          rigidbody_dat->b2bodyPtr=bd;
          rigidbody_dat->offsetCenterX=half_sz_x;
          rigidbody_dat->offsetCenterY=half_sz_y;
       }
       build_fixture(bd,physic_materials,collider_entity,mass,Math_G::to_rad(transform_entity->angle),i);

       bd->GetUserData().pointer=static_cast<uintptr_t>(i);

   }
   Joint_System* joint_sys=target_scene->get_joint_sys();
   for (uint16_t index:has_joint){
       RigidBody_Data* rbA=rigidbody_sys->get_body(index);
       if(rbA==NULL){
         continue;
       }
       Distance_Join_Data* distance_joint=joint_sys->get_distance_joint(index);
       Revolute_Join_Data* revolute_joint=joint_sys->get_revolute_joint(index);
       Prismatic_Join_Data* prismatic_joint=joint_sys->get_prismatic_joint(index);
       Weld_Join_Data* weld_joint=joint_sys->get_weld_joint(index);
       Pulley_Join_Data* pulley_joint=joint_sys->get_pulley_joint(index);
       if(distance_joint!=NULL){
         std::bitset<64> maskB=target_scene->get_mask(distance_joint->header.entityB);
         if(HasComponentBit(maskB,RIGIDBODY_COMP)==false){
            continue;
         }

         RigidBody_Data* rbB=rigidbody_sys->get_body(distance_joint->header.entityB);
         if(rbB==NULL){
             continue;
         }
         add_jointToworld(distance_joint,rbA->b2bodyPtr,rbB->b2bodyPtr);
         if(distance_joint->header.removeOnBuild){
            target_scene->remove_Jointcomponent(index,JointFlag_Distance);
         }
       }
       if(revolute_joint!=NULL){

           std::bitset<64> maskB=target_scene->get_mask(revolute_joint->header.entityB);
           if(HasComponentBit(maskB,RIGIDBODY_COMP)==false){
              continue;
           }
           RigidBody_Data* rbB=rigidbody_sys->get_body(revolute_joint->header.entityB);
           if(rbB==NULL){
               continue;
           }
           add_jointToworld(revolute_joint,rbA->b2bodyPtr,rbB->b2bodyPtr);
           if(revolute_joint->header.removeOnBuild){
               target_scene->remove_Jointcomponent(index,JointFlag_Revolute);
           }
       }
       if(prismatic_joint!=NULL){

           std::bitset<64> maskB=target_scene->get_mask(prismatic_joint->header.entityB);
           if(HasComponentBit(maskB,RIGIDBODY_COMP)==false){
              continue;
           }
           RigidBody_Data* rbB=rigidbody_sys->get_body(prismatic_joint->header.entityB);
           if(rbB==NULL){
               continue;
           }
           add_jointToworld(prismatic_joint,rbA->b2bodyPtr,rbB->b2bodyPtr);
           if(prismatic_joint->header.removeOnBuild){
              target_scene->remove_Jointcomponent(index,JointFlag_Prismatic);
           }
       }
       if(weld_joint!=NULL){
           std::bitset<64> maskB=target_scene->get_mask(weld_joint->header.entityB);

           if(HasComponentBit(maskB,RIGIDBODY_COMP)==false){
              continue;
           }
           RigidBody_Data* rbB=rigidbody_sys->get_body(weld_joint->header.entityB);
           if(rbB==NULL){
               continue;
           }
           add_jointToworld(weld_joint,rbA->b2bodyPtr,rbB->b2bodyPtr);
           if(weld_joint->header.removeOnBuild){
            target_scene->remove_Jointcomponent(index,JointFlag_Weld);
           }
       }
       if(pulley_joint!=NULL){

           std::bitset<64> maskB=target_scene->get_mask(pulley_joint->header.entityB);
           if(HasComponentBit(maskB,RIGIDBODY_COMP)==false){
              continue;
           }
           RigidBody_Data* rbB=rigidbody_sys->get_body(pulley_joint->header.entityB);
           if(rbB==NULL){
               continue;
           }
           add_jointToworld(pulley_joint,rbA->b2bodyPtr,rbB->b2bodyPtr);
           if(pulley_joint->header.removeOnBuild){
               target_scene->remove_Jointcomponent(index,JointFlag_Pulley);
           }
       }
   }
}

//Reset the World
void Physic_Engine_System::reset_world(){
   b2Body* old_bds=world->GetBodyList();
   while(old_bds!=NULL){
      b2JointEdge* joints=old_bds->GetJointList();
      while(joints!=NULL){
          b2Joint* jnt=joints->joint;
          jnt->GetUserData().pointer=NULL;
          joints=joints->next;
          world->DestroyJoint(jnt);
      }
      b2Fixture* target_fixture=old_bds->GetFixtureList();
      while(target_fixture!=NULL){
          b2Fixture* next_fixture=target_fixture->GetNext();
          target_fixture->GetUserData().pointer=NULL;
          old_bds->DestroyFixture(target_fixture);
          target_fixture=next_fixture;
      }
      old_bds->GetUserData().pointer=NULL;
      world->DestroyBody(old_bds);
      old_bds=old_bds->GetNext();
   }
   collisions_queue.clear();
   triggers_queue.clear();
}

//Free Memory of System
void Physic_Engine_System::clear_world(){
   reset_world();
   destroy_joint_queue.clear();
   if(world!=nullptr){
      delete world;
      world=nullptr;
   }
   if(listener!=nullptr){
      delete listener;
      listener=nullptr;
   }
}

//Update the World state abd Components from Scene Data
void Physic_Engine_System::update(Scene* target_scene){
   b2Body* bd=world->GetBodyList();
   Transform_System* transform_sys=target_scene->get_transform_sys();
   Collider_System* collider_sys=target_scene->get_collider_sys();
   RigidBody_System* rigidbody_sys=target_scene->get_rigidbody_sys();
   Physic_Material_System* physic_material_sys=target_scene->get_physic_material_sys();
   Joint_System* joint_sys=target_scene->get_joint_sys();

   while(bd!=NULL){
      if(bd->IsEnabled()==false){
         bd=bd->GetNext();
         continue;
      }
      uint16_t id=static_cast<uint16_t>(bd->GetUserData().pointer);
      Transform_Data* transform_entity=transform_sys->get_transform(id);
      std::bitset<64> mask=target_scene->get_mask(id);
      if(HasComponentBit(mask,components_types::COLLIDER_COMP)==false){
           bd=bd->GetNext();
           continue;
      }
      Collider_Data* collider=collider_sys->get_collider(id);
      b2Fixture* fixtures= bd->GetFixtureList();

      //Update Fixture data from Collider
      Physic_Material_Data* material=physic_material_sys->get_material(collider->material_id);
      while(fixtures!=NULL){
         uint32_t data_fixture=static_cast<uint32_t>(fixtures->GetUserData().pointer);
         int8_t id_fixture=((data_fixture>>16)&0xFF);
         bool is_sensor=(id_fixture==-1)?false:true;

         if(is_sensor){
             b2Filter filter_val=fixtures->GetFilterData();
             filter_val.maskBits=collider->Additional_fixtures[id_fixture].mask_fixture;
             filter_val.categoryBits=collider->Additional_fixtures[id_fixture].category_bits_fixture;
             fixtures->SetFilterData(filter_val);
         }
         else{
           fixtures->SetSensor(collider->is_trigger);
           if(material!=NULL){
              fixtures->SetFriction(material->friction);
              fixtures->SetRestitution(material->restitution);
           }
           b2Filter filter_val=fixtures->GetFilterData();
           filter_val.maskBits=collider->collider_mask;
           filter_val.categoryBits=collider->category_bits;
           fixtures->SetFilterData(filter_val);
         }
         fixtures=fixtures->GetNext();
      }

      //Update RigidBody and Transform
      bool has_changed=false;
      if(HasComponentBit(mask,RIGIDBODY_COMP)==true){
          b2Vec2 vel_bd=bd->GetLinearVelocity();
          if(vel_bd.LengthSquared()>limit_forceSquare){
            vel_bd.Normalize();
            vel_bd*=limit_forceBodies;
            bd->SetLinearVelocity(vel_bd);
          }
          RigidBody_Data* rb=rigidbody_sys->get_body(id);
          switch(rb->bd_type){
             case BODY_DINAMIC: bd->SetType(b2BodyType::b2_dynamicBody);break;
             case BODY_KINEMATIC:bd->SetType(b2BodyType::b2_kinematicBody);break;
             case BODY_STATIC:bd->SetType(b2BodyType::b2_staticBody);break;
             default:break;
          }
           bd->SetGravityScale(rb->gravity_scale);
           b2Vec2 final_force=bd->GetLinearVelocity();
           rb->Last_ForceX=final_force.x;
           rb->Last_ForceY=final_force.y;
           float currentRadians=bd->GetAngle();
           transform_entity->accumlated_radians=Math_G::Wrap_AngleAccumulated(currentRadians,transform_entity->lastRadians,transform_entity->accumlated_radians);
           transform_entity->lastRadians=currentRadians;
           float new_angle=Math_G::to_degree(transform_entity->accumlated_radians);
           float renderAngle=fmod(-new_angle,360.0);
           //Update Joints if is Neccesary
           if(HasComponentBit(mask,JOINT_COMP)){
               update_joinData(id,joint_sys);
           }
           if(renderAngle<0){
              renderAngle+=360;
           }

           b2Vec2 new_pos=bd->GetPosition();
           float old_x=transform_entity->x;
           float old_y=transform_entity->y;
           float old_angle=transform_entity->angle;
           transform_entity->x=Converter::MetterToPixel(new_pos.x);
           transform_entity->y=Converter::MetterToPixel(new_pos.y);
           transform_entity->angle=renderAngle;
           rb->last_MoveX=old_x;
           rb->last_MoveY=old_y;

           if(bd->IsAwake()){
             if(Math_G::Abs(old_x-transform_entity->x)>=1.0 || Math_G::Abs(old_y-transform_entity->y)>=1.0 || old_angle!=transform_entity->angle ){
                  has_changed=true;
              }
           }

      }

      if(bd->IsAwake()==true && has_changed==true){
         target_scene->transform_change(id);
      }
      bd=bd->GetNext();
   }
}

//update the world Physics and Forces On the RigidBodies of Scenes
void Physic_Engine_System::update_world(float dt,Scene* target_scene){
   if(dt>1.0){
      return;
   }
   verify_destroy_joints();
   accumul_steps+=dt;
   if(accumul_steps>0.2){
       accumul_steps=0.2;
   }
   while(accumul_steps>=time_steps){
      accumul_steps-=time_steps;
      world->Step(time_steps,velIterations,posIterations);

   }

   if(target_scene==NULL){
      return;
   }
   for(contactTriggerData trigger:triggers_queue){
       uint16_t entityA=trigger.entityA;
       uint16_t entityB=trigger.entityB;
       bool begin_ev=trigger.begin_ev;
       target_scene->Execute_Trigger_Ev(entityA,entityB,begin_ev);
       target_scene->Execute_Trigger_Ev(entityB,entityA,begin_ev);

   }
   for(contactCollisionData collision:collisions_queue){
       uint16_t entityA=collision.entityA;
       uint16_t entityB=collision.entityB;
       bool begin_ev=collision.begin_ev;
       float normal_x=collision.normal_x;
       float normal_y=collision.normal_y;
       target_scene->Execute_Collision_Ev(entityA,entityB,normal_x,normal_y,begin_ev);
       target_scene->Execute_Collision_Ev(entityB,entityA,normal_x,normal_y,begin_ev);

   }
   triggers_queue.clear();
   collisions_queue.clear();

   b2Body* bd=world->GetBodyList();
   RigidBody_System* rigid_body_sys=target_scene->get_rigidbody_sys();
   while(bd!=NULL){
      if(bd->IsEnabled()==false){
         bd=bd->GetNext();
         continue;
      }
      uint16_t id=static_cast<uint16_t>(bd->GetUserData().pointer);
      std::bitset<64> mask_target=target_scene->get_mask(id);

      if(HasComponentBit(mask_target,components_types::RIGIDBODY_COMP)==true){
          RigidBody_Data* rb=rigid_body_sys->get_body(id);
          b2Vec2 force_bd=bd->GetLinearVelocity();
          rb->Last_ForceX=force_bd.x;
          rb->Last_ForceY=force_bd.y;
          float angular_vel=bd->GetAngularVelocity();
          rb->last_Angular_Force=angular_vel;
      }
      bd=bd->GetNext();
   }
}


//Methods for Add a Joint to the World
void Physic_Engine_System::add_jointToworld(Distance_Join_Data* joint_data,b2Body* bdA,b2Body* bdB){
   if(joint_data==NULL || bdA==NULL || bdB==NULL){
     return;
   }
   b2DistanceJointDef defs;
   defs.maxLength=Converter::pixelToMetter(joint_data->maxdistance);
   defs.minLength=Converter::pixelToMetter(joint_data->mindistance);
   b2Vec2 anchorA(Converter::pixelToMetter(joint_data->anchorA_x),Converter::pixelToMetter(joint_data->anchorA_y));
   b2Vec2 anchorB(Converter::pixelToMetter(joint_data->anchorB_x),Converter::pixelToMetter(joint_data->anchorB_y));
   defs.localAnchorA=anchorA;
   defs.localAnchorB=anchorB;
   defs.stiffness=joint_data->stiffness;
   defs.damping=joint_data->damping;
   defs.bodyA=bdA;
   defs.bodyB=bdB;
   defs.collideConnected=joint_data->header.collide_connect;
   b2Joint* joint=world->CreateJoint(&defs);
   joint_data->header.b2jointPtr=joint;

}

void Physic_Engine_System::add_jointToworld(Revolute_Join_Data* joint_data,b2Body* bdA,b2Body* bdB){
    if(joint_data==NULL || bdA==NULL || bdB==NULL){
         return;
    }
    b2RevoluteJointDef defs;
    b2Vec2 pivot=bdB->GetWorldCenter();
    defs.Initialize(bdA,bdB,pivot);
    defs.enableLimit=joint_data->enableLimits;
    defs.enableMotor=joint_data->enableMotor;
    defs.lowerAngle=Math_G::Wrap_Angle(Math_G::to_rad(joint_data->lowerAngle));
    defs.maxMotorTorque=joint_data->maxMotorTorque;
    defs.upperAngle=Math_G::Wrap_Angle(Math_G::to_rad(joint_data->upperAngle));
    defs.motorSpeed=joint_data->motorSpeed;
    defs.referenceAngle=Math_G::Wrap_Angle(Math_G::to_rad(joint_data->ref_angle));
    defs.collideConnected=joint_data->header.collide_connect;
    b2Joint* joint=world->CreateJoint(&defs);
    joint_data->header.b2jointPtr=joint;

}
void Physic_Engine_System::add_jointToworld(Prismatic_Join_Data* joint_data,b2Body* bdA,b2Body* bdB){
    if(joint_data==NULL || bdA==NULL || bdB==NULL){
         return;
    }
    b2PrismaticJointDef defs;
    defs.Initialize(bdA,bdB,bdB->GetWorldCenter(),b2Vec2(-1*joint_data->axisX,-1*joint_data->axisY));
    defs.enableLimit=joint_data->enableLimits;
    defs.enableMotor=joint_data->enableMotor;
    defs.lowerTranslation=Converter::pixelToMetter(joint_data->lowerTranslation);
    defs.upperTranslation=Converter::pixelToMetter(joint_data->upperTranslation);
    defs.maxMotorForce=joint_data->maxMotorForce;
    defs.motorSpeed=joint_data->motorSpeed;
    defs.referenceAngle=Math_G::Wrap_Angle(Math_G::to_rad(joint_data->angle));
    defs.collideConnected=joint_data->header.collide_connect;
    b2Joint* joint=world->CreateJoint(&defs);
    joint_data->header.b2jointPtr=joint;
}
void Physic_Engine_System::add_jointToworld(Weld_Join_Data* joint_data,b2Body* bdA,b2Body* bdB){
    if(joint_data==NULL || bdA==NULL || bdB==NULL){
         return;
    }
    b2WeldJointDef defs;
    defs.bodyA=bdA;
    defs.bodyB=bdB;
    defs.Initialize(bdA,bdB,bdB->GetWorldCenter());
    defs.stiffness=joint_data->stiffnes;
    defs.damping=joint_data->damping;
    defs.referenceAngle=Math_G::Wrap_Angle(Math_G::to_rad(joint_data->ref_angle));

    defs.collideConnected=joint_data->header.collide_connect;
    b2Joint* joint=world->CreateJoint(&defs);
    joint_data->header.b2jointPtr=joint;
}
void Physic_Engine_System::add_jointToworld(Pulley_Join_Data* joint_data,b2Body* bdA,b2Body* bdB){
    if(joint_data==NULL || bdA==NULL || bdB==NULL){
        return;
    }
    b2PulleyJointDef defs;
    b2Vec2 centerA=bdA->GetWorldCenter();
    b2Vec2 centerB=bdB->GetWorldCenter();
    b2Vec2 groundA(centerA.x,centerA.y+Converter::pixelToMetter(joint_data->groundA_OffsetY));
    b2Vec2 groundB(centerB.x,centerB.y+Converter::pixelToMetter(joint_data->groundB_OffsetY));
    float ratio_val=joint_data->ratio_value;
    if(ratio_val<1.0){
        ratio_val=1.0;
    }
    defs.collideConnected=joint_data->header.collide_connect;
    defs.Initialize(bdA,bdB,groundA,groundB,centerA,centerB,ratio_val);
    b2Joint* joint=world->CreateJoint(&defs);
    joint_data->header.b2jointPtr=joint;
}

//Build the Fixtures of Body
void Physic_Engine_System::build_fixture(b2Body* bd,Physic_Material_System* physyic_material_sys,Collider_Data* col,float mass_body,float angl,uint16_t index_entity){
    if(bd==NULL || col==NULL){
         return;
    }
    b2Vec2 offsetCollider(Converter::pixelToMetter(col->offset_x),Converter::pixelToMetter(col->offset_y));
    b2PolygonShape Boxshape;
    b2CircleShape shape_circle;
    b2FixtureDef fixturedef;
    Physic_Material_Data* material=physyic_material_sys->get_material(col->material_id);
    float density=0.0f;
    float restitution=0;
    float friction=0;
    if(material!=NULL){
         restitution=material->restitution;
         friction=material->friction;
    }
    fixturedef.isSensor=col->is_trigger;
    switch(col->collider_type){

        case FIGURE_TYPE::FIGURE_SQUARE:{
           b2Vec2 center_fig(offsetCollider.x,offsetCollider.y);
           float w=Converter::pixelToMetter(col->shape.box.width);
           float h=Converter::pixelToMetter(col->shape.box.height);
           b2Vec2 halfSz(w*0.5,h*0.5);
           Boxshape.SetAsBox(halfSz.x,halfSz.y,center_fig,angl);
           float area=w*h;
           if(area!=0.0 && mass_body!=0.0){
               density=mass_body/area;
           }
           fixturedef.shape=&Boxshape;
           break;
        }
        case FIGURE_TYPE::FIGURE_CIRCLE:{
            float radius=Converter::pixelToMetter(col->shape.circle.radius);
            shape_circle.m_radius=radius;
            shape_circle.m_p=offsetCollider;
            fixturedef.shape=&shape_circle;
            float pow_radius=Math_G::Pow(2,radius);
            float area=MATH_CONSTANTS::PI_VAL*pow_radius;
            if(mass_body!=0.0f && area!=0.0f){
                density=mass_body/area;
            }
            break;
        }
        case FIGURE_TYPE::FIGURE_POLYGON:{
           uint8_t count_vertexs=col->vertexs.size();
           b2Vec2 vertexs[count_vertexs];
           for(uint8_t i=0;i<count_vertexs;i++){
               Vertex_Polygon vertex=col->vertexs[i];
               float px=Converter::pixelToMetter(vertex.point_x);
               float py=Converter::pixelToMetter(vertex.point_y);
               vertexs[i].Set(px,py);
           }
           float area=0.0;
           if(count_vertexs>=3){
              for(uint8_t i=0;i<count_vertexs;i++){
                 uint8_t pair_index=(i+1)%count_vertexs;
                 Vertex_Polygon vertex=col->vertexs[i];
                 Vertex_Polygon vertexpair=col->vertexs[pair_index];
                 area+=(vertex.point_x*vertexpair.point_y)-(vertexpair.point_x*vertex.point_y);
              }
               area=std::abs(area)*0.5;
           }

           Boxshape.Set(vertexs,count_vertexs);

           if(mass_body!=0.0f && area!=0.0f){
                density=mass_body/area;
           }
           col->vertexs.clear();
           fixturedef.shape=&Boxshape;
           break;
        }
    }
    std::vector<fixture_Data> add_fixtures=col->Additional_fixtures;
    uint8_t count_additional_fixtures=add_fixtures.size();
    if(add_fixtures.size()>0){
        density=density/(float)static_cast<uint16_t>(count_additional_fixtures);
    }

    //Main Collider
    fixturedef.density=density;
    fixturedef.friction=friction;
    fixturedef.restitution=restitution;
    fixturedef.filter.categoryBits=col->category_bits;
    fixturedef.filter.maskBits=col->collider_mask;
    uint32_t packed_collider_data=(static_cast<uint32_t>(index_entity) & 0xFFFF)|((static_cast<uint32_t>(static_cast<int8_t>(-1))& 0xFF)<<16);

    fixturedef.userData.pointer=static_cast<uintptr_t>(packed_collider_data);
    bd->CreateFixture(&fixturedef);
    for(uint8_t i=0;i<count_additional_fixtures;i++){
        fixture_Data target_fixture=add_fixtures[i];
        b2FixtureDef target_fixture_defs;

        b2Vec2 target_offset(Converter::pixelToMetter(target_fixture.offsetX),Converter::pixelToMetter(target_fixture.offsetY));
        b2CircleShape target_shape;
        float target_radius=Converter::pixelToMetter(target_fixture.radius);
        target_shape.m_radius=target_radius;
        target_shape.m_p=target_offset;
        target_fixture_defs.shape=&target_shape;
        target_fixture_defs.isSensor=true;
        float target_density=0.0f;
        float pow_radius=Math_G::Pow(2,target_radius);
        float area=MATH_CONSTANTS::PI_VAL*pow_radius;
        if(mass_body!=0.0f && area!=0.0f){
            target_density=(mass_body/area)/(float)static_cast<uint16_t>(count_additional_fixtures);
        }
        target_fixture_defs.density=target_density;
        target_fixture_defs.filter.categoryBits=target_fixture.category_bits_fixture;
        target_fixture_defs.filter.maskBits=target_fixture.mask_fixture;

        uint32_t packed_targetFixture_data=(static_cast<uint32_t>(index_entity) & 0xFFFF)|((static_cast<uint32_t>(static_cast<int8_t>(i))& 0xFF)<<16);
        target_fixture_defs.userData.pointer=static_cast<uintptr_t>(packed_targetFixture_data);
        bd->CreateFixture(&target_fixture_defs);
    }


}

//Request Destroy a Joint
void Physic_Engine_System::request_destroy_joint(b2Joint* jnt){
   destroy_joint_queue.push_back(jnt);
}

//Verify if Is Possible Destroy Any Joint Requested to Destroy
void Physic_Engine_System::verify_destroy_joints(){
   if(world->IsLocked()){
       return;
   }
   for(uint8_t i=0;i<destroy_joint_queue.size();i++){
       world->DestroyJoint(destroy_joint_queue[i]);
   }
   destroy_joint_queue.clear();
}

//Update Join Data Values
void Physic_Engine_System::update_joinData(uint16_t index,Joint_System* joint_sys){
  if(joint_sys==NULL){
      return;
  }
  Distance_Join_Data* distance=joint_sys->get_distance_joint(index);
  Revolute_Join_Data* revolute=joint_sys->get_revolute_joint(index);
  Prismatic_Join_Data* prismatic=joint_sys->get_prismatic_joint(index);
  Weld_Join_Data* weld=joint_sys->get_weld_joint(index);
  if(distance!=NULL){
     b2Joint* jnt=distance->header.b2jointPtr;
     if(jnt==NULL){
         return;
     }
     b2DistanceJoint* target=static_cast<b2DistanceJoint*>(jnt);
     target->SetMaxLength(Converter::pixelToMetter(distance->maxdistance));
     target->SetMinLength(Converter::pixelToMetter(distance->mindistance));
     target->SetDamping(distance->damping);
     target->SetStiffness(distance->stiffness);
  }
  if(revolute!=NULL){
    b2Joint* jnt=revolute->header.b2jointPtr;
    if(jnt==NULL){
         return;
    }
    b2RevoluteJoint* target=static_cast<b2RevoluteJoint*>(jnt);
    target->EnableLimit(revolute->enableLimits);
    target->EnableMotor(revolute->enableMotor);
    target->SetMotorSpeed(revolute->motorSpeed);
    target->SetMaxMotorTorque(revolute->maxMotorTorque);
    float lower=Math_G::Wrap_Angle(Math_G::to_rad(revolute->lowerAngle));
    float upper=Math_G::Wrap_Angle(Math_G::to_rad(revolute->upperAngle));
    target->SetLimits(lower,upper);
  }
  if(prismatic!=NULL){
    b2Joint* jnt=prismatic->header.b2jointPtr;
    if(jnt==NULL){
         return;
    }
    b2PrismaticJoint* target=static_cast<b2PrismaticJoint*>(jnt);
    target->EnableMotor(prismatic->enableMotor);
    target->EnableLimit(prismatic->enableLimits);
    target->SetMaxMotorForce(prismatic->maxMotorForce);
    target->SetMotorSpeed(prismatic->motorSpeed);
    float lower=Converter::pixelToMetter(prismatic->lowerTranslation);
    float upper=Converter::pixelToMetter(prismatic->upperTranslation);
    target->SetLimits(lower,upper);
  }
  if(weld!=NULL){
    b2Joint* jnt=weld->header.b2jointPtr;
    if(jnt==NULL){
         return;
    }
    b2WeldJoint* target=static_cast<b2WeldJoint*>(jnt);
    target->SetDamping(weld->damping);
    target->SetStiffness(weld->stiffnes);
  }
}

//Change Gravity Direction
void Physic_Engine_System::change_gravityDir(float x_grav,float y_grav,float gravity_val){
   b2Vec2 grav_dir(x_grav*gravity_val,y_grav*gravity_val);
   if(world==NULL){
       return;
   }
   world->SetGravity(grav_dir);
}

//Make a RayCast and Return a RayCast Hit
RayCastHit Physic_Engine_System::RayCast(float x ,float y ,float dir_x,float dir_y,float magnitude){
   RayCastHit res;
   x=Converter::pixelToMetter(x);
   y=Converter::pixelToMetter(y);
   res.hit=false;
   res.hit_entity=0;
   RaycastDetection callback=RaycastDetection();
   b2Vec2 p1(x,y);
   b2Vec2 p2 (x+(dir_x*magnitude),y+(dir_y*magnitude));
   world->RayCast(&callback,p1,p2);
   if(callback.hit){
      b2Vec2 norm=callback.hit_normal;
      b2Vec2 point=callback.hit_point;
      b2Fixture* fixture=callback.hit_fixture;
      res.hit=true;
      res.normal=norm;
      b2Vec2 pt(Converter::MetterToPixel(point.x),Converter::MetterToPixel(point.y));
      res.hit_point=pt;

      if(fixture!=NULL){
        b2Body* bd=fixture->GetBody();
        if(bd!=NULL){
            uint16_t entity=static_cast<uint16_t>(bd->GetUserData().pointer);
            res.hit_entity=entity;
        }
      }
   }
   return res;
}

//Add Collision Event from Collision Contact Event
void Physic_Engine_System::add_CollisionEvent(uint16_t entityA,uint16_t entityB, float x_norm,float y_norm,bool begin_ev){
  collisions_queue.push_back({entityA,entityB,x_norm,y_norm,begin_ev});
}

//Add Trigger Event from Collision Contact Event
void Physic_Engine_System::add_TriggerEvent(uint16_t entityA,uint16_t entityB,bool begin_ev){
   triggers_queue.push_back({entityA,entityB,begin_ev});
}

