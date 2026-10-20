#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H
#include<vector>
#include<unordered_map>
#include<cstdint>
#include<string>

enum CONFIG_CATEGORIES:uint8_t{
   CONFIG_CATEGORY_AUDIO=0,
   CONFIG_CATEGORY_WINDOWS=1,
   CONFIG_CATEGORY_SCENES=2,
   CONFIG_CATEGORY_INPUT=3,
};

//System for Manage the Configuration of Game
struct Game_Config_System
{
  public:
    bool Init(std::string config_src);
    void write_GameConfig();
    void modify_Config(std::string param,std::string value);
    void free_system();
    std::unordered_map<std::string,std::string> get_Config_Data(std::string id);
    std::unordered_map<std::string,std::unordered_map<std::string,std::string>>get_allConfig();
    void set_param_config(uint8_t category,std::string param_id,std::string val);
  private:
     std::unordered_map<std::string,std::unordered_map<std::string,std::string>> config_dat;
     std::string source;
};

#endif // GAME_CONFIG_H
