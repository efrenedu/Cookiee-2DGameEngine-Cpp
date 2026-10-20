#include "vector2.h"
#include <cmath>

Vector2::Vector2(){
   x=0;
   y=0;
}
//Build a Vector of Two Dimension
Vector2::Vector2(float xval,float yval){
   x=xval;
   y=yval;
}

//Return the Vector as String
std::string Vector2::to_str(){
   std::string msg="";
   msg=msg+"( "+std::to_string(x)+" , "+std::to_string(y)+" )";
   return msg;
}

//return the Vector Normalized
Vector2 Vector2::normalize(){
   Vector2 res=Vector2(x,y);
   float mag=magnitud();
   if(mag>0){
      res.x=x/mag;
      res.y=y/mag;
   }
   return res;
}

//Return the Magnitude of Vector
float Vector2::magnitud(){
   float res=0.0f;
   res=sqrt(pow(x,2)+pow(y,2));
   return res;
}

//Return the Addition of Two Vectors
Vector2 Vector2::operator+(Vector2 const& vec){
    Vector2 res=Vector2(x,y);
    res.x=res.x+vec.x;
    res.y=res.y+vec.y;
    return res;
}

//Return the Substraction of Two Vectors
Vector2 Vector2::operator-(Vector2 const& vec){
    Vector2 res=Vector2(x,y);
    res.x=res.x-vec.x;
    res.y=res.y-vec.y;
    return res;
}

//Return True if the Indicated Vector components are greater or equals
bool Vector2:: operator >=(Vector2 const& other){
    if(other.x>=x && other.y>=y){
        return true;
    }
    return false;
}

//Return True if the Indicated Vector components are greater
bool Vector2:: operator >(Vector2 const& other){
    if(other.x>x && other.y>y){
        return true;
    }
    return false;
}
//Return True if the Indicated Vector components are Lower or equals
bool Vector2:: operator <=(Vector2 const& other){
    if(other.x<=x && other.y<=y){
        return true;
    }
    return false;
}
//Return True if the Indicated Vector components are Lower
bool Vector2:: operator < (Vector2 const& other){
    if(other.x<x && other.y<y){
       return true;
    }
    return false;
}

//Return the Dot Product of this vector and the Indicated Vector
float Vector2::dot(Vector2 vec){
    float res=(x*vec.x)+(y*vec.y);
    return res;
 }

//Return True if the Other Vector Component are equals to this vector
bool Vector2::operator == (Vector2 const &other){
   if(other.x==x && other.y==y){
       return true;
   }
   return false;
}
//Return True if the Other Vector Component are Not equals to this vector
bool Vector2::operator != (Vector2 const &other){
   if(other.x!=x || other.y!=y){
       return true;
   }
   return false;
}
