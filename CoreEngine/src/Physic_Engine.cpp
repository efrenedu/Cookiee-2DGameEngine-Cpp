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
   Physic_Material_System* physic_materials=target_scene->get_physic_material_sys();
   Transform_System* transform_sys=target_scene->get_transform_sys();
   Collider_System* collider_sys=target_scene->get_collider_sys();
   RigidBody_System* rigidbody_sys=target_scene->get_rigidbody_sys();

   uint16_t count_entities=target_scene->get_count_entities();
   for(uint16_t i=0;i<count_entities;i++){
       Transform_Data* transform_entity=transform_sys->get_transform(i);
       uint32_t mask_entity=target_scene->get_mask(i);
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
       float angle=Math_G::Wrap_Angle(Math_G::to_rad(transform_entity->angle))*-1;
       float mass=0.0;
       bddefs.type=b2BodyType::b2_staticBody;
       RigidBody_Data* rigidbody_dat=NULL;
       if(HasComponentBit(mask_entity,components_types::RIGIDBODY_COMP)==true){

          rigidbody_dat=rigidbody_sys->get_body(i);
          if(rigidbody_dat->is_kinematic==true){
              bddefs.type=b2BodyType::b2_kinematicBody;
          }
          else{
              bddefs.type=b2BodyType::b2_dynamicBody;
          }
          bddefs.allowSleep=rigidbody_dat->can_sleep;
          bddefs.gravityScale=rigidbody_dat->gravity_scale;
          bddefs.angle=angle;
          bddefs.bullet=false;
          bddefs.fixedRotation=rigidbody_dat->lock_rotations;
          mass=rigidbody_dat->mass;
          bddefs.linearDamping=rigidbody_dat->damping;
          bddefs.angularDamping=rigidbody_dat->angular_damping;
          bddefs.enabled=rigidbody_dat->active;
       }

       float x=Converter::pixelToMetter(transform_entity->x)+half_sz_x;
       float y=Converter::pixelToMetter(transform_entity->y)-half_sz_y;
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
   while(bd!=NULL){
      uint16_t id=static_cast<uint16_t>(bd->GetUserData().pointer);
      Transform_Data* transform_entity=transform_sys->get_transform(id);
      uint32_t mask=target_scene->get_mask(id);
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
        //   filter_val.maskBits=collider->sensor_mask;
        //   filter_val.categoryBits=collider->category_bits_sensor;
          // fixtures->SetFilterData(filter_val);
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

      //Update Constraints data from Joints
      b2JointEdge* jnts=bd->GetJointList();
      while(jnts!=NULL){
          b2Joint* jnt_target=jnts->joint;
          if(jnt_target!=NULL){
            //Joint_Component* joint_comp=reinterpret_cast<Joint_Component*>(jnt_target->GetUserData().pointer);
            //update_joinData(joint_comp,jnt_target);
          }
          jnts=jnts->next;
      }

      //Update RigidBody and Transform
      if(HasComponentBit(mask,RIGIDBODY_COMP)==true){
          RigidBody_Data* rb=rigidbody_sys->get_body(id);
          if(rb->is_kinematic==true){
              bd->SetType(b2BodyType::b2_kinematicBody);
          }
           else{
              bd->SetType(b2BodyType::b2_dynamicBody);
           }
           bd->SetGravityScale(rb->gravity_scale);
           b2Vec2 final_force=bd->GetLinearVelocity();
           rb->Last_ForceX=final_force.x;
           rb->Last_ForceY=final_force.y;
           float currentRadians=bd->GetAngle();

           transform_entity->accumlated_radians=Math_G::Wrap_AngleAccumulated(currentRadians,transform_entity->lastRadians,transform_entity->accumlated_radians);
           transform_entity->lastRadians=currentRadians;
           float new_angle=Math_G::to_degree(transform_entity->accumlated_radians);
           b2Vec2 new_pos=bd->GetPosition();
           transform_entity->x=Converter::MetterToPixel(new_pos.x-rb->offsetCenterX);
           transform_entity->y=Converter::MetterToPixel(new_pos.y+rb->offsetCenterY);
           transform_entity->angle=new_angle;
      }
      bool change_pos=bd->IsAwake();
      if(change_pos){
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
      uint16_t id=static_cast<uint16_t>(bd->GetUserData().pointer);
      uint32_t mask_target=target_scene->get_mask(id);

      if(HasComponentBit(mask_target,components_types::RIGIDBODY_COMP)==true){
          RigidBody_Data* rb=rigid_body_sys->get_body(id);
          b2Vec2 force_bd=bd->GetLinearVelocity();
          b2Vec2 pos=bd->GetPosition();
          rb->Last_ForceX=force_bd.x;
          rb->Last_ForceY=force_bd.y;
          float angular_vel=bd->GetAngularVelocity();
          rb->last_Angular_Force=angular_vel;
      }
      bd=bd->GetNext();
   }
}

//Add a Joint to the World
void Physic_Engine_System::add_jointsToworld(){

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

//Update Join Data Values
void Physic_Engine_System::update_joinData(b2Joint* jnt){

}

//Make a RayCast and Return a RayCast Hit
RayCastHit Physic_Engine_System::RayCast(float x ,float y ,float dir_x,float dir_y,float magnitude){
   RayCastHit res;
   /*x=Converter::pixelToMetter(x);
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
   }*/
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

/*
void Physic_Engine::define_world(){
   //set initial gravity

   gravity_val.Set(grav_dir.x,grav_dir.y);
   world.SetGravity(gravity_val);

   for(int i=0;i<remove_joint_requests.size();i++){
       remove_joint_requests.at(i)=NULL;
   }
   remove_joint_requests.clear();
   joint_requests.clear();
   b2Body* old_bds=world.GetBodyList();
   while(old_bds!=NULL){
      b2JointEdge* joints=old_bds->GetJointList();
      while(joints!=NULL){
          b2Joint* jnt=joints->joint;
          jnt->GetUserData().pointer=NULL;
          joints=joints->next;
      }
      b2Fixture* old_fixtures=old_bds->GetFixtureList();
      while(old_fixtures!=NULL){
          old_fixtures->GetUserData().pointer=NULL;
          old_fixtures=old_fixtures->GetNext();
      }
      old_bds->GetUserData().pointer=NULL;
      world.DestroyBody(old_bds);
      old_bds=old_bds->GetNext();
   }

   vector<Game_Object*> list_objs=Game_Object_Manager::get_allObjs_Pointers();
   vector<Game_Object*>::iterator it;
   vector<Joint_Component*> joints_pendents;

   for(it=list_objs.begin();it!=list_objs.end();it++){
       Game_Object* g=*it;
       if(g!=NULL){
          Component_Store* comps=g->get_components();
          if(comps!=NULL){
              Transform* transf=comps->get_Transform();
              vector<Collider*> col_list=comps->get_Collider();
              RigidBody* rb=comps->get_rigidBody();
              vector<Joint_Component*> joints_target=comps->get_Joints();
              Proyectil* projectil_comp=comps->get_Proyectil();
              Render* render_comp=comps->get_Render();
              if(transf!=NULL && col_list.size()>0){


                  //get the data for build the body
                  Collider* main_collider=get_mainCollider(col_list);
                  Figure main_fig=main_collider->get_figure();
                  Vector2 halfSz_main_fig=main_fig.get_halfDim();
                  Vector2 pos_obj=transf->Get_Position();
                  Vector2 Halfsz_render=Vector2(0,0);
                  Vector2 center_offset=Vector2(0,0);
                  if(render_comp!=NULL){
                      Vector2 scal=transf->Get_Scale();
                      SDL_Rect r_spr=render_comp->get_sprite().get_rect_sprite();
                      Halfsz_render.x=r_spr.w*scal.x*0.5;
                      Halfsz_render.y=r_spr.h*scal.y*0.5;
                  }

                  //Convert Pixels to Metters format
                  pos_obj.x=Converter::pixelToMetter(pos_obj.x);
                  pos_obj.y=Converter::pixelToMetter(pos_obj.y);
                  if(Halfsz_render.x!=0 && Halfsz_render.y!=0){
                    center_offset=Vector2(Halfsz_render.x,-1*Halfsz_render.y);
                    Halfsz_render.x=Converter::pixelToMetter(Halfsz_render.x);
                    Halfsz_render.y=Converter::pixelToMetter(Halfsz_render.y);
                    pos_obj.x=pos_obj.x+Halfsz_render.x;
                    pos_obj.y=pos_obj.y-Halfsz_render.y;

                  }
                  else{
                     center_offset=Vector2(halfSz_main_fig.x,-1*halfSz_main_fig.y);
                     halfSz_main_fig.x=Converter::pixelToMetter(halfSz_main_fig.x);
                     halfSz_main_fig.y=Converter::pixelToMetter(halfSz_main_fig.y);
                     pos_obj.x=(pos_obj.x+halfSz_main_fig.x);
                     pos_obj.y=(pos_obj.y-halfSz_main_fig.y);
                  }
                  //Build the Body
                  float mass=0.0f;
                  b2BodyDef defs;
                  defs.position.Set(pos_obj.x,pos_obj.y);
                  float angl=Math_G::to_rad(transf->get_angle());
                  angl=Math_G::Wrap_Angle(angl);
                  angl*=-1;
                  defs.angle=angl;
                  defs.userData.pointer= reinterpret_cast<uintptr_t>(g);
                  if(rb!=NULL){
                    defs.type=b2_dynamicBody;
                    defs.fixedRotation=rb->get_lock_Rotation();
                    defs.angularDamping=rb->get_angular_Damping();
                    defs.linearDamping=rb->get_damping();
                    if(rb->is_kinematic()){
                         defs.type=b2_kinematicBody;
                    }
                    mass=rb->get_mass();
                    defs.allowSleep=rb->Permit_Sleep();
                  }
                  if(projectil_comp!=NULL){
                     if(projectil_comp->get_force()>=FORCE_BULLET){
                         defs.bullet=true;
                     }
                  }
                  b2Body* bd=world.CreateBody(&defs);

                  //Build the fixtures
                  for(int i=0;i<col_list.size();i++){
                      Collider* target_col=col_list.at(i);
                      if(target_col!=NULL){
                         target_col->set_centerBd(center_offset);
                         build_fixture(bd,target_col,mass,angl,col_list.size(),i);
                      }
                  }

                  //add Joints to Build After if is Necceary
                  if(joints_target.size()>0){
                      for(int i=0;i<joints_target.size();i++){
                          joints_pendents.push_back(joints_target.at(i));
                      }
                  }
               }

          }
       }
   }

   //build joints of Objects
   for(int i=0;i<joints_pendents.size();i++){
       Joint_Component* temp_joint=joints_pendents.at(i);
       if(temp_joint!=NULL){
          add_joint(temp_joint);
       }
   }
}


//Get the Main Collider of a Body
Collider* Physic_Engine::get_mainCollider(vector<Collider*> list_cols){
   Collider* res=NULL;
   if(list_cols.size()==1){
       return list_cols.at(0);
   }
   for(int i=0;i<list_cols.size();i++){
      Collider* temp_col=list_cols.at(i);
      if(temp_col!=NULL){
          if(temp_col->is_mainCollider()){
              res=list_cols.at(i);
              i=list_cols.size();
          }
      }
   }
   return res;
}


//update the world with objects values
void Physic_Engine::update_world(){

   b2Body* bd=world.GetBodyList();
   while(bd!=NULL){
      Game_Object* g=reinterpret_cast<Game_Object*>(bd->GetUserData().pointer);
      if(g!=NULL){

          Component_Store* comps=g->get_components();
          Transform* transf=NULL;
          Collider* col=NULL;
          RigidBody* rb=NULL;
          Render* render_comp=NULL;
          if(comps!=NULL){
             transf=comps->get_Transform();
             vector<Collider*> col_list=comps->get_Collider();
             col=get_mainCollider(col_list);
             rb=comps->get_rigidBody();
             render_comp=comps->get_Render();
          }
          if(transf!=NULL && col!=NULL){
             b2Fixture* fixtures= bd->GetFixtureList();

             bool active_obj=g->is_active();
             if(active_obj==false || g->is_updated()==false){
                  bd->SetType(b2BodyType::b2_staticBody);
             }
             else{
                if(rb!=NULL){
                    bd->SetAngularDamping(rb->get_angular_Damping());
                    bd->SetLinearDamping(rb->get_damping());
                    if(rb->is_kinematic()){
                        bd->SetType(b2BodyType::b2_kinematicBody);
                    }
                    else if(rb->is_active()==false){
                        bd->SetType(b2BodyType::b2_staticBody);
                    }
                    else{
                        bd->SetType(b2BodyType::b2_dynamicBody);
                    }
                }
                else{
                    bd->SetType(b2BodyType::b2_staticBody);
                }

             }

             //Update Fixture data from Collider
             while(fixtures!=NULL){
                Collider* col_fixture=reinterpret_cast<Collider*>(fixtures->GetUserData().pointer);
                if(col_fixture!=NULL){
                    fixtures->SetSensor(col_fixture->is_trigger());
                    fixtures->SetFriction(col_fixture->get_material().get_friction());
                    fixtures->SetRestitution(col_fixture->get_material().get_restitution());
                    if(active_obj==false || g->is_updated()==false){
                        b2Filter filter_val=fixtures->GetFilterData();
                        filter_val.maskBits=0;
                        filter_val.categoryBits=0;
                        filter_val.groupIndex=-1;
                        fixtures->SetFilterData(filter_val);
                    }
                    else{
                       b2Filter filter_val=fixtures->GetFilterData();
                       filter_val.maskBits=col_fixture->get_mask();
                       filter_val.categoryBits=col_fixture->get_category_bits();
                       filter_val.groupIndex=col_fixture->get_layer_Group();
                       fixtures->SetFilterData(filter_val);
                    }

                }
                fixtures=fixtures->GetNext();
             }
             //Update Constraints data from Joints
             b2JointEdge* jnts=bd->GetJointList();
             while(jnts!=NULL){
                 b2Joint* jnt_target=jnts->joint;
                 if(jnt_target!=NULL){
                     Joint_Component* joint_comp=reinterpret_cast<Joint_Component*>(jnt_target->GetUserData().pointer);
                     update_joinData(joint_comp,jnt_target);
                 }
                 jnts=jnts->next;
             }
             int transf_request=transf->require_change();
             Vector2 offsetBd_Target=col->get_centerBody();

             if(transf_request==REQUEST_TRANSFORM::REQUEST_ALL || transf_request==REQUEST_TRANSFORM::REQUEST_ANGLE){
                  float next_angle=Math_G::to_rad(transf->get_requestedAngle())*-1;
                  b2Vec2 requested=bd->GetPosition();
                  if(transf_request!=REQUEST_TRANSFORM::REQUEST_ANGLE ){
                     Vector2 temp_offset=Vector2(0,0);
                     temp_offset.x=Converter::pixelToMetter(offsetBd_Target.x);
                     temp_offset.y=Converter::pixelToMetter(offsetBd_Target.y);
                     Vector2 requested_pos_transf=transf->get_requestedPosition();
                     requested_pos_transf.x=Converter::pixelToMetter(requested_pos_transf.x);
                     requested_pos_transf.y=Converter::pixelToMetter(requested_pos_transf.y);
                     requested_pos_transf.x=requested_pos_transf.x+temp_offset.x;
                     requested_pos_transf.y=requested_pos_transf.y+temp_offset.y;
                     requested.Set(requested_pos_transf.x,requested_pos_transf.y);
                  }
                  bd->SetTransform(requested,next_angle);
                  transf->clear_requestChange();

             }
             b2Vec2 new_pos=bd->GetPosition();
             new_pos.x=Converter::MetterToPixel(new_pos.x);
             new_pos.y=Converter::MetterToPixel(new_pos.y);

            // Vector2 half_szCol=col->get_figure().get_halfDim();
             new_pos.x=new_pos.x-offsetBd_Target.x;
             new_pos.y=new_pos.y-offsetBd_Target.y;

             if(rb!=NULL){

                Vector2 impulse=rb->get_ImpulseForce();
                float grav_scale=rb->get_gravity();
                float angular_impulse=rb->get_AngularImpulse();
                if(rb->require_newForce()){

                    Vector2 force_rb=rb->get_LinearForce();
                    b2Vec2 linear_force;
                    linear_force.Set(force_rb.x,force_rb.y);
                    bd->SetLinearVelocity(linear_force);
                }
                if(impulse.x!=0 || impulse.y!=0){
                      b2Vec2 impulse_F;
                      impulse_F.Set(impulse.x,impulse.y);
                      if(rb->get_lock_Rotation()==true){
                         bd->ApplyLinearImpulseToCenter(impulse_F,true);
                      }
                      else{
                          Vector2 half_sz=col->get_figure().get_halfDim();
                          half_sz.x=Converter::pixelToMetter(half_sz.x);
                          half_sz.y=Converter::pixelToMetter(half_sz.y);

                          b2Vec2 offset;
                          offset.Set(half_sz.x,half_sz.y);
                          b2Vec2 center=bd->GetWorldCenter()+offset;
                          bd->ApplyLinearImpulse(impulse_F,center,true);
                      }


                }
                if(rb->require_newAngularVel()){
                    float next_angularVel=rb->get_AngularVelocity();
                    bd->SetAngularVelocity(next_angularVel);
                }
                if(angular_impulse!=0){
                     bd->ApplyAngularImpulse(angular_impulse,true);
                }
                rb->clear_Forces();
                bd->SetGravityScale(grav_scale);
                b2Vec2 final_force=bd->GetLinearVelocity();
                rb->set_bodyForce(Vector2(final_force.x,final_force.y));
             }

             float new_angle=Math_G::Wrap_Angle(bd->GetAngle()*-1);
             new_angle=Math_G::to_degree(new_angle);

             b2Vec2 new_pos2=bd->GetPosition();
             g->update_transform(Vector2(new_pos.x,new_pos.y),new_angle);
          }
      }
      bd=bd->GetNext();
   }

}

//raycast
RayCastHit Physic_Engine::RayCast(Vector2 start,Vector2 dir,float magnitud) {

   start.x=Converter::pixelToMetter(start.x);
   start.y=Converter::pixelToMetter(start.y);
   RayCastHit res;
   res.hit=false;
   res.hit_obj=NULL;
   RaycastDetection callback=RaycastDetection();
   b2Vec2 p1(start.x,start.y);
   Vector2 end_p=start+(Vector2(dir.x*magnitud,dir.y*magnitud));
   b2Vec2 p2(end_p.x,end_p.y);
   world.RayCast(&callback,p1,p2);
   if(callback.hit){
      b2Vec2 norm=callback.hit_normal;
      b2Vec2 point=callback.hit_point;
      b2Fixture* fixture=callback.hit_fixture;
      res.hit=true;
      res.normal=Vector2(norm.x,norm.y);
      Vector2 pt=Vector2(point.x,point.y);
      pt.x=Converter::MetterToPixel(pt.x);
      pt.y=Converter::MetterToPixel(pt.y);
      res.hit_point=pt;

      if(fixture!=NULL){
        b2Body* bd=fixture->GetBody();
        if(bd!=NULL){
            Game_Object* g=reinterpret_cast<Game_Object*>(bd->GetUserData().pointer);
            res.hit_obj=g;
        }
      }
   }
   return res;

}


// Request to Add a Joint
void Physic_Engine::add_joint(Joint_Component* joint_target){
   if(joint_target==NULL){
      return;
   }
   int joint_t=joint_target->get_type();
   string child_name=joint_target->get_child();
   string parent_name=joint_target->get_parent();
   b2JointType typ;
   b2JointDef joint_defs;
   joint_defs.userData.pointer=reinterpret_cast<uintptr_t>(joint_target);
   joint_defs.collideConnected=joint_target->get_collide();
   switch(joint_t){
     case JOINT_TYPES::DISTANCE_JOINT:{
         typ=b2JointType::e_distanceJoint;
         joint_defs.type=b2JointType::e_distanceJoint;
         break;
     }
     case JOINT_TYPES::REVOLUTE_JOINT:{
           typ=b2JointType::e_revoluteJoint;
           joint_defs.type=b2JointType::e_revoluteJoint;
           break;
     }
     case JOINT_TYPES::PRISMATIC_JOINT:{
          typ=b2JointType::e_prismaticJoint;
          joint_defs.type=b2JointType::e_prismaticJoint;
          break;
     }
     case JOINT_TYPES::PULLEY_JOINT:{
          typ=b2JointType::e_pulleyJoint;
          joint_defs.type=b2JointType::e_pulleyJoint;
          break;
     }
     case JOINT_TYPES::WELD_JOINT:{
        typ=b2JointType::e_weldJoint;
        joint_defs.type=b2JointType::e_weldJoint;
        break;
     }
     default:{
        break;
     }
   }

   b2Body* bd_child=NULL;
   b2Body* bd_parent=NULL;
   b2Body* body_list=world.GetBodyList();
   bool valid_child=true;
   bool valid_bodies=false;
   while(body_list!=NULL ){
      Game_Object* g=reinterpret_cast<Game_Object*>(body_list->GetUserData().pointer);
      if(g!=NULL){
         if(g->get_name()==child_name){
             bd_child=body_list;
         }
         else if(g->get_name()==parent_name){
              bd_parent=body_list;
              b2JointEdge* parent_joints=bd_parent->GetJointList();
              while(parent_joints!=NULL){
                  b2Joint* temp_joint=parent_joints->joint;
                  if(temp_joint!=NULL){
                     b2Body* temp_bd=temp_joint->GetBodyB();
                     Game_Object* temp_obj=reinterpret_cast<Game_Object*>(temp_bd->GetUserData().pointer);
                     if(temp_bd!=NULL && temp_joint->GetType()==typ){
                        if(temp_obj->get_name()==child_name){
                            valid_child=false;
                        }
                     }

                  }
                  parent_joints=parent_joints->next;
              }
         }
      }
      body_list=body_list->GetNext();
   }

   if(bd_parent!=NULL && bd_child!=NULL){
      valid_bodies=true;
   }
   if(valid_child && valid_bodies){
      joint_defs.bodyA=bd_parent;
      joint_defs.bodyB=bd_child;
      joint_requests.push_back(joint_defs);
   }
}

//Add Joint Request to the world
void Physic_Engine::add_jointsToworld(){
    if(!world.IsLocked()){
       vector<b2JointDef>::iterator it;
       for(it=joint_requests.begin();it!=joint_requests.end();it++){
           b2JointDef request=*it;
           b2Body* bdA=request.bodyA;
           b2Body* bdB=request.bodyB;
           Joint_Component* joint_comp=reinterpret_cast<Joint_Component*>(request.userData.pointer);
           if(joint_comp!=NULL ){
               Vector2 OffsetanchorA=joint_comp->get_anchorOffset_Parent();
               Vector2 OffsetanchorB=joint_comp->get_anchorOffset_Child();
               Vector2 groundOffsetA=joint_comp->get_groundOffset_Parent();
               Vector2 groundOffsetB=joint_comp->get_groundOffset_Child();
               OffsetanchorA.x=Converter::pixelToMetter(OffsetanchorA.x);
               OffsetanchorA.y=Converter::pixelToMetter(OffsetanchorA.y);
               OffsetanchorB.x=Converter::pixelToMetter(OffsetanchorB.x),
               OffsetanchorB.y=Converter::pixelToMetter(OffsetanchorB.y);
               groundOffsetA.x=Converter::pixelToMetter(groundOffsetA.x);
               groundOffsetA.y=Converter::pixelToMetter(groundOffsetA.y);
               groundOffsetB.x=Converter::pixelToMetter(groundOffsetB.x);
               groundOffsetB.y=Converter::pixelToMetter(groundOffsetB.y);
               b2Vec2 anchorA(0,0);
               b2Vec2 anchorB(0,0);
               b2Vec2 groundA(0,0);
               b2Vec2 groundB(0,0);
               b2Vec2 posA(0,0);
               b2Vec2 posB(0,0);
               if(bdA!=NULL && bdB!=NULL){
                  posA=bdA->GetPosition();
                  posB=bdB->GetPosition();
                  anchorA=bdA->GetLocalCenter();
                  anchorB=bdB->GetLocalCenter();
                  groundA.Set(posA.x+groundOffsetA.x,posA.y+groundOffsetA.y);
                  groundB.Set(posB.x+groundOffsetB.x,posB.y+groundOffsetB.y);
                  anchorA.x=anchorA.x+OffsetanchorA.x;
                  anchorA.y=anchorA.y+OffsetanchorA.y;
                  anchorB.x=anchorB.x+OffsetanchorB.x;
                  anchorB.y=anchorB.y+OffsetanchorB.y;
               }
               switch(request.type){
                   case b2JointType::e_distanceJoint:{
                       b2DistanceJointDef joint_defs;
                       joint_defs.bodyA=request.bodyA;
                       joint_defs.bodyB=request.bodyB;
                       joint_defs.userData.pointer=request.userData.pointer;
                       joint_defs.collideConnected=request.collideConnected;
                       joint_defs.type=request.type;
                       joint_defs.localAnchorA=anchorA;
                       joint_defs.localAnchorB=anchorB;

                       joint_defs.minLength=Converter::pixelToMetter(joint_comp->get_minLimit());
                       joint_defs.maxLength=Converter::pixelToMetter(joint_comp->get_maxLimit());
                       if(joint_defs.minLength<=0){
                          joint_defs.minLength=0.01;
                       }
                       float damping=joint_comp->get_damping();
                       float stiffness=joint_comp->get_stiffness();
                       joint_defs.damping=damping;
                       joint_defs.stiffness=stiffness;
                       b2Vec2 dist_bds=posA-posB;
                       Vector2 dist_vect=Vector2(dist_bds.x,dist_bds.y);
                       joint_defs.length=dist_vect.magnitud();
                       world.CreateJoint(&joint_defs);
                       break;
                   }
                    case b2JointType::e_revoluteJoint:{
                       b2RevoluteJointDef joint_defs;
                       joint_defs.bodyA=request.bodyA;
                       joint_defs.bodyB=request.bodyB;
                       joint_defs.userData.pointer=request.userData.pointer;
                       joint_defs.collideConnected=request.collideConnected;
                       joint_defs.type=request.type;
                       joint_defs.localAnchorA=anchorA;
                       joint_defs.localAnchorB=anchorB;
                       joint_defs.enableMotor=false;
                       joint_defs.enableLimit=false;
                       if(joint_comp->apply_limits()){
                           float min_angle=Math_G::Wrap_Angle(Math_G::to_rad(joint_comp->get_minLimit())*-1);
                           float max_angle=Math_G::Wrap_Angle(Math_G::to_rad(joint_comp->get_maxLimit())*-1);
                           float angl=Math_G::Wrap_Angle(Math_G::to_rad(joint_comp->get_Angle()))*-1;

                           joint_defs.enableLimit=true;
                           joint_defs.lowerAngle=min_angle;
                           joint_defs.upperAngle=max_angle;
                           joint_defs.referenceAngle=angl;
                       }
                       if(joint_comp->get_speed()>0 && joint_comp->get_maxSpeed()>0){
                            float max_torque=joint_comp->get_maxSpeed();
                            float speed=Math_G::Wrap_Angle(Math_G::to_rad(joint_comp->get_speed()));
                            joint_defs.enableMotor=true;
                            joint_defs.maxMotorTorque=max_torque;
                            joint_defs.motorSpeed=speed;
                       }
                       world.CreateJoint(&joint_defs);
                       break;
                   }
                   case b2JointType::e_pulleyJoint:{
                       b2PulleyJointDef joint_defs;
                       joint_defs.bodyA=request.bodyA;
                       joint_defs.bodyB=request.bodyB;
                       joint_defs.userData.pointer=request.userData.pointer;
                       joint_defs.collideConnected=request.collideConnected;
                       joint_defs.type=request.type;
                       joint_defs.localAnchorA=anchorA;
                       joint_defs.localAnchorB=anchorB;
                       joint_defs.lengthA=Converter::pixelToMetter(joint_comp->get_maxLimit());
                       joint_defs.lengthB=Converter::pixelToMetter(joint_comp->get_minLimit());
                       float ratio_val=joint_comp->get_ratio();
                       if(ratio_val>0){
                          joint_defs.ratio=ratio_val;
                       }
                       joint_defs.groundAnchorA=groundA;
                       joint_defs.groundAnchorB=groundB;
                       world.CreateJoint(&joint_defs);
                       break;
                   }
                   case b2JointType::e_prismaticJoint:{
                       b2PrismaticJointDef joint_defs;
                       joint_defs.bodyA=request.bodyA;
                       joint_defs.bodyB=request.bodyB;
                       joint_defs.userData.pointer=request.userData.pointer;
                       joint_defs.collideConnected=request.collideConnected;
                       joint_defs.type=request.type;
                       joint_defs.localAnchorA=anchorA;
                       joint_defs.localAnchorB=anchorB;
                       Vector2 axis_joint=joint_comp->get_axis();
                       b2Vec2 axis(axis_joint.x,axis_joint.y);
                       joint_defs.enableMotor=false;
                       joint_defs.enableLimit=false;
                       joint_defs.localAxisA=axis;
                       if(joint_comp->apply_limits()){

                          float min_translation=Converter::pixelToMetter(joint_comp->get_minLimit());
                          float max_translation=Converter::pixelToMetter(joint_comp->get_maxLimit());
                          joint_defs.lowerTranslation=min_translation;
                          joint_defs.upperTranslation=max_translation;
                          joint_defs.enableLimit=true;
                       }
                       if(joint_comp->get_speed()>0 &&joint_comp->get_maxSpeed()>0){
                             float max_speed=joint_comp->get_maxSpeed();
                             float speed=joint_comp->get_speed();
                             joint_defs.enableMotor=true;
                             joint_defs.maxMotorForce=max_speed;
                             joint_defs.motorSpeed=speed;
                       }
                       if(joint_comp->get_Angle()){
                            float angl=joint_comp->get_Angle();
                            joint_defs.referenceAngle=Math_G::Wrap_Angle(Math_G::to_rad(angl))*-1;
                       }
                       world.CreateJoint(&joint_defs);
                       break;
                   }
                   case b2JointType::e_weldJoint:{
                        b2WeldJointDef joint_defs;
                        joint_defs.bodyA=request.bodyA;
                        joint_defs.bodyB=request.bodyB;
                        joint_defs.userData.pointer=request.userData.pointer;
                        joint_defs.collideConnected=request.collideConnected;
                        joint_defs.type=request.type;
                        float angl=joint_comp->get_Angle();
                        if(angl>=0){
                            joint_defs.referenceAngle=Math_G::Wrap_Angle(Math_G::to_rad(angl))*-1;
                        }
                        float stiffness=joint_comp->get_stiffness();
                        float damping=joint_comp->get_damping();
                        joint_defs.stiffness=stiffness;
                        joint_defs.damping=damping;
                        world.CreateJoint(&joint_defs);
                        break;
                   }

                   default:{
                      break;
                   }
               }
           }
       }
       joint_requests.clear();
       for(int i=0;i<remove_joint_requests.size();i++){
          b2Joint* request=remove_joint_requests.at(i);
          if(request!=NULL){
              b2Body* A_bd=request->GetBodyA();
              b2Body* B_bd=request->GetBodyB();
              Joint_Component* jnt_dat=reinterpret_cast<Joint_Component*>(request->GetUserData().pointer);
              string child_name="";
              string parent_name="";
              int type_j=0;
              if(jnt_dat!=NULL){
                 child_name=jnt_dat->get_child();
                 parent_name=jnt_dat->get_parent();
                 type_j=jnt_dat->get_type();
              }
              request->GetUserData().pointer=NULL;
              if(A_bd!=NULL){
                Game_Object* A_obj=reinterpret_cast<Game_Object*>(A_bd->GetUserData().pointer);
                if(A_obj!=NULL){
                     A_obj->remove_Joint(child_name,parent_name,type_j);
                }
              }
              if(B_bd!=NULL){
                Game_Object* B_obj=reinterpret_cast<Game_Object*>(B_bd->GetUserData().pointer);
                if(B_obj!=NULL){
                     B_obj->remove_Joint(child_name,parent_name,type_j);
                }
              }
              world.DestroyJoint(request);
              request=NULL;
          }
       }
       remove_joint_requests.clear();
    }
}
//request remove a joint
void Physic_Engine::remove_joint(Joint_Component* joint_target){
   if(joint_target==NULL){
      return;
   }
   int joint_t=joint_target->get_type();
   string parent_name=joint_target->get_parent();
   string child_name=joint_target->get_child();
   b2JointType typ;

   switch(joint_t){
     case JOINT_TYPES::DISTANCE_JOINT:{
         typ=b2JointType::e_distanceJoint;
         break;
     }
     case JOINT_TYPES::REVOLUTE_JOINT:{
        typ=b2JointType::e_revoluteJoint;
        break;
     }
     case JOINT_TYPES::PRISMATIC_JOINT:{
        typ=b2JointType::e_prismaticJoint;
        break;
     }
     case JOINT_TYPES::PULLEY_JOINT:{
          typ=b2JointType::e_pulleyJoint;
          break;
     }

     case JOINT_TYPES::WELD_JOINT:{
        typ=b2JointType::e_weldJoint;
        break;
           }
     default:{
        break;
     }
   }

   b2Joint* jnt=NULL;
   b2Body* body_list=world.GetBodyList();
   while(body_list!=NULL){

      Game_Object* g=reinterpret_cast<Game_Object*>(body_list->GetUserData().pointer);
      if(g!=NULL && jnt==NULL){
           if(g->get_name()==parent_name){
              b2JointEdge* parent_joints=body_list->GetJointList();
              while(parent_joints!=NULL ){
                  b2Joint* temp_joint=parent_joints->joint;
                  if(temp_joint!=NULL){
                       b2Body* child_bd=temp_joint->GetBodyB();
                       bool correct_child=false;
                       if(child_bd!=NULL){
                           Game_Object* g_child=reinterpret_cast<Game_Object*>(child_bd->GetUserData().pointer);
                           if(g_child!=NULL){
                              if(g_child->get_name()==child_name){
                                  correct_child=true;
                              }
                           }
                       }
                       if(temp_joint->GetType()==typ && correct_child){
                           jnt=temp_joint;
                       }
                  }
                  parent_joints=parent_joints->next;
              }
           }
      }
      body_list=body_list->GetNext();
   }
   if(jnt!=NULL){
      remove_joint_requests.push_back(jnt);
   }
}

//Update Joint data from Component Associated
void Physic_Engine::update_joinData(Joint_Component* jnt_comp,b2Joint* jnt){
  if(jnt_comp==NULL || jnt==NULL){
      return;
  }
  if(jnt_comp->apply_limits()==false || jnt_comp->require_update()==false){
      return;
  }

  switch(jnt_comp->get_type()){
      case JOINT_TYPES::DISTANCE_JOINT:{
         float min_dist=Converter::pixelToMetter(jnt_comp->get_minLimit());
         float max_dist=Converter::pixelToMetter(jnt_comp->get_maxLimit());
         b2DistanceJoint* dist_jnt=(b2DistanceJoint*)jnt;
         if(dist_jnt!=NULL){
              dist_jnt->SetMaxLength(max_dist);
              dist_jnt->SetMinLength(min_dist);
              dist_jnt->SetDamping(jnt_comp->get_damping());
              dist_jnt->SetStiffness(jnt_comp->get_stiffness());
         }
         break;
      }
      case JOINT_TYPES::PRISMATIC_JOINT:{
           float min_desplz=Converter::pixelToMetter(jnt_comp->get_minLimit());
           float max_desplz=Converter::pixelToMetter(jnt_comp->get_maxLimit());
           b2PrismaticJoint* primatic_jnt=(b2PrismaticJoint*)jnt;
           if(primatic_jnt!=NULL){
               primatic_jnt->SetLimits(min_desplz,max_desplz);
               primatic_jnt->SetMaxMotorForce(jnt_comp->get_speed());
               primatic_jnt->SetMotorSpeed(jnt_comp->get_maxSpeed());
           }
           break;
      }

      case JOINT_TYPES::REVOLUTE_JOINT:{
         b2RevoluteJoint* revolute_jnt=(b2RevoluteJoint*)jnt;
         float min_angle=Math_G::Wrap_Angle(Math_G::to_rad(jnt_comp->get_minLimit()))*-1;
         float max_angle=Math_G::Wrap_Angle(Math_G::to_rad(jnt_comp->get_maxLimit()))*-1;

         if(revolute_jnt!=NULL){
             revolute_jnt->SetLimits(min_angle,max_angle);
             revolute_jnt->SetMotorSpeed(Math_G::Wrap_Angle(Math_G::to_rad(jnt_comp->get_speed()))*-1);
             revolute_jnt->SetMaxMotorTorque(jnt_comp->get_maxSpeed());
         }
         break;
      }
      default:{

        break;
      }
  }
  jnt_comp->clear_requests();
}


//Return True if the world is Locked
bool Physic_Engine::is_locked(){
   return world.IsLocked();
}

//Change the gravity direction
void Physic_Engine::change_gravDir(Vector2 dir){
   dir=dir.normalize();
   Vector2 next_grav=Vector2(dir.x*gravedad,dir.y*gravedad);
   gravity_val.Set(next_grav.x,next_grav.y);
   world.SetGravity(gravity_val);
}
*/
