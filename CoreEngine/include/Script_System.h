#ifndef SCRIPT_SYSTEM_H
#define SCRIPT_SYSTEM_H
#include <lua.hpp>

class Script_System
{
    public:
       bool Init();
    private:
        lua_State* L=nullptr;

};

#endif // SCRIPT_SYSTEM_H
