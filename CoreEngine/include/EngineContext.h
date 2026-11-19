#ifndef ENGINECONTEXT_H
#define ENGINECONTEXT_H
#include<cstdint>
#include<UI_event.h>
#include<Input.h>
#include<Physic_Engine.h>
#include<Game_Config.h>
#include<Render_DrawSystem.h>

struct EngineContext
{
    public:
        static void init();
        static EngineContext& get();
        void set_pause(bool val);
        void exit_game();
        void load_scene(uint8_t index);
        uint8_t& get_next_scene();
        bool& is_paused();
        bool& requiere_finish();
        UI_event_System& get_uiEvent();
        Physic_Engine_System& get_physic();
        Input_System& get_input();
        Game_Config_System& get_config();
        Render_DrawSystem& get_drawContext();
    private:
        static EngineContext* instance;
        UI_event_System ui_event;
        Game_Config_System config;
        Physic_Engine_System physic_sys;
        Input_System input_sys;
        Render_DrawSystem draw_sys;
        bool pause_game;
        bool finish_game;
        uint8_t scene_index;
        uint8_t next_scene;

};

#endif // ENGINECONTEXT_H
