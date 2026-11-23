#include "UI_event.h"
#include<lua.hpp>
#include<iostream>
#include<Scene.h>
#include<EngineContext.h>

//Pack the data of Payload
uint64_t pack_Payload(uint16_t id_target,bool state,float val){
  uint64_t packed=0;
  uint32_t floatbits=0;
  memcpy(&floatbits,&val,sizeof(float));
  packed|=static_cast<uint64_t>(id_target)& 0xFFFF;
  packed|=(static_cast<uint64_t>(state?1:0)& 0x1)<<16;
  packed|=(static_cast<uint64_t>(floatbits & 0xFFFFFFFF))<<17;
  return packed;
}

//unpack the data of Payload
void unpack_payload(uint64_t packed,uint16_t& out_id,bool& out_state,float&out_val){
   out_id=static_cast<uint16_t>(packed & 0xFFFF);
   out_state=(packed &(static_cast<uint64_t>(1)<<16))!=0;
   uint32_t float_bits=static_cast<uint32_t>((packed>>17)& 0xFFFFFFFF);
   memcpy(&out_val,&float_bits,sizeof(float));
}

void UI_event_System::init(Scene* target_scene){
    scene_ptr=target_scene;
    //store default events of Event Manager
    cpp_events["Change_Music_Volume"]=[this](const EventPayload& payload){
       this->change_Music_Volume(payload);
    };
    cpp_events["Change_Sfx_Volume"]=[this](const EventPayload& payload){
       this->change_volume(payload);
    };
    cpp_events["Exit_Game"]=[this](const EventPayload& payload){
       this->exit_game(payload);
    };
    cpp_events["Load_Scene"]=[this](const EventPayload& payload){
       this->play_scene(payload);
    };
    cpp_events["Set_UI_Active"]=[this](const EventPayload& payload){
       this->set_UI_active(payload);
    };
}


void UI_event_System::bind_luaEvent(std::string ev_name,std::string lua_func){
  lua_events[ev_name]=lua_func;
}
void UI_event_System::trigget_event(std::string ev_name,const EventPayload& payload){
   if(cpp_events.find(ev_name)!=cpp_events.end()){
      cpp_events[ev_name](payload);
      return;
   }
   if(scene_ptr==NULL){
      return;
   }
   Script_System* script_sys=scene_ptr->get_script_system();
   if(script_sys==NULL){
      return;
   }
   lua_State* lua_st=script_sys->get_lua_state();
   if(lua_events.find(ev_name)!=lua_events.end() && lua_st!=NULL){
      lua_getglobal(lua_st,lua_events[ev_name].c_str());
      if(lua_isfunction(lua_st,-1)){
        if(lua_pcall(lua_st,0,0,0)!=LUA_OK){
           const char* err=lua_tostring(lua_st,-1);
           lua_pop(lua_st,1);
        }
      }
      else{
        lua_pop(lua_st,1);
      }
   }
}

//Pre Defined Events
void UI_event_System::change_Music_Volume(const EventPayload& payload){
    if(payload.sender_type!=UI_EVENT_SENDER_SLIDER){
       return;
    }
    scene_ptr->get_Audio_sys()->set_music_volume(payload.value_data);
}

void UI_event_System::change_volume(const EventPayload& payload){
 if(payload.sender_type!=UI_EVENT_SENDER_SLIDER){
     return;
 }
  scene_ptr->get_Audio_sys()->set_sfx_volume(payload.value_data);
}
void UI_event_System::exit_game(const EventPayload& payload){
    EngineContext::get().exit_game();
}
void UI_event_System::play_scene(const EventPayload& payload){
   scene_ptr->load_scene(payload.value_data);
}
void UI_event_System::set_UI_active(const EventPayload& payload){

  std::bitset<64> mask=scene_ptr->get_mask(payload.value_data);
  if(HasComponentBit(mask,UI_COMP)==false){
     return;
  }
  UI_System* ui_sys=scene_ptr->get_UI_sys();
  UI_header* header=ui_sys->get_UIHeader(payload.value_data);
  if(header==NULL){
     return;
  }
  header->active_state=(payload.state_data==true)?UI_ACTIVE_STATE_ACTIVE:UI_ACTIVE_STATE_INACTIVE;
}
