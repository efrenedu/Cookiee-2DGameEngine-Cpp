#include "Audio_System.h"
#include<iostream>
#include<vector2.h>
#include<Game_Config.h>
#include<string>

//Audio Engine Functions

//Init the Audio Engine
bool AudioEngine::Init(){
   loaded=false;
   ma_result result=ma_engine_init(NULL,&engine);
   if(result==MA_SUCCESS){
        ma_engine_listener_set_position(&engine,0,0,0,0);
       loaded=true;
   }
   return loaded;
}

//Free the Audio Engine
void AudioEngine::free_memory(){
    if(loaded==false){
        return;
    }
    ma_engine_stop(&engine);
    ma_engine_uninit(&engine);
}

//Get the Engine
ma_engine* AudioEngine::get_engine(){
    return &engine;
}

//Return True if the Engine is Loaded
bool AudioEngine::is_loaded(){
   return loaded;
}

//Audio Clip Methods

//Load a Clip from a Audio Source
void AudioClip::load_clip(const char* src,ma_engine* engine,uint32_t hash_n){
     clip= new ma_sound();
     loaded=false;
     ma_result result=ma_sound_init_from_file(engine,src,0,NULL,NULL,clip);
     if(result==MA_SUCCESS){
        loaded=true;
        hash_name=hash_n;
     }
     else{
        delete clip;
     }
}

//Get the Audio Clip Pointer
ma_sound* AudioClip::get_clip(){
   return clip;
}

//Free Memoryt of Clip
void AudioClip::free_memory(){
    if(clip!=NULL &&loaded==true){
        if(ma_sound_is_playing(clip)){
           ma_result result= ma_sound_stop(clip);
        }
        ma_sound_uninit(clip);
    }
    loaded=false;
}

//Return True if the Clip is Loaded
bool AudioClip::is_loaded(){
   return loaded;
}

//Audio System Methods

//Init the Audio System
void Audio_System::Init_System(ma_engine* target_engine,float music_vol_init,float sfx_vol_init){
   music_playing=-1;
   engine=target_engine;
   music_volume=music_vol_init;
   sfx_volume=sfx_vol_init;
   volume_changed=false;

}

//Free the System Memory
void Audio_System::Free_System(){

    uint8_t index_music=0;
    uint8_t index_sound=0;
    for(AudioClip* clip:musics){
         if(clip->is_loaded()){
            clip->free_memory();
         }
         delete clip;
         index_music++;
    }
    for(AudioClip* clip:sounds){
         if(clip->is_loaded()){
           clip->free_memory();
         }
         delete clip;
         index_sound++;
    }
    for(ma_sound* clip:list_copy_sounds){
         ma_sound_stop(clip);
         ma_sound_uninit(clip);
    }
    list_copy_sounds.clear();
    musics.clear();
    sounds.clear();
}

//Reserve Memory for the Audio System
void Audio_System::reserve_memory(uint8_t num_sounds,uint8_t num_musics){
    musics.reserve(num_musics);
    sounds.reserve(num_sounds);
}

//Update Audio System
void Audio_System::update_system(Game_Config_System* config_sys){
   int8_t remove_index=-1;
   if(volume_changed){
      volume_changed=false;
      config_sys->set_param_config(CONFIG_CATEGORY_AUDIO,"Music_Volume",std::to_string(music_volume));
      config_sys->set_param_config(CONFIG_CATEGORY_AUDIO,"Sfx_Volume",std::to_string(sfx_volume));

   }
   for(uint8_t i=0;i<list_copy_sounds.size();i++){
       ma_sound* target=list_copy_sounds[i];
       if(ma_sound_at_end(target)){
           ma_sound_stop(target);
           ma_sound_uninit(target);
           remove_index=i;
       }
   }
   if(remove_index<0){
      return;
   }
   list_copy_sounds.erase(list_copy_sounds.begin()+remove_index);
}


//Add Audio Clip to the Systm
void Audio_System::add_clip(const char* src,bool is_music,uint32_t hash_name){
   AudioClip* clip=new AudioClip();
   clip->load_clip(src,engine,hash_name);
   if(clip->is_loaded()==false){
       delete clip;
       return;
   }
   if(is_music){
      musics.push_back(clip);
   }
   else{
      sounds.push_back(clip);
   }
}

//Resume the Music
void Audio_System::resume_music(uint8_t id_music,bool from_start,uint16_t fadein){
     if(id_music<0 || id_music>=musics.size()){
       return;
     }
     AudioClip* clip=musics[id_music];
     if(clip->is_loaded()==false){
         return;
     }
     ma_sound_reset_stop_time_and_fade(clip->get_clip());
     ma_sound * snd=clip->get_clip();
     if(from_start){
         ma_sound_seek_to_pcm_frame(snd,0);
     }
     if(fadein>0){
        ma_sound_set_fade_in_milliseconds(snd,0,1.0,fadein);
     }
     ma_sound_set_spatialization_enabled(snd,MA_FALSE);
     ma_sound_set_volume(snd,music_volume);
     ma_sound_start(snd);
}

//Stop the Music
void Audio_System::stop_music(uint8_t id_music,uint16_t fadeout){
   if(id_music<0 || id_music>=musics.size()){
       return;
   }
   AudioClip* clip=musics[id_music];
   if(clip->is_loaded()==false){
      return;
   }
   if(fadeout==0){
        ma_sound_stop(clip->get_clip());
   }
   else{
        ma_sound_stop_with_fade_in_milliseconds(clip->get_clip(),fadeout);
   }
}

//Play the Music
void Audio_System::play_music(uint8_t music_id,bool loop,uint16_t fadeIn_time){
   if(music_id<0 || music_id>=musics.size()){
       return;
   }
   AudioClip* clip=musics[music_id];
   if(clip->is_loaded()==false){
      return;
   }
   if(music_playing!=-1){
      AudioClip* playing_clip=musics[music_playing];
      ma_sound_stop(playing_clip->get_clip());
      music_playing=-1;
   }
   ma_sound* snd=clip->get_clip();
   if(loop){
      ma_sound_set_looping(snd,MA_TRUE);
   }
   else{
       ma_sound_set_looping(snd,MA_FALSE);
   }
   ma_sound_reset_stop_time_and_fade(snd);
   ma_sound_seek_to_pcm_frame(snd,0);
   if(fadeIn_time>0){
      ma_sound_set_fade_in_milliseconds(snd,0,1.0,fadeIn_time);
   }
   ma_sound_set_spatialization_enabled(snd,MA_FALSE);
   ma_sound_set_volume(snd,music_volume);
   ma_sound_start(snd);
   music_playing=music_id;
}

//Play a Sound Clip
void Audio_System::play_sound(uint8_t sound_index,float pitch,float min_dist,float max_dist,float posx,float posy){
  if(sound_index<0 || sound_index>=sounds.size()){
       return;
   }
   AudioClip* clip=sounds[sound_index];
   if(clip->is_loaded()==false){
      return;
   }

   ma_sound* snd=clip->get_clip();
   bool playing_sfx=false;
   if(ma_sound_is_playing(snd)){
        if(ma_sound_at_end(snd)==false){
               playing_sfx=true;
        }
   }
   if(playing_sfx==false){
        ma_sound_seek_to_pcm_frame(snd,0);
        ma_sound_set_pitch(snd,pitch);
       // add_repeat_sfx_data(snd,play_count,Maxdist_attenuation,min_distAtenuation,position);
        set_sound_atenuation(snd,min_dist,max_dist,posx,posy);
        ma_sound_start(snd);
        return;
   }
   ma_data_source* dat=ma_sound_get_data_source(snd);
   ma_sound* copy_sound=new ma_sound();
   ma_result result=ma_sound_init_from_data_source(engine,dat,0,NULL,copy_sound);
   if(result==MA_SUCCESS){
       ma_sound_set_pitch(copy_sound,pitch);
       ma_sound_seek_to_pcm_frame(copy_sound,0);

       set_sound_atenuation(snd,min_dist,max_dist,posx,posy);
       ma_sound_start(copy_sound);
       list_copy_sounds.push_back(copy_sound);

   }
   else{
      delete copy_sound;
   }
}

//Get Second of the Music is Playing
uint16_t Audio_System::get_music_second(uint8_t id_music){
    uint16_t res=0;
    AudioClip* clip=musics[id_music];
    if(clip->is_loaded()==false){
       return 0;
    }
    res=ma_sound_get_time_in_milliseconds(clip->get_clip());
       if(res!=0){
          res=(float)res/1000.0f;
       }
    return res;
}

//get the Music Volume
float Audio_System::get_music_volume(){
   return music_volume;
}

//Get the Sfx Volume
float Audio_System::get_sfx_volume(){
   return sfx_volume;
}

//Set Attenuation of Sound
void Audio_System::set_sound_atenuation(ma_sound* snd,float min_dist,float max_dist,float x, float y){
    if(max_dist>0){
        ma_vec3f p_listenerr=ma_engine_listener_get_position(engine,0);
        Vector2 pos_listener=Vector2(p_listenerr.x,p_listenerr.y);
        Vector2 position=Vector2(x,y);
        float dist=(pos_listener-position).magnitud();
        if(dist>=min_dist){
            if(dist>=max_dist){
                 ma_sound_set_volume(snd,0.0);
                 return;
            }
            float dif=dist-min_dist;
            float distance_atenuation=max_dist-min_dist;
            float next_vol=sfx_volume;
            if(distance_atenuation!=0){
                  next_vol=next_vol-(next_vol*(dif/distance_atenuation));
            }
             ma_sound_set_volume(snd,next_vol);
        }
        else{
            ma_sound_set_volume(snd,sfx_volume);
        }
    }
    else{
         ma_sound_set_volume(snd,sfx_volume);
    }
}

//Set the Music On Indicated Second
void Audio_System::set_music_second(uint8_t id_music,uint16_t second){
    AudioClip* clip=musics[id_music];
    if(clip->is_loaded()){
        ma_sound_seek_to_second(clip->get_clip(),second);
    }
}

//Set Audio Sfx Volume
void Audio_System::set_sfx_volume(float val){
   sfx_volume=val;
   volume_changed=true;

}

//Set Music Volume
void Audio_System:: set_music_volume(float val){
   music_volume=val;
   volume_changed=true;
   if(music_playing!=-1){
      AudioClip* clip_playing=musics[music_playing];
      ma_sound*snd=clip_playing->get_clip();
      if(snd!=NULL){
         ma_sound_set_volume(snd,music_volume);
      }
   }
}
