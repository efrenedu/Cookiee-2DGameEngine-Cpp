#ifndef SCRIPT_SYSTEM_H
#define SCRIPT_SYSTEM_H
#include <lua.hpp>

class Script_System
{
    public:
       bool Init();
       lua_State* get_lua_state();
    private:
        lua_State* L=nullptr;

};

#endif // SCRIPT_SYSTEM_H
