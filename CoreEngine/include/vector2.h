#ifndef VECTOR2_H
#define VECTOR2_H

#include<string>

//Store and Manage the Data of a Vector of Two Dimension: X,Y
struct Vector2{
    public:
       float x;
       float y;
       Vector2();
       Vector2(float xval, float yval);
       Vector2 normalize();
       float magnitud();
       std::string to_str();
       Vector2 operator+(Vector2 const& vec);
       Vector2 operator-(Vector2 const& vec);
       bool operator == (Vector2 const &other);
       bool operator != (Vector2 const &other);
       bool operator >=(Vector2 const& other);
       bool operator >(Vector2 const& other);
       bool operator <=(Vector2 const& other);
       bool operator < (Vector2 const& other);
       float dot(Vector2 vec);
};

#endif // VECTOR2_H
