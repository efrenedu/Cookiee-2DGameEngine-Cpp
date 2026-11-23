#include "EngineContext.h"

EngineContext* EngineContext::instance=NULL;
//Build the Singleton Engine Context
void EngineContext::init(){
  if(!instance){
    instance=new EngineContext();
    instance->finish_game=false;
    instance->pause_game=false;
    instance->next_scene=0xFF;
    instance->scene_index=0xFF;
  }
}

//get the Singleton of EngineContext
EngineContext& EngineContext::get(){
   return *instance;
}

void EngineContext:: set_pause(bool val){
   pause_game=val;
}
void EngineContext::exit_game(){
    finish_game=true;
}
void EngineContext::load_scene(uint8_t index){
   next_scene=index;
}


//Getter Methods
bool& EngineContext::is_paused(){
   return pause_game;
}

bool& EngineContext::requiere_finish(){
    return finish_game;
}
uint8_t& EngineContext::get_next_scene(){
    return next_scene;
}
UI_event_System& EngineContext::get_uiEvent(){
   return ui_event;
}
Physic_Engine_System&  EngineContext::get_physic(){
   return physic_sys;
}
Input_System& EngineContext:: get_input(){
   return input_sys;
}
Game_Config_System& EngineContext::get_config(){
   return config;
}
Render_DrawSystem& EngineContext::get_drawContext(){
   return draw_sys;
}
Screen_System& EngineContext::get_screen(){
   return screen_sys;
}
