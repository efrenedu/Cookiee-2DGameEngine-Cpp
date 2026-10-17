#include "Camera.h"
#include<Scene.h>


//Init the Camer
void Camera2D::Init(float pos_x,float pos_y,uint16_t w_view,uint16_t h_view,uint16_t target){
    x=pos_x;
    y=pos_y;
    width_viewport=w_view;
    height_viewport=h_view;
    follow=target;
}
//Set Render Position
void Camera2D::update_renderPos(){
   last_RenderX=x;
   last_RenderY=y;
}

//Follow the Target Assignated
void Camera2D::follow_target(Scene* target_scene){
  if(follow>=target_scene->get_count_entities()){
     return;
  }
  Transform_Data* transf=target_scene->get_transform_sys()->get_transform(follow);
  if(transf==NULL){
    return;
  }
  float x_target=transf->x;
  float y_target=transf->y;

}


//set View Port
void Camera2D::set_viewport(uint16_t w_view,uint16_t h_view){
  width_viewport=w_view;
  height_viewport=h_view;
}

//Set Camera Position
void Camera2D::set_position(float new_x,float new_y){
  x=new_x;
  y=new_y;
}

//Return True if the Camera has Moved Significantly and is required Redraw All Window
bool Camera2D::hasMoved_significantly(){
  float dx=x-last_RenderX;
  float dy=y-last_RenderY;
  return((dx*dx)+(dy*dy)>(threshold*threshold));
}


/*Getter Methods (Positions of Camera Rect)*/

//Get Position of Extreme Left of Camera
float Camera2D::getLeft(){
    return x;
}

//Get Position of Extreme Right of Camera
float Camera2D::getRight(){
    return x+width_viewport;
}

//Get Position of Extreme Bottom of Camera
float Camera2D::getBottom(){
    return y;
}

//Get Position of Extreme Top of Camera
float Camera2D::getTop(){
    return y+height_viewport;

}
