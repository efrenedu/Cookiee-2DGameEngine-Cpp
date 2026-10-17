#ifndef MATH_STRUCTS_H
#define MATH_STRUCTS_H
#include<string>
#include<cmath>
#include<cstdint>
#include<vector>

//Math Constants
namespace MATH_CONSTANTS{
   constexpr float RADIANS=57.293821;
   constexpr float PI_VAL=3.141592653589793;
};

//for Math Operations
namespace Math_G{
    const bool is_number(std::string val);
    const float Clamp(float val,float minimo,float maximo);
    const float Abs(float val);
    const float Pow(float pow_val,float number_val);
    const float Atan2(float y_val,float x_val);
    const float Acos(float val);
    const float Asin(float val);
    const float Cos(float rad);
    const float Sin(float rad);
    const int get_random(int32_t mod,int32_t param1,int32_t param2,int32_t initial_seed=-1);
    const float to_rad(float angle);
    const float to_degree(float radians);
    const float Wrap_Angle(float angle);
    const float Wrap_AngleAccumulated(float new_radians,float last_radians,float radians_accumulated);
    const float get_angle_fromVector(float x ,float y);
    const std::vector<float> Smooth_Vector(float x_target,float y_target,float x_origin ,float y_origin,float smooth_value);
};



#endif // MATH_STRUCTS_H
