#include "Screen.h"
#include "iostream"
#include "math_structs.h"

//Init the Screen System
void Screen_System::Init(SDL_Window* win){
   win_ptr=win;
   selected_size=0;
   full_screenX=0;
   full_screenY=0;
   On_fullScreen=false;
   uint16_t Count{SDL_GetNumDisplayModes(0)};
   SDL_DisplayMode Mode;
   SDL_DisplayMode desktop_Mode;
   if(SDL_GetDesktopDisplayMode(0,&desktop_Mode)!=0){
      return;
   }
   for (uint16_t i{0}; i<Count; ++i) {
        SDL_GetDisplayMode(0, i, &Mode);
        if(Mode.w<desktop_Mode.w && Mode.h<desktop_Mode.h){
            list_modes.push_back(Mode);
        }
        else if(Mode.w==desktop_Mode.w && Mode.h==desktop_Mode.h){
            full_screenX=Mode.w;
            full_screenY=Mode.h;
        }
   }

}

//Set the Size of Screen
void Screen_System::set_size(int8_t target){
  if(target<0 || target>=list_modes.size()){
      return;
  }
  if(win_ptr==nullptr){
      return;
  }
  selected_size=target;
  uint16_t new_w=list_modes[target].w;
  uint16_t new_h=list_modes[target].h;
  if(new_w==full_screenX && new_h==full_screenY){
      On_fullScreen=true;
  }
  else{
    On_fullScreen=false;
  }
  SDL_SetWindowSize(win_ptr,new_w,new_h);

}

//get the Size of Active Screen Size
std::vector<uint16_t> Screen_System::get_ActiveSize(){
   std::vector<uint16_t> res={0,0};
   if(selected_size<list_modes.size()){
       res[0]=list_modes[selected_size].w;
       res[1]=list_modes[selected_size].h;
   }
   return res;
}
/*
bool Screen::modific_win=false;
bool Screen::iniciado=false;
Vector2 Screen::target_camera_Size=Vector2(0,0);
std::vector<SDL_DisplayMode> Screen::modes;
Vector2 Screen::size_win=Vector2(0,0);
Vector2 Screen::fullscreen_size=Vector2(0,0);
bool Screen::is_fullscreen=false;

//get the size of windows
Vector2 Screen::get_size(){
   return size_win;
}

//set the size of windows
void Screen::set_size(Vector2 dim){
     size_win=dim;
}

//change windows size
void Screen::change_size(Vector2 sz,bool fullscreen){
   modific_win=true;
   set_size(sz);
   is_fullscreen=fullscreen;
}

//update the windows
void Screen::update(SDL_Window* win){
    if(iniciado==false){
        iniciado=true;
        init();
        verify_size();
        SDL_SetWindowSize(win,(int)size_win.x,(int)size_win.y);

    }
    if(modific_win==true && win!=NULL){
          modific_win=false;
          verify_size();
          SDL_SetWindowSize(win,(int)size_win.x,(int)size_win.y);
    }
}

//verify if the size is compatible with the Available display modes
void Screen::verify_size(){
   if(is_fullscreen){
      size_win=Vector2(fullscreen_size.x,fullscreen_size.y);
   }
   else{
       SDL_DisplayMode first_mode=modes.at(0);
       int difw=Math_G::Abs(first_mode.w-size_win.x);
       int difh=Math_G::Abs(first_mode.h-size_win.y);;
       int best=0;
       for(int i=1;i<modes.size();i++){
           SDL_DisplayMode temp_mode=modes.at(i);
           int temp_w=Math_G::Abs(temp_mode.w-size_win.x);
           int temp_h=Math_G::Abs(temp_mode.h-size_win.y);
           if(temp_w<difw){
              if(temp_h<difh){
                  difw=temp_w;
                  difh=temp_h;
                  best=i;
               }
            }
       }
       SDL_DisplayMode best_mode=modes.at(best);
       size_win=Vector2(best_mode.w,best_mode.h);
   }
}
//return the List of Display Valids for the Windows
std::vector<SDL_DisplayMode> Screen:: get_modes(){
   return modes;
}

//Init the Screen default values
void Screen::init(){
    int Count{SDL_GetNumDisplayModes(0)};
    SDL_DisplayMode Mode;
    SDL_DisplayMode desktop_Mode;
    if(SDL_GetDesktopDisplayMode(0,&desktop_Mode)==0){
        for (int i{0}; i<Count; ++i) {
            SDL_GetDisplayMode(0, i, &Mode);
            if(Mode.w<desktop_Mode.w && Mode.h<desktop_Mode.h){
                modes.push_back(Mode);
            }
            else if(Mode.w==desktop_Mode.w && Mode.h==desktop_Mode.h){
                fullscreen_size=Vector2(Mode.w,Mode.h);
            }
        }

    }
}

//Get the Target Canvas In Pixels
Vector2 Screen::get_target_Camera_Size(){
    return target_camera_Size;
}

//Set the Target Canvas In Pixels
void Screen::set_target_Camera_Size(Vector2 val){
    target_camera_Size=val;
}
*/
