#include "math_structs.h"
#include<iostream>
#include<vector2.h>

/*Math G Methods*/

//return the Clamp value between a Min and Max
const float Math_G::Clamp(float val,float minimo,float maximo){
    if(val>maximo){
       val=maximo;
    }
    if(val<minimo){
        val=minimo;
    }
    return val;
}

//Return true if the String is Number
const bool Math_G::is_number(std::string val){
   bool res=true;
   if(val==""){
       res=false;
   }
   else{
       for(int i=0;i<val.length();i++){
           unsigned char c=val[i];
           if(c==' '){
              res=false;
              i=val.length();
           }
           else{
                if(c=='.' || c==','){
                    if(i==0 || i==val.length()-1){
                        res=false;
                        i=val.length();
                    }
                }
                else if(c=='-'){
                    if(i!=0){
                        res=false;
                        i=val.length();
                    }
                }
                else{
                    int code=(int)c;
                    if(code<48 || code>57){
                        res=false;
                        i=val.length();
                    }
                }
           }
       }
   }
   return res;
}

//Get Atan2 Value
const float Math_G::Atan2(float y_val,float x_val){
   return atan2(y_val,x_val);
}

//Get Acos
const float Math_G::Acos(float val){
   return acos(val);
}
//Get Asin
const float Math_G::Asin(float val){
   return asin(val);
}
//Get Cos
const float Math_G::Cos(float rad){
   return cos(rad);
}

//Get Sin
const float Math_G::Sin(float rad){
   return sin(rad);
}
//Convert to Radians
const float Math_G::to_rad(float angle){
    return angle/MATH_CONSTANTS::RADIANS;
}

//Convert to Degree
const float Math_G:: to_degree(float radians){
   return (radians*180)/MATH_CONSTANTS::PI_VAL;

}

//Get a Random value from a Seed
const int Math_G::get_random(int32_t mod,int32_t param1,int32_t param2,int32_t initial_seed){
   if(initial_seed==-1){
     int32_t rand_n=rand() % 100 + 1;
     initial_seed=rand_n;
   }
   int32_t val=(param1*initial_seed)+param2;
   while(val>mod){
      val=val%mod;
   }
   return val;
}

//Return the absolute Value
const float Math_G::Abs(float val){
   return abs(val);
}

//Return the Pow Value of a Number
const float Math_G::Pow(float pow_val,float number_val){
    return pow(number_val,pow_val);
}

//wrap an Angle between -PI and PI
const float Math_G::Wrap_Angle(float angl){
   angl=fmod(angl+MATH_CONSTANTS::PI_VAL,2.0f*MATH_CONSTANTS::PI_VAL);
   if(angl<0){
      angl+=2.0*MATH_CONSTANTS::PI_VAL;
   }
   return angl-MATH_CONSTANTS::PI_VAL;
}

//Wrap Accumulated Angle using as Reference Radians Accumulated and last Radian Received
const float Math_G::Wrap_AngleAccumulated(float new_radians,float last_radians,float Radians_accumulated){
   float res=Radians_accumulated;
   float delta_radians=new_radians-last_radians;
   if(delta_radians>MATH_CONSTANTS::PI_VAL){
        delta_radians-=2.0*MATH_CONSTANTS::PI_VAL;
   }
   else if(delta_radians<-MATH_CONSTANTS::PI_VAL){
        delta_radians+=2.0*MATH_CONSTANTS::PI_VAL;
   }
   res+=delta_radians;
   res=fmod(res,2.0*MATH_CONSTANTS::PI_VAL);

   return res;
}

//Return the Angle On Degree of Indicated Vector
const float Math_G::get_angle_fromVector(float x, float y){
    Vector2 vect=Vector2(x,y);
    vect=vect.normalize();
    float angl=Acos(vect.dot(Vector2(0,1)));
    angl=to_degree(angl);
    if(vect.x<0){
        angl=180+(180-angl);
    }
    return angl;
}

//Return the Vector with Smooth
const std::vector<float> Math_G::Smooth_Vector(float x_target,float y_target,float x_origin,float y_origin,float smooth_value){
   smooth_value=Clamp(smooth_value,0.0,1.0);
   float dif_x=(x_target-x_origin)*smooth_value;
   float dif_y=(y_target-y_origin)*smooth_value;
   float res_x=x_target-dif_x;
   float res_y=y_target-dif_y;
   std::vector<float>res={res_x,res_y};
   return res;
}
