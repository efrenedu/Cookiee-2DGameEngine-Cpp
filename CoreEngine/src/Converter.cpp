#include "Converter.h"
#include "iostream"
#include "math.h"


//convert a string into boolean
const bool Converter::strTo_Boolean(std::string val){
    bool res=false;
    if(val=="True" || val=="true" || val=="1"){
        res=true;
    }
    return res;
}

//convert a string into Integer
const int32_t Converter::strTo_Int(std::string val,int32_t default_value){
  bool negative=false;
  int32_t index=0;
  if(val.empty()){
      return default_value;
  }
  if(val[0]=='-'){
      negative=true;
      index=1;
  }
  else if(val[0]=='+'){
      index=1;
  }
  int64_t res=0;
  bool has_digits=false;
  int32_t limit_overflow=2147483647LL;
  int32_t max_char=val.length();
  for(int32_t i=index;i<max_char;i++){
       unsigned char c=val[i];
       int32_t code=(int32_t)c;
       if(code>=48 && code<=57){
            has_digits=true;
            code=code-48;
            res=(res*10)+code;
            if(res>limit_overflow){
                return negative?-limit_overflow:limit_overflow;
            }
        }
        else{

            break;
        }
  }

  if(!has_digits){
      return default_value;
  }
  return negative? static_cast<int32_t>(-res): static_cast<int32_t>(res);
}

//Conversion to Unsigned Int
const uint32_t Converter::strTo_UInt(std::string val,uint32_t default_value){

  bool negative=false;
  uint32_t index=0;
  if(val.empty()){
      return default_value;
  }
  if(val[0]=='-'){
      index=1;
  }
  else if(val[0]=='+'){
      index=1;
  }
  uint64_t res=0;
  bool has_digits=false;
  uint32_t limit_overflow=2147483647LL*2;
  uint32_t max_char=val.length();
  for(uint32_t i=index;i<max_char;i++){
       unsigned char c=val[i];
       int32_t code=(int32_t)c;
       if(code>=48 && code<=57){
            has_digits=true;
            code=code-48;
            res=(res*10)+code;
            if(res>limit_overflow){
                return limit_overflow;
            }
        }
        else{

            break;
        }
  }
  if(!has_digits){
      return default_value;
  }

  return static_cast<uint32_t>(res);
}

//convert a string to float
const float Converter::strTo_Float(std::string val){
   int32_t max_decimal=9;
   float res=0.0f;
   std::string enteros="";
   std::string decimales="";
   bool negative=false;
   bool get_decimal=false;
   int num_decimales=0;
   for(int32_t i=0;i<val.length();i++){
       unsigned char c=val[i];
       if(c!=' '){
           if(c=='-'){
             negative=true;
           }
           else if(c=='.' || c==','){
               get_decimal=true;
           }
           else{
              int32_t code=(int32_t)c;
              if(code>=48 && code<=57){
                if(get_decimal==false){
                      enteros+=c;
                }
                else{
                      if(num_decimales<max_decimal){
                          decimales+=c;
                          num_decimales=num_decimales+1;
                      }
                 }
              }
           }
       }
   }
   int32_t val_entero=strTo_Int(enteros);
   res=0.0f+((float)val_entero);
   int32_t val_decimal=strTo_Int(decimales);
   int multiplic=pow(10,num_decimales);
   if(multiplic!=0){
       float f_val=(float)val_decimal/(float)multiplic;
       res=res+f_val;
   }
   if(negative){
      res=res*-1;
   }
   return res;
}

//Conver a String into a Vector of Strings
const std::vector<std::string> Converter::str_toVect(std::string val,char separator,std::vector<char> ignore){
   std::vector<std::string> res;
   std::string temp_val="";
   for(int32_t i=0;i<val.length();i++){
       unsigned char c=val[i];
       bool ignore_required=false;
       for (char temp_ignore:ignore){
          if(temp_ignore==c){
               ignore_required=true;
               break;
          }
       }
       if(ignore_required){
          continue;
       }
       if(c==separator){
             res.push_back(temp_val);
             temp_val="";
        }
        else{

            temp_val+=c;
        }
   }
   if(temp_val!=""){
     res.push_back(temp_val);
   }
   return res;
}

//convert a string into double
const double Converter::strTo_Double(std::string val){
    double res=0.0;
    return res;
}

//Comvert pixel Value to Metter Value for Box2d Physics Calculation
const float Converter::pixelToMetter(float pxl){
    float res=0.0f;
    if(Metter_Factor!=0.0f ){
        res=pxl/Metter_Factor;
    }
    return res;
}

//Convert a String to Uint16 packedColor
const uint16_t Converter:: str_to_colorPacked(std::string val){
    std::vector<std::string> vect_color=str_toVect(val);
    uint16_t color_packed=0;
    if(vect_color.size()>=4){
        uint8_t r=strTo_UInt(vect_color[0]);
        uint8_t g=strTo_UInt(vect_color[1]);
        uint8_t b=strTo_UInt(vect_color[2]);
        uint8_t a=strTo_UInt(vect_color[3]);
        color_packed=packRGBA4444(r,g,b,a);
   }
   return color_packed;
}

//Convert Metter Value from  Box2d Physic Engine to Pixel Value
const float Converter::MetterToPixel(float metter_val){
    return metter_val*Metter_Factor;
}

//Convert a Color RGBA to Uint16
const uint16_t Converter::packRGBA4444(uint8_t r,uint8_t g,uint8_t b,uint8_t a){
    return((r>>4) & 0x0F)<<12 |((g>>4) &0x0F)<<8 | ((b>>4)& 0x0F)<<4 | ((a>>4)& 0x0F);
}

//Convert a uint16 to Color RGBA
const void Converter::unpackRGBA4444(uint16_t color,uint8_t&r,uint8_t&g,uint8_t&b,uint8_t&a){
   uint8_t multiply=17;
   r=((color>>12) & 0x0F)*multiply;
   g=((color>>8) & 0x0F)*multiply;
   b=((color>>4) & 0x0F)*multiply;
   a=((color) & 0x0F)*multiply;

}



