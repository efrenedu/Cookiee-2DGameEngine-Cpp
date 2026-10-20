#include <stdio.h>
#include<Physic_Engine.h>
#include<Component.h>
#include <cctype>
#include<chrono>
#include <Timer.h>
#include <Converter.h>
#include <Render_DrawSystem.h>
#include<Scene.h>

bool playing=true;
int filter_events(void* user_data,SDL_Event* ev);

//Method Main
int main( int argc, char* argv[] ){
    Scene target_scene;
    Render_DrawSystem render_sys;
    Physic_Engine_System physic_engine;
    AudioEngine engine_audio;
    //Initialize Basic Systems and SDL
    if( SDL_Init( SDL_INIT_VIDEO| SDL_INIT_AUDIO|SDL_INIT_TIMER |SDL_INIT_EVENTS) < 0 ){
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 0;
    }
    if(render_sys.Init_System()==false){
        cout<<"Fail Init Render Draw System"<<endl;
        render_sys.Free_System();
        return 0;
    }
    if(engine_audio.Init()==false){
        cout<<"fail init Audio System"<<endl;
        engine_audio.free_memory();
        render_sys.Free_System();
        return 0;
    }
    //Initialize the Rest of Systems
    Game_Config_System game_config;
    if(game_config.Init("assets/config.json")==false){
          cout<<"Error Reading Config Data"<<endl;
          engine_audio.free_memory();
          render_sys.Free_System();
          return 0;
    }
    if(render_sys.Init_Main_Windows(game_config.get_Config_Data("Windows_Config"))==false){
          engine_audio.free_memory();
          render_sys.Free_System();
          return 0;
    }
    target_scene.Init(engine_audio.get_engine(),game_config.get_allConfig());
    Screen_System screen;
    screen.Init(render_sys.get_win());
    physic_engine.Init(&target_scene,9.8,0.0,-1.0);
    Timer_System timer_scene;
    Input_System input_sys;
    input_sys.init(0.01f,game_config.get_Config_Data("Input_Config"));
    timer_scene.Init(60);
    SDL_Event evento;
    SDL_SetEventFilter(filter_events,NULL);

    //Init Main Loop of Game
    while(playing==true){
        if(target_scene.require_loadScene()){
            target_scene.load_sceneSource(render_sys.get_win_render());
            physic_engine.loadWorld(&target_scene);
        }
        //Updatr Scene and Systems
        timer_scene.update();
        physic_engine.update_world(timer_scene.get_delta_time(),&target_scene);
        target_scene.update_scene(&render_sys,timer_scene.get_delta_time(),&input_sys,&game_config);
        physic_engine.update(&target_scene);
        render_sys.draw_scene(&target_scene);
        input_sys.update(timer_scene.get_delta_time());
        //Update Inputs Events
        if( SDL_PollEvent( &evento )) {
            input_sys.get_event(evento,timer_scene.get_delta_time());
            switch(evento.type){
                case SDL_QUIT:{
                    playing=false;
                    break;
                }
                default:break;
            }
        }
        SDL_Delay( timer_scene.get_ticks_frame());
    }
    //free SDL Pointers and Systems from memory
    physic_engine.clear_world();
    target_scene.free_memory();
    engine_audio.free_memory();
    render_sys.Free_System();
    input_sys.free_system();
    game_config.free_system();
    SDL_Quit();
    return 0;
}

//filter events of Engine
int filter_events(void* user_data,SDL_Event* ev){

   if( ev->type==SDL_KEYDOWN || ev->type==SDL_KEYUP){
       return 1;
   }
   else if(ev->type==SDL_MOUSEBUTTONDOWN ||ev->type==SDL_MOUSEBUTTONUP || ev->type==SDL_MOUSEMOTION ){
      return 1;
   }
   else if(ev->type==SDL_QUIT){
       return 1;
   }
   return 0;
}
