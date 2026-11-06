#ifndef CONVERTER_H
#define CONVERTER_H
#include<vector>
#include<cstdint>
#include<string>
#include<unordered_map>

//Convert data to the Requerid Format
namespace Converter{

    const bool strTo_Boolean(std::string val);
    const int32_t strTo_Int(std::string val,int32_t default_value=0);
    const uint32_t strTo_UInt(std::string val,uint32_t default_value=0);
    const  float strTo_Float(std::string val);
    const double strTo_Double(std::string val);
    const std::vector<std::string>str_toVect(std::string val,char separator=',',std::vector<char> ignore_chars={'\n','\0',' '});
    const float pixelToMetter(float pxl);
    const float MetterToPixel(float metter_val);
    const uint16_t str_to_colorPacked(std::string val);
    const uint16_t packRGBA4444(uint8_t r,uint8_t g,uint8_t b,uint8_t a);
    const void unpackRGBA4444(uint16_t color,uint8_t&r,uint8_t&g,uint8_t&b,uint8_t&a);
    constexpr float Metter_Factor=32;
};


//for Props of Entities
struct Entity_Props{
  std::unordered_map<std::string,std::string> data;
  void set_value(std::string key,std::string value){
     data[key]=value;
  }
  const std::string get_string(const std::string &key,const char* default_value)const{
      auto it=data.find(key);
      if(it!=data.end()){
         return it->second;
      }
      return default_value;
  }
  float get_float(const std::string & key,float default_value)const{
      auto it=data.find(key);
      if(it!=data.end()){
         return Converter::strTo_Float(it->second);
      }
      return default_value;
  }
  int32_t get_int(const std::string & key,int32_t default_value)const{
      auto it=data.find(key);
      if(it!=data.end()){
         return Converter::strTo_Int(it->second);
      }
      return default_value;
  }
  uint32_t get_uint(const std::string & key,uint32_t default_value)const{
      auto it=data.find(key);
      if(it!=data.end()){
         return Converter::strTo_UInt(it->second);
      }
      return default_value;
  }
  bool get_bool(const std::string & key,bool default_value)const{
      auto it=data.find(key);
      if(it!=data.end()){
         return Converter::strTo_Boolean(it->second);
      }
      return default_value;
  }
};

#endif // CONVERTER_H
