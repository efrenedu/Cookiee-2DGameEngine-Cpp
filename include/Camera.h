#ifndef CAMERA_H
#define CAMERA_H
#include<cstdint>
class Scene;

//The Camera of Scene
struct Camera2D{
  public:
    void Init(float pos_x,float pos_y,uint16_t w_view,uint16_t h_view,uint16_t target);
    void update_renderPos();
    void follow_target(Scene* target_scene);
    void set_viewport(uint16_t w_view,uint16_t h_view);
    void set_position(float new_x,float new_y);
    bool hasMoved_significantly();
    float getLeft();
    float getRight();
    float getBottom();
    float getTop();
  private:
    uint16_t width_viewport=400;
    uint16_t height_viewport=400;
    uint16_t follow;
    float x=0;
    float y=0;
    float last_RenderX=0;
    float last_RenderY=0;
    float threshold=15.0f;


};

#endif // CAMERA_H
