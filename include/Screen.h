#ifndef SCREEN_H
#define SCREEN_H
#include<SDL.h>
#include<vector>
#include<cstdint>

//System for Manage the Screen
struct Screen_System{
  public:
      void set_size(int8_t target);
      void Init(SDL_Window * win);
      std::vector<uint16_t> get_ActiveSize();
  private:
      SDL_Window* win_ptr=nullptr;
      std::vector<SDL_DisplayMode> list_modes;
      uint16_t full_screenX;
      uint16_t full_screenY;
      uint8_t selected_size;
      bool On_fullScreen;

};
#endif // SCREEN_H
