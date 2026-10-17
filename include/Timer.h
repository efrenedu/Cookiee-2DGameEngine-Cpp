#ifndef TIMER_H
#define TIMER_H
#include<cstdint>

//System for Mange the Time of System
struct Timer_System{
    public:
        void update();
        void Init(uint8_t limit_fps);
        float get_delta_time();
        float get_ticks_frame();
        uint8_t get_targetFps();
    private:
        uint32_t Now_Counter;
        uint32_t last_Counter;
        uint32_t counter_frecuency;
        float tick_perFrame;
        float delta_counter;
        uint8_t target_fps;
};
#endif // TIMER_H
