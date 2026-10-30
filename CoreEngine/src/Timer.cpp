#include "Timer.h"
#include "iostream"
#include <SDL.h>

//Update the Delta Time
void Timer_System::update_deltaTime(){
   if(tick_perFrame==0){
       if(target_fps>0){
          tick_perFrame=(1000.0f/(float)target_fps);
       }
   }
   if(counter_frecuency==-1){
     counter_frecuency=SDL_GetPerformanceFrequency();
     last_Counter=SDL_GetPerformanceCounter();
   }
   last_Counter=Now_Counter;
   Now_Counter=SDL_GetPerformanceCounter();
   if(counter_frecuency>0){
     delta_counter=(float)((Now_Counter-last_Counter)/(float)counter_frecuency);
   }
}

//Limit Frequency of Updates
void Timer_System::Limit_frames(uint64_t start_time){
   float  elapsed=Now_Counter-last_Counter;
   uint64_t end_frame=SDL_GetPerformanceCounter();
   uint64_t dif_frames=((end_frame-start_time)/counter_frecuency);
   if(dif_frames>0){
      float fps=(float)counter_frecuency/(float)dif_frames;
   }
   if(dif_frames<tick_perFrame){
      SDL_Delay(tick_perFrame-dif_frames);
   }

}


//Init the Timer
void Timer_System::Init(uint8_t limit_fps){
  target_fps=limit_fps;
  delta_counter=0.0f;
  tick_perFrame=0;
  last_Counter=0;
  Now_Counter=0;
  counter_frecuency=-1;
}

//Get Delta Time
float Timer_System:: get_delta_time(){
   return delta_counter;
}

//Get the Ticks per Frame
float Timer_System::get_ticks_frame(){
   return tick_perFrame;
}

//Get the Target FPS
uint8_t Timer_System::get_targetFps(){
   return target_fps;
}
