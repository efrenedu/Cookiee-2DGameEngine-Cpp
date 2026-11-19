#include "Camera.h"
#include<Scene.h>


//Init the Camer
void Camera2D::Init(float pos_x,float pos_y,uint16_t w_view,uint16_t h_view,uint16_t target,float offset_targt_x,float offset_targt_y){
    x=pos_x;
    y=pos_y;
    width_viewport=w_view;
    height_viewport=h_view;
    half_width_viewport=width_viewport*0.5;
    half_height_viewport=height_viewport*0.5;
    follow=target;
    offset_targetX=offset_targt_x;
    offset_targetY=offset_targt_y;
    get_TargetSize=true;
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
  if(get_TargetSize){
    get_TargetSize=false;
    Render_data* render=target_scene->get_render_sys()->get_render(follow);
    if(render!=NULL){
       SDL_Rect rect;
       SDL_Texture *tex=target_scene->get_texture_system()->get_texture_fromSprite(rect,render->sprite_id);
       if(tex!=NULL){
           offset_targetX-=rect.w*0.5;
           offset_targetY-=rect.h*0.5;
        }
     }
  }
  if(transf==NULL){
    return;
  }
  float new_x=(transf->x-offset_targetX)-half_width_viewport;
  float new_y=((transf->y*-1)-offset_targetY)-half_height_viewport;
  set_position(new_x,new_y);
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

//Get the Entities from Quatree and store the Ids
void Camera2D::get_entities_Quatree(Quatree_System* quatree_sys){
   if(quatree_sys==NULL){
      return;
   }
   float cx=x+(width_viewport*0.5);
   float cy=(y+(height_viewport*0.5))*-1;
   float left=cx-width_viewport;
   float right=cx+width_viewport;
   float top=cy+height_viewport;
   float bottom=cy-height_viewport;
   near_entities=quatree_sys->get_entities_fromRect(left,right,top,bottom);

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

//Convert World Position to Screen Position
std::vector<float> Camera2D::WorlPos_To_ScreenPos(float x_world,float y_world){
   x_world-=x;
   y_world=(-y_world)-y;
   std::vector<float> res={x_world,y_world};
   return res;
}


//Get the List of Entities Id Near to Camera
std::vector<uint16_t> Camera2D::get_NearEntities(){
   return near_entities;
}
