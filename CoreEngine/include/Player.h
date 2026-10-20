#ifndef PLAYER_H
#define PLAYER_H
#include<cstdint>
#include<vector>
class Scene;
class Input_System;

//Store the Player data
struct player_data{
   uint16_t id_player;
};

//Player System forManage the Player component

struct Player_System{
   public:
       void reserve_memory(uint16_t count_players, uint16_t count_entities,uint16_t fillvalue);
       void regiser_player(uint16_t id);
       void update_player(uint32_t mask,uint16_t id,Scene* target_scene,float dt,Input_System* input_sys);
       player_data* get_player(uint16_t id);
       void On_Collision(uint16_t id,uint16_t id_other,float norm_x,float norm_y,bool begin_ev);
       void On_Trigger(uint16_t id,uint16_t id_other,bool begin_ev);
   private:
       std::vector<player_data> players;
       std::vector<uint16_t>EntitytoDense;

};
/*using namespace std;
class Game_Object;
class ManiFold;
class Resources_Store;

enum INPUT_UI_TYPE{
   UI_INPUT_MOUSE=0,UI_INPUT_KEYBOARD=1,
};

//Save the Information and Behavour of Player
class Player
{
public:
    Player();
    void Assign_CombatData(int ply_health,int ply_continues,int ply_energy,float ply_attack,vector<float> times_combat,float force_damage);
    void Assign_ControllerData(float ply_speed,float ply_jump_speed,float ply_coyote_time,int ply_moveType,Vector2 spd_limit);
    void Assign_Inventory(int num_slots,int initial_money);
    virtual ~Player();
    void start(Game_Object *g);
    void update(Game_Object * g);
    void OnTrigger(Game_Object*  other,Game_Object* g);
    void OnTriggerStay(Game_Object*  other,Game_Object* g);
    void OnTriggerExit(Game_Object*  other,Game_Object* g);
    void OnCollision(Game_Object* other,Game_Object* g, Vector2 norm);
    void OnCollisionStay(Game_Object* other,Game_Object* g);
    void OnCollisionExit(Game_Object* other,Game_Object* g);
    void set_checkPoint(Vector2 p);
    void reset(Game_Object *g);
    void resize_ply(Vector2 dif);
    bool is_dead();
    bool is_attacking();
    bool under_damage();
    bool recovering_damage();
    bool is_jumping();
    bool receive_damage(int cant,Game_Object* ply_object,Game_Object* damage_obj);
    void receive_Something(string id,int cant);
    void receive_powerUp(Game_Object* g,string id);
    void receive_item(string id_item,int cant);
    void use_item(Game_Object* g,string id_item,string target);
    void apply_state(string id);
    void finish_GameOver();
    void exit_menu();
    Resources_Store* get_resourceStore();
    Player_UI* get_UI_Controller();
    Player_Controler* get_controller();
    void free_memory();
    void update_status_UI(string id);
    void stop_controller(bool val);
    void verify_states_controller(Game_Object* g);
    States_Controller_Manager* get_state_manager();
    void teleport(Game_Object* g,Vector2 pos,float sleep_cam);
private:
    Player_Controler controler;
    Player_Combat combat_script;
    Player_UI ui_controller;
    Resources_Store* resources_player;
    bool is_controller;
    int energy;
    int max_enery;
    int size_inventory;
    Vector2 pos_reset;
    Vector2 initial_pos;
    float initial_angle;
    Vector2 force_modify;
    bool on_menu;
    States_Controller_Manager state_manager;

};*/
#endif // PLAYER_H
