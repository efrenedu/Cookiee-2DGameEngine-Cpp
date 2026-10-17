#include "Game_Config.h"
#include<iostream>
#include<fstream>
#include<json.hpp>

//Init the System and Readthe Game Configuration
bool Game_Config_System::Init(std::string src){
  src=src.substr(src.find_last_of("/\\") + 1);
  src="assets\\"+src;
  std::ifstream file(src);
  if(!file.is_open()){
    return false;
  }
  source=src;
  nlohmann::json json_dat;
  file>>json_dat;
  std::vector<std::string> targets={"Windows_Config","Audio_Config","Input_Config","Scenes"};
  for(std::string id:targets){
      if(json_dat.contains(id)){
         config_dat[id]=json_dat[id];
      }
      else{
         return false;
      }
  }
  return true;
}

//Free the Config System
void Game_Config_System::free_system(){
   write_GameConfig();
   config_dat.clear();
}

//Write Game Config
void Game_Config_System::write_GameConfig(){
  if(source==""){
     return;
  }
  std::ofstream file(source);
  if(!file.is_open()){
     std::cout << "Error to Write Config: "<<source<< std::endl;
     return;
  }
  nlohmann::json json_root=nlohmann::json::object();
  for(const auto& outer_dat:config_dat){
      std::string key=outer_dat.first;
      nlohmann::json json_inner=nlohmann::json::object();
      for(const auto& inner_dat:config_dat[key]){
        json_inner[inner_dat.first]=inner_dat.second;
      }
      json_root[key]=json_inner;
  }
   file<<json_root.dump(4);
   file.close();
}

//Change a Config Data Parameter Value
void Game_Config_System::set_param_config(uint8_t category,std::string param_id,std::string val){
  std::string category_str="";
  switch(category){
     case CONFIG_CATEGORY_AUDIO:category_str="Audio_Config";break;
     case CONFIG_CATEGORY_WINDOWS:category_str="Windows_Config";break;
     case CONFIG_CATEGORY_SCENES:category_str="Scenes";break;
     case CONFIG_CATEGORY_INPUT:category_str="Input_Config";break;
     default:break;
  }
  if(category_str==""){
     return;
  }
  auto it_category=config_dat[category_str].find(param_id);
  if(it_category==config_dat[category_str].end()){
     return;
  }
  config_dat[category_str][param_id]=val;
}

//Return the Config data of the Indicated Key
std::unordered_map<std::string,std::string> Game_Config_System::get_Config_Data(std::string id){
  std::unordered_map<std::string,std::string> default_res;
  auto it=config_dat.find(id);
  if(it!=config_dat.end()){
      return it->second;
  }
  default_res;
}

//Get All Configuration Data
std::unordered_map<std::string,std::unordered_map<std::string,std::string>> Game_Config_System::get_allConfig(){
    return config_dat;
}
