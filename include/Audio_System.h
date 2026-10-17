#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H
#include <SDL.h>
#include<miniaudio.h>
#include<vector>
#include<string>
class Game_Config_System;

//Engine of Audio
struct AudioEngine{
public:
    bool Init();
    void free_memory();
    ma_engine* get_engine();
    bool is_loaded();
private:
    ma_engine engine;
    bool loaded;
};

//Store the Data of an Audio Clip
struct AudioClip{
   public:
       void load_clip(const char* src,ma_engine* engine,uint32_t hash_n);
       void free_memory();
       ma_sound* get_clip();
       bool is_loaded();
       uint32_t get_hash_name();
   private:
       ma_sound* clip;
       bool loaded;
       uint32_t hash_name;
};

//System for the Management of Audio
struct Audio_System
{
  public:
      void Init_System(ma_engine* target_engine,float music_vol_init, float sfx_vol_init);
      void Free_System();
      void reserve_memory(uint8_t num_sound,uint8_t num_musics);
      void update_system(Game_Config_System* config_sys);
      void add_clip(const char* src,bool is_music,uint32_t hash_name);
      void play_music(uint8_t music_id,bool loop,uint16_t fadeIn_time=0);
      void play_sound(uint8_t sound_index,float pitch,float min_dist=0.0f,float max_dist=0.0f,float posx=0.0f,float posy=0.0f);
      void stop_music(uint8_t id_music,uint16_t fadeout=0);
      void resume_music(uint8_t id_music,bool from_start=false,uint16_t  fadein=0);
      uint16_t get_music_second(uint8_t id_music);
      float get_music_volume();
      float get_sfx_volume();
      void set_sound_atenuation(ma_sound* snd,float min_dist,float max_dist,float x,float y);
      void set_music_second(uint8_t id_music,uint16_t second);
      void set_sfx_volume(float val);
      void set_music_volume(float val);
  private:
      ma_engine* engine=nullptr;
      std::vector<AudioClip*>sounds;
      std::vector<AudioClip*>musics;
      std::vector<ma_sound*>list_copy_sounds;
      float sfx_volume;
      float music_volume;
      int8_t music_playing;
      bool  volume_changed;
};

#endif // AUDIO_SYSTEM_H
