#include "Input.h"
#include "hash_support.h"
#include<iostream>
#include<Converter.h>

//Init Input System
void Input_System::init(uint8_t w_time,std::unordered_map<std::string,std::string> key_config){
  key_states.assign(SDL_NUM_SCANCODES,INPUT_NONE);
  keyboard_timers.assign(SDL_NUM_SCANCODES,0.0);
  action_keys.assign(KEY_COUNT-1,0);
  for(const auto& temp_data:key_config){
     std::string temp_id=temp_data.first;
     std::string temp_val=temp_data.second;
     uint8_t id_key=get_key_id(temp_id.c_str());
     if(id_key!=KEY_INVALID){
          action_keys[id_key]=Converter::strTo_UInt(temp_val);
     }
  }
  mouse_timers={0.0f,0.0f,0.0f};
  mouse_y=0;
  mouse_x=0;
  wait_time=w_time;
  mouse_states.assign(3,INPUT_NONE);
  last_keyInput=0xFFFF;


}

//free Input System
void Input_System::free_system(){
    mouse_states.clear();
    mouse_timers.clear();
    keyboard_timers.clear();
    key_states.clear();
}

//Reset Input System
void Input_System::reset(){
    for(uint8_t i=0;i<3;i++){
        mouse_states[i]=INPUT_NONE;
        mouse_timers[i]=0.0f;
    }
    for(uint8_t i=0;i<key_states.size();i++){
        key_states[i]=INPUT_NONE;
        keyboard_timers[i]=0.0f;
    }
    mouse_x=0;
    mouse_y=0;
}

//Update Input System
void Input_System::update(float dt){
   for(uint8_t i=0;i<mouse_timers.size();i++){
       float time_passed=mouse_timers[i];
       if(time_passed<=0){
          continue;
       }
       time_passed+=dt;
       if(time_passed>=wait_time){
          time_passed=0;
          uint8_t state_btn=mouse_states[i];
          switch(state_btn){
              case INPUT_DOWN:{
                mouse_states[i]=INPUT_PRESSED;
                break;
              }
              case INPUT_UP:{
                 mouse_states[i]=INPUT_NONE;
                 break;
              }
              default:break;
          }
       }
       mouse_timers[i]=time_passed;
   }
   for(uint16_t i=0;i<keyboard_timers.size();i++){
       float time_passed=keyboard_timers[i];
       if(time_passed<=0){
          continue;
       }
       time_passed+=dt;
       if(time_passed>=wait_time){
          time_passed=0;
          uint8_t last_state=key_states[i];
          switch(last_state){
              case INPUT_DOWN:{
                key_states[i]=INPUT_PRESSED;
                break;
              }
              case INPUT_UP:{
                 key_states[i]=INPUT_NONE;
                 break;
              }
              default:break;
          }
       }
       keyboard_timers[i]=time_passed;
   }
}

//Interprete SDL Event
void Input_System::get_event(SDL_Event ev,float dt){
  if(ev.type==SDL_MOUSEBUTTONDOWN || ev.type==SDL_MOUSEBUTTONUP || ev.type==SDL_MOUSEMOTION){
      mouse_x=ev.motion.x;
      mouse_y=ev.motion.y;
      int8_t index=-1;
      switch(ev.button.button){
         case SDL_BUTTON_LEFT: index=LEFT_BUTTON;break;
         case SDL_BUTTON_RIGHT:index=RIGHT_BUTTON;break;
         case SDL_BUTTON_MIDDLE:index=CENTER_BUTTON;break;
         default:break;
      }
      uint8_t new_state=INPUT_NONE;
      switch(ev.type){
          case SDL_MOUSEBUTTONDOWN:{
               new_state=INPUT_DOWN;
               break;
          }
          case SDL_MOUSEBUTTONUP:{
              new_state=INPUT_UP;
              break;
          }
          default:break;
      }
      if(index<0 ||new_state==INPUT_NONE){
          return;
      }
      uint8_t last_state=mouse_states[index];
      if(mouse_timers[index]>0.0){
          if(last_state==new_state || (last_state==INPUT_PRESSED && new_state==INPUT_DOWN)){
              return;
          }
      }
      mouse_timers[index]=dt;
      mouse_states[index]=new_state;

  }
  else if(ev.type==SDL_KEYDOWN || ev.type==SDL_KEYUP){
       const Uint8* keys=SDL_GetKeyboardState(NULL);
       uint8_t last_state=key_states[ev.key.keysym.scancode];
       if(ev.type==SDL_KEYUP){
            if(last_state==INPUT_PRESSED || last_state==INPUT_DOWN){
                key_states[ev.key.keysym.scancode]=INPUT_UP;
                keyboard_timers[ev.key.keysym.scancode]=dt;
            }
        }
        else{
            if(last_state==INPUT_NONE){
                key_states[ev.key.keysym.scancode]=INPUT_DOWN;
                keyboard_timers[ev.key.keysym.scancode]=dt;
            }
        }

  }
  else if(ev.type==SDL_JOYBUTTONDOWN || ev.type==SDL_JOYBUTTONUP || ev.type==SDL_JOYAXISMOTION){

  }

}

//get key state by Name
uint8_t Input_System:: get_keyState(uint16_t code){
   if(code>=key_states.size() || code<0){
       return INPUT_NONE;
   }
   return key_states[code];
}

//Get Key state by SDL_Scancode
uint8_t Input_System:: get_keyState(SDL_Scancode code){

   if(code>=key_states.size() || code<0){
       return INPUT_NONE;
   }
   return key_states[code];
}

//Get Mouse Data of Indicated Button
uint8_t Input_System::get_mouseState(uint8_t button){
    if(button<0 || button>=mouse_states.size()){
          return INPUT_NONE;
    }
    return mouse_states[button];
}

//Get the Mouse Position
std::vector<uint16_t> Input_System::get_mousePosition(){
   std::vector<uint16_t> res={mouse_x,mouse_y};
   return res;
}

//Get the Scan Key Code from the Action of Player
uint16_t Input_System::get_PlayerKey(uint8_t action_player){
   if(action_player>=KEY_COUNT-1){
      return key_states.size();
   }
   return action_keys[action_player];
}

//Get the Last key as string
std::string Input_System::get_lastKey(){
    return "";
}
