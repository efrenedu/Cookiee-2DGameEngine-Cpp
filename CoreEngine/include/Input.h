#ifndef INPUT_H
#define INPUT_H
#include<stdlib.h>
#include<string>
#include<vector>
#include<unordered_map>
#include <SDL.h>


//Mouse Buttons
enum MOUSE_BUTTONS:uint8_t{
   LEFT_BUTTON=0,RIGHT_BUTTON=1,CENTER_BUTTON=2
};

//States of Inputs
enum INPUT_STATES:uint8_t{
   INPUT_DOWN=0,INPUT_PRESSED=1,INPUT_UP=2,INPUT_NONE=3
};

//Key Id of Player Action
enum PLAYER_KEYS:uint16_t{
   KEY_ACTION=0,
   KEY_SECOND_ACTION=1,
   KEY_JUMP=2,
   KEY_INTERACT=3,
   KEY_LEFT=4,
   KEY_RIGHT=5,
   KEY_UP=6,
   KEY_DOWN=7,
   KEY_INVALID=8,
   KEY_COUNT

};


//System For Manage the Inputs
struct Input_System{
   public:
       void init(uint8_t w_time,std::unordered_map<std::string,std::string> key_config);
       void free_system();
       void reset();
       void update(float dt);
       void get_event(SDL_Event ev,float dt);
       uint8_t get_keyState(uint16_t code);
       uint8_t get_keyState(SDL_Scancode code);
       uint8_t get_mouseState(uint8_t button);
       std::vector<uint16_t> get_mousePosition();
       uint16_t get_PlayerKey(uint8_t action_player);
       std::string get_lastKey();
   private:
       std::vector<float> keyboard_timers;
       std::vector<float> Gamepads_timers;
       std::vector<float> mouse_timers;
       std::vector<uint8_t> mouse_states;
       std::vector<uint8_t> Gamepads_states;
       std::vector<uint8_t>key_states;
       std::vector<uint16_t> action_keys;
       float wait_time;
       uint16_t mouse_x;
       uint16_t mouse_y;
       uint16_t last_keyInput;

};

#endif // INPUT_H
