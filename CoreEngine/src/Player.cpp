#include "player.h"
#include "Scene.h"
#include<Input.h>
#include<iostream>

//Register the Player On the System
void Player_System::regiser_player(uint16_t id){
   uint16_t next_id=players.size();
   player_data ply;
   ply.id_player=id;
   players.push_back(ply);
   EntitytoDense[id]=next_id;
}

//Reserve Memory for the System
void Player_System::reserve_memory(uint16_t count_player,uint16_t count_entities,uint16_t fillvalue){

  players.reserve(count_player);
  EntitytoDense.assign(count_entities,fillvalue);
}

//Update the Player by Id
void Player_System::update_player(std::bitset<64> mask,uint16_t id,Scene* target_scene,float dt,Input_System* input_sys){
  player_data* ply=get_player(id);
  if(ply==NULL){
     return;
  }
  if(HasComponentBit(mask,RIGIDBODY_COMP)==false){
       return;
  }
  uint8_t st=input_sys->get_keyState(SDL_SCANCODE_RIGHT);
  RigidBody_Data* rb=target_scene->get_rigidbody_sys()->get_body(id);
  float f_y=0;
  if(input_sys->get_keyState(SDL_SCANCODE_SPACE)==INPUT_DOWN){
     f_y=580*dt;
  }
  float x_f=0;
  if(input_sys->get_keyState(SDL_SCANCODE_LEFT)==INPUT_PRESSED){
     x_f=-1*dt*10;
  }
  else if(st==INPUT_PRESSED){
   //  target_scene->get_rigidbody_sys()->set_velocity(id,30*dt,rb->Last_ForceY+f_y);
     x_f=10*dt;
  }
   target_scene->get_rigidbody_sys()->add_force(id,x_f,f_y);

}

//Get the Player by Id
player_data* Player_System::get_player(uint16_t id){
   if(id<0 || id>=EntitytoDense.size()){
      return NULL;
   }
   return &players[EntitytoDense[id]];
}

//On Collision Event
void Player_System::On_Collision(uint16_t id,uint16_t id_other,float norm_x,float norm_y,bool begin_ev){

}

//On Trigger Event
void Player_System::On_Trigger(uint16_t id,uint16_t id_other,bool begin_ev){

}
