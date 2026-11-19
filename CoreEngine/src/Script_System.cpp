#include "Script_System.h"
#include<iostream>
#include<lua.hpp>
bool Script_System::Init(){
   L=luaL_newstate();
   if(L==NULL){
      std::cout<<"fail creating machine "<<std::endl;
      return false;
   }
   luaL_openlibs(L);
   return true;
}

lua_State* Script_System::get_lua_state(){
  return L;
}
