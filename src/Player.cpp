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
void Player_System::update_player(uint32_t mask,uint16_t id,Scene* target_scene,float dt,Input_System* input_sys){
  player_data* ply=get_player(id);
  if(ply==NULL){
     return;
  }
  if(HasComponentBit(mask,RIGIDBODY_COMP)==false){
       return;
  }

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
