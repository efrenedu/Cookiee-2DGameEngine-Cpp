#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H
#include "pugixml.hpp"
#include "json.hpp"
#include <Converter.h>
#include <Component.h>
#include<hash_support.h>
#include <iostream>
#include<TileMap.h>
#include<Camera.h>
#include<texture.h>
#include<Audio_System.h>
#include<Render.h>
#include<Transform_System.h>
#include<Player.h>
#include<Animation.h>

class Scene;

//For Load Scenes
struct Scene_Loader{
   public:
      bool LoadSceneXML(const char* filePath, Scene* target_scene,const char* root_name,const char* entity_node_name,const char* component_node_name,const char* header_name);
      bool LoadSceneJSON(const char* filePath,Scene* target_scene,const char* root_name,const char* entity_node_name,const char* component_node_name,const char* header_name);
      bool LoadSceneTiledXML(const char* filepath,SDL_Renderer* render_win,Scene* target_scene,System_TileMap& tilemap,textureManager_system& tex_sys,Audio_System& audio_sys,Physic_Material_System& physic_sys);
   private:
     string verify_paramValue(string param_name,string param_value,Physic_Material_System& physic_sys);
};

#endif // SCENE_LOADER_H
