#ifndef CAMERA_H
#define CAMERA_H
#include<cstdint>
#include<vector>
class Scene;
class Quatree_System;

//The Camera of Scene
struct Camera2D{
  public:
    void Init(float pos_x,float pos_y,uint16_t w_view,uint16_t h_view,uint16_t target,float offset_targt_x,float offset_targt_y);
    void update_renderPos();
    void follow_target(Scene* target_scene);
    void set_viewport(uint16_t w_view,uint16_t h_view);
    void set_position(float new_x,float new_y);
    bool hasMoved_significantly();
    void get_entities_Quatree(Quatree_System* quatree_sys);
    float getLeft();
    float getRight();
    float getBottom();
    float getTop();
    std::vector<float> WorlPos_To_ScreenPos(float x_world,float y_world);

    std::vector<uint16_t> get_NearEntities();

  private:
    uint16_t width_viewport=400;
    uint16_t height_viewport=400;
    uint16_t half_width_viewport=200;
    uint16_t half_height_viewport=200;
    uint16_t follow;
    float x=0;
    float y=0;
    float last_RenderX=0;
    float last_RenderY=0;
    float threshold=2.0f;
    float offset_targetX=0.0;
    float offset_targetY=0.0f;
    bool get_TargetSize;
    std::vector<uint16_t> near_entities;


};

#endif // CAMERA_H
