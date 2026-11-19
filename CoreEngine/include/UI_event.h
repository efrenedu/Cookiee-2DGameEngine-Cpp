#ifndef UI_EVENT_H
#define UI_EVENT_H
#include<cstdint>
#include<vector>
#include<string>
#include<unordered_map>
#include<functional>
#include<cstdint>
#include<cstring>
class Scene;
class UI_System;
class Script_System;

//Sender Type of Event Payload
enum UI_EVENT_SENDER:uint8_t{
   UI_EVENT_SENDER_BUTTON=0,
   UI_EVENT_SENDER_SLIDER=1,
   UI_EVENT_SENDER_LISTBOX=2,
   UI_EVENT_SENDER_COMBOBOX=3,
   UI_EVENT_SENDER_SCROLLBAR=4,
   UI_EVENT_SENDER_CHECKBUTTON=5,
   UI_EVENT_SENDER_RADIOBUTTON=6,
   UI_EVENT_SENDER_PANEL=7
};

//for send the data of args of Events from the Widgets
struct EventPayload{
   std::string text_data;
   uint16_t entity_target;
   float value_data;
   uint8_t sender_type;
   bool state_data;

};



struct UI_event_System
{
    public:
       void init(Scene* target_scene);
       void set_luaEvents(std::unordered_map<std::string,std::string> list_lua_Events);
       void bind_luaEvent(std::string ev_name,const std::string lua_func);
       void trigget_event(std::string ev_name,const EventPayload& payload);
    private:
       void change_volume(const EventPayload& payload);
       void change_Music_Volume(const EventPayload& payload);
       void set_UI_active(const EventPayload& payload);
       void exit_game(const EventPayload& payload);
       void play_scene(const EventPayload& payload);
       std::unordered_map<std::string ,std::function<void(const EventPayload&)>> cpp_events;
       std::unordered_map<std::string,std::string>lua_events;
       Scene* scene_ptr=nullptr;
};

//Support Function for EventPayload Pack/Unpack
uint64_t pack_Payload(uint16_t id_target,bool state,float val);
void unpack_payload(uint64_t packed,uint16_t& out_id,bool& out_state,float&out_val);

#endif // UI_EVENT_H
