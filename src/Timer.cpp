#include "Timer.h"
#include "iostream"
#include <SDL.h>

//Update the Timer
void Timer_System:: update(){
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
   float elapsed_counter=delta_counter*1000.0f;
   float  elapsed=Now_Counter-last_Counter;
   if(elapsed>0){
      float fps=(float)counter_frecuency/elapsed;
   }
   if(elapsed_counter<tick_perFrame){
      SDL_Delay(tick_perFrame-elapsed_counter);
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
