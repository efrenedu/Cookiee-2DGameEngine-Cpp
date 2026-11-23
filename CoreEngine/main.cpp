#include <stdio.h>
#include<Physic_Engine.h>
#include<Component.h>
#include <cctype>
#include<chrono>
#include <Timer.h>
#include <Converter.h>
#include <Render_DrawSystem.h>
#include<Scene.h>
#include<EngineContext.h>

bool playing=true;
int filter_events(void* user_data,SDL_Event* ev);

//Method Main
int main( int argc, char* argv[] ){
    Scene target_scene;
    AudioEngine engine_audio;
    EngineContext::init();

    //Initialize Basic Systems and SDL
    if( SDL_Init( SDL_INIT_VIDEO| SDL_INIT_AUDIO|SDL_INIT_TIMER |SDL_INIT_EVENTS) < 0 ){
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return 0;
    }
    if(EngineContext::get().get_drawContext().Init_System()==false){
        cout<<"Fail Init Render Draw System"<<endl;
        EngineContext::get().get_drawContext().Free_System();
        return 0;
    }
    if(engine_audio.Init()==false){
        cout<<"fail init Audio System"<<endl;
        engine_audio.free_memory();
        EngineContext::get().get_drawContext().Free_System();
        return 0;
    }
    //Initialize the Rest of Systems
    if(EngineContext::get().get_config().Init("assets/config.json")==false){
          cout<<"Error Reading Config Data"<<endl;
          engine_audio.free_memory();
          EngineContext::get().get_drawContext().Free_System();
          return 0;
    }
    if(EngineContext::get().get_drawContext().Init_Main_Windows()==false){
          engine_audio.free_memory();
          EngineContext::get().get_drawContext().Free_System();
          return 0;
    }
    if(target_scene.Init(engine_audio.get_engine())==false){
          engine_audio.free_memory();
          EngineContext::get().get_drawContext().Free_System();
          return 0;
    }
    EngineContext::get().get_screen().Init();
    EngineContext::get().get_physic().Init(&target_scene,9.8,0.0,-1.0);
    Timer_System timer_scene;
    EngineContext::get().get_input().init(0.01f);
    EngineContext::get().get_uiEvent().init(&target_scene);

    timer_scene.Init(60);
    SDL_Event evento;
    SDL_SetEventFilter(filter_events,NULL);

    //Init Main Loop of Game
    while(playing==true){
        if(EngineContext::get().requiere_finish()){
             playing=false;
             continue;
        }
        if(target_scene.require_loadScene()){
            target_scene.load_sceneSource();
            EngineContext::get().get_physic().loadWorld(&target_scene);
        }
        //Update Scene and Systems
        uint64_t start_time=SDL_GetPerformanceCounter();
        timer_scene.update_deltaTime();
        EngineContext::get().get_physic().update_world(timer_scene.get_delta_time(),&target_scene);
        target_scene.update_scene(timer_scene.get_delta_time());
        EngineContext::get().get_physic().update(&target_scene);

        EngineContext::get().get_drawContext().update_requests(timer_scene.get_delta_time());
        EngineContext::get().get_drawContext().draw_scene(&target_scene);
        EngineContext::get().get_input().update(timer_scene.get_delta_time());
        //Update Inputs Events
        if( SDL_PollEvent( &evento )) {
            EngineContext::get().get_input().get_event(evento,timer_scene.get_delta_time());
            switch(evento.type){
                case SDL_QUIT:{
                    playing=false;
                    break;
                }
                default:break;
            }
        }
        timer_scene.Limit_frames(start_time);
    }
    //free SDL Pointers and Systems from memory
    EngineContext::get().get_physic().clear_world();
    target_scene.free_memory();
    engine_audio.free_memory();
    EngineContext::get().get_drawContext().Free_System();
    EngineContext::get().get_input().free_system();
    EngineContext::get().get_config().free_system();
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
