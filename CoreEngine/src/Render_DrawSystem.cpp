#include "Render_DrawSystem.h"
#include "math_structs.h"
#include<sstream>

//Init the System
bool Render_DrawSystem::Init_System(){
    splash_win=NULL;
    splash_render=NULL;
    main_render=NULL;
    main_win=NULL;
    if(!(IMG_Init(IMG_INIT_PNG))){
        std::cout<<"PNG init fail"<<std::endl;
         return false;
    }
    if(!(IMG_Init(IMG_INIT_JPG))){
        std::cout<<"JPG init fail"<<std::endl;
        return false;
    }
    int flags=SDL_RENDERER_ACCELERATED;
    splash_win =SDL_CreateWindow( "iniciando", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 400, SDL_WINDOW_SHOWN);
    if(splash_win==NULL){
        std::cout<<"Error Init Splash Windows"<<std::endl;
        return false;
    }
    splash_render=SDL_CreateRenderer(splash_win,-1,0);
    if(splash_render==NULL){
         std::cout<<"Erro Init Splash Renderer"<<std::endl;
         return false;
    }
    SDL_SetRenderDrawColor(splash_render,0,0,0,255);
    SDL_RenderClear(splash_render);
    SDL_RenderPresent(splash_render);
    redraw=REDRAW_ALL;
    return true;
}

//Init the Main Windows
bool Render_DrawSystem::Init_Main_Windows(std::unordered_map<std::string,std::string>config_win){
   if(splash_render!=NULL){
       SDL_DestroyRenderer(splash_render);
   }
   if(splash_win!=NULL){
      SDL_DestroyWindow(splash_win);
   }
   std::vector<std::string> targets={"Width","Height","Title","FullScreen","Target_SizeWidth","Target_SizeHeight"};
   for(std::string id:targets){
      auto it=config_win.find(id);
      if(it==config_win.end()){
          std::cout<<"Fail Load Windows Configuration"<<std::endl;
          return false;
      }
   }
   std::string title=config_win["Title"];
   uint16_t w_win=Converter::strTo_UInt(config_win["Width"]);
   uint16_t h_win=Converter::strTo_UInt(config_win["Height"]);
   uint16_t target_w=Converter::strTo_UInt(config_win["Target_SizeWidth"]);
   uint16_t target_h=Converter::strTo_UInt(config_win["Target_SizeHeight"]);
   uint32_t flags_win=SDL_WINDOW_SHOWN;
   main_win=SDL_CreateWindow( title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w_win, h_win,flags_win);
   if(main_win==NULL){
      std::cout<<"Error Init Main Windows"<<std::endl;
      return false;
   }
   uint32_t flags_render=SDL_RENDERER_ACCELERATED ;
   main_render=SDL_CreateRenderer(main_win,-1,flags_render);
   if(main_render==NULL){
     std::cout<<"Error Init Render of Main Windows"<<std::endl;
      return false;
   }
   SDL_RenderSetLogicalSize(main_render,target_w,target_h);
   SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
   bg_texture=SDL_CreateTexture(main_render,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET,target_w,target_h);
   SDL_SetRenderDrawBlendMode(main_render, SDL_BLENDMODE_BLEND);
   return true;
}

//Free System
void Render_DrawSystem::Free_System(){
  if(main_render!=NULL){
      SDL_DestroyRenderer(main_render);
  }
  if(main_win!=NULL){
    SDL_DestroyWindow(main_win);
  }
  if(splash_render!=NULL){
      SDL_DestroyRenderer(splash_render);
  }
  if(splash_win!=NULL){
    SDL_DestroyWindow(splash_win);
  }
  if(bg_texture!=NULL){
    SDL_DestroyTexture(bg_texture);
  }
  redraw=REDRAW_NONE;

}

//Request Redraw the Scene
void Render_DrawSystem::request_redraw(uint8_t redraw_type){
   if(redraw==REDRAW_ALL){
      return;
   }
   redraw=redraw_type;
}

//Draw the Scene
void  Render_DrawSystem::draw_scene(Scene* target_scene){
    if(redraw==REDRAW_NONE){
        return;
    }
    System_TileMap* tilemap=target_scene->get_system_tilemap();
    Camera2D* main_cam=target_scene->get_camera();
    textureManager_system* texture_sys=target_scene->get_texture_system();
    draw_tilemap(texture_sys,tilemap,main_cam);
    draw_entities(texture_sys,target_scene->get_count_entities(),target_scene,main_cam);
    draw_Particles(texture_sys,main_cam);
    draw_UI(texture_sys,target_scene);
    SDL_RenderPresent(main_render);
    redraw=REDRAW_NONE;
    render_queue.clear();

}

//Draw Active Entities
void  Render_DrawSystem::draw_entities(textureManager_system* texture_sys,uint16_t num_enities,Scene* target_scene,Camera2D* cam,bool draw_colliders_borders){
    float left_cam=cam->getLeft();
    float right_cam=cam->getRight();
    float top_cam=cam->getTop();
    float bottom_cam=cam->getBottom();
    Transform_System* transf_sys=target_scene->get_transform_sys();
    Render_System* render_sys=target_scene->get_render_sys();
    RigidBody_System* body_sys=target_scene->get_rigidbody_sys();
    float alpha=0.4;
    std:vector<uint16_t> cam_near=cam->get_NearEntities();
    for(uint16_t index:cam_near){
          Transform_Data* data_transform=transf_sys->get_transform(index);
          float x=data_transform->x;
          float y=data_transform->y*-1;
          float dif_x=0;
          float dif_y=0;
          float angle=data_transform->angle;
          bool flipped=data_transform->flipped;
          uint16_t scale=data_transform->scale;
          uint16_t width=32;
          uint16_t height=32;
          float screen_x=x-left_cam;
          float screen_y=y-bottom_cam;
          SDL_Rect rect_obj;
          uint16_t color=0;

          std::bitset<64> mask_entity=target_scene->get_mask(index);
          bool is_collider=false;
          bool is_rendered=false;
          bool is_composer=false;
          if(HasComponentBit(mask_entity,UI_COMP)){
              continue;
          }
          if(HasComponentBit(mask_entity,COLLIDER_COMP)){
             is_collider=true;
          }

          if(HasComponentBit(mask_entity,RENDER_COMP)){
             is_rendered=true;
          }
          if(HasComponentBit(mask_entity,SPRITECOMPOSER_COMP)){
             is_composer=true;
          }
          if(is_rendered==false &&(is_collider==true && draw_colliders_borders==true)==false){
             continue;
          }
          if(is_collider==true && draw_colliders_borders==true){
             SDL_Rect collider_rect;
             collider_rect.x=screen_x;
             collider_rect.y=screen_y;
             Collider_System* collider_sys=target_scene->get_collider_sys();
             Collider_Data* collider=collider_sys->get_collider(index);
             SDL_SetRenderDrawColor(main_render,255,0,0,255);
             if(collider->collider_type==FIGURE_TYPE::FIGURE_CIRCLE){
                collider_rect.w=collider->shape.circle.radius*2;
                collider_rect.h=collider->shape.circle.radius*2;

                SDL_Point pt;
                render_queue.push_back({
                     NULL,
                     collider_rect,
                     rect_obj,
                     color,
                     pt,
                     0,
                     100,
                     false,
                     false,
                     RENDER_BORDER_ONLY
                 });
                 std::stable_sort(render_queue.begin(),render_queue.end(),[](const RenderCommand&a, const RenderCommand& b){
                        return a.z_index<b.z_index;
                });

             }
             else if(collider->collider_type==FIGURE_TYPE::FIGURE_SQUARE){
                collider_rect.w=collider->shape.box.width;
                collider_rect.h=collider->shape.box.height;
                SDL_Point pt;
                render_queue.push_back({
                     NULL,
                     collider_rect,
                     rect_obj,
                     color,
                     pt,
                     0,
                     100,
                     false,
                     false,
                     RENDER_BORDER_ONLY
                 });
                 std::stable_sort(render_queue.begin(),render_queue.end(),[](const RenderCommand&a, const RenderCommand& b){
                        return a.z_index<b.z_index;
                });

             }
          }
          if(is_rendered==false){
            continue;
          }
          Render_data* render_dat=render_sys->get_render(index);
          if(render_dat->visible==false){
             continue;
          }
          SDL_Rect rect_sprite;
          SDL_Texture* tex=texture_sys->get_texture_fromSprite(rect_sprite,render_dat->sprite_id);
          rect_obj.x=screen_x;
          rect_obj.y=screen_y;
          if(tex!=NULL){
            width=rect_sprite.w*scale;
            height=rect_sprite.h*scale;
            rect_obj.x=x-(width*0.5);
            rect_obj.y=y-(height*0.5);
          }
          rect_obj.x-=left_cam;
          rect_obj.y-=bottom_cam;
          rect_obj.w=width;
          rect_obj.h=height;
          bool on_Camera=(x+width>=left_cam)&&(x<=right_cam)&&(y+height>=bottom_cam) &&(y<=top_cam);
          if(on_Camera==false){
              continue;
          }
          if(is_composer){
            float render_x=screen_x;
            float render_y=screen_y;
            if(HasComponentBit(mask_entity,RIGIDBODY_COMP)){
               RigidBody_Data* rb=body_sys->get_body(index);
               float old_x=rb->last_MoveX-left_cam;
               float old_y=(rb->last_MoveY*-1)-bottom_cam;
               render_y=Math_G::lerp_Value(old_y,screen_y,alpha);
               render_x=Math_G::lerp_Value(old_x,screen_x,alpha);
            }
            draw_spriteComposer(target_scene,texture_sys,index,render_dat->z_index,render_x,render_y,angle,data_transform->initialAngle,flipped);
            continue;
          }
          if(HasComponentBit(mask_entity,RIGIDBODY_COMP)){
               RigidBody_Data* rb=body_sys->get_body(index);
               float old_x=(rb->last_MoveX-(rect_sprite.w*0.5))-left_cam;
               float old_y=((rb->last_MoveY*-1)-(rect_sprite.h*0.5))-bottom_cam;
               rect_obj.y=Math_G::lerp_Value(old_y,rect_obj.y,alpha);
               rect_obj.x=Math_G::lerp_Value(old_x,rect_obj.x,alpha);
          }
          SDL_Point point_pivot;
          point_pivot.x=rect_sprite.w*0.5;
          point_pivot.y=rect_sprite.h*0.5;
          render_queue.push_back({
              tex,
              rect_obj,
              rect_sprite,
              color,
              point_pivot,
              angle,
              render_dat->z_index,
              false,
              flipped,
              RENDER_BORDER_NONE
          });
          std::stable_sort(render_queue.begin(),render_queue.end(),[](const RenderCommand&a, const RenderCommand& b){
                 return a.z_index<b.z_index;
          });

    }

    for (const RenderCommand& cmd:render_queue){

        SDL_Texture* tex=cmd.tex_id;
        SDL_Rect rect_world=cmd.rect_world;
        bool render_border=cmd.render_border;
        if(cmd.render_border!=RENDER_BORDER_NONE){
           SDL_SetRenderDrawColor(main_render,255,255,255,255);
           SDL_RenderDrawRect(main_render,&rect_world);
           if(cmd.render_border==RENDER_BORDER_ONLY){
              continue;
           }
        }

        if(tex==NULL){
            SDL_SetRenderDrawColor(main_render,255,255,255,255);
            SDL_RenderFillRect(main_render,&rect_world);
            continue;
        }
        SDL_Rect rect_sprite=cmd.rect_sprite;
        SDL_Point pivot=cmd.pivot;
        uint16_t angle=cmd.angle;
        bool flipped=cmd.flipped;
        auto flip_val=SDL_FLIP_NONE;

        if(flipped){
             flip_val=SDL_FLIP_HORIZONTAL;
        }
        SDL_RenderCopyEx(main_render,tex,&rect_sprite,&rect_world,static_cast<double>(angle),&pivot,flip_val);

    }
    render_queue.clear();

}

//Draw Particles
void Render_DrawSystem::draw_Particles(textureManager_system* texture_sys,Camera2D* cam){
    for (const RenderCommand& cmd:render_queue){

        SDL_Texture* tex=cmd.tex_id;
        SDL_Rect rect_world=cmd.rect_world;
        if(cmd.render_border!=RENDER_BORDER_NONE){
            SDL_SetRenderDrawColor(main_render,255,255,255,255);
            SDL_RenderDrawRect(main_render,&rect_world);
            if(cmd.render_border==RENDER_BORDER_ONLY){
                continue;
            }
        }
        if(tex==NULL){
            uint8_t r,g,b,a;
            Converter::unpackRGBA4444(cmd.color,r,g,b,a);
            SDL_Color color;
            color.r=r;
            color.g=g;
            color.b=b;
            color.a=a;
            SDL_SetRenderDrawColor(main_render,color.r,color.g,color.b,color.a);
            SDL_RenderFillRect(main_render,&rect_world);
            continue;
        }
        SDL_Rect rect_sprite=cmd.rect_sprite;
        SDL_Point pivot=cmd.pivot;
        uint16_t angle=cmd.angle;
        bool flipped=cmd.flipped;
        auto flip_val=SDL_FLIP_NONE;

        if(flipped){
             flip_val=SDL_FLIP_HORIZONTAL;
        }
        SDL_RenderCopyEx(main_render,tex,&rect_sprite,&rect_world,static_cast<double>(angle),&pivot,flip_val);
   }
   render_queue.clear();

}

//Draw UI
void Render_DrawSystem::draw_UI(textureManager_system* texture_sys,Scene* target_scene){
    Transform_System* transf_sys=target_scene->get_transform_sys();
    Render_System* render_sys=target_scene->get_render_sys();
    UI_System* ui_sys=target_scene->get_UI_sys();
    Fonts_System* font_sys=target_scene->get_font_sys();
    uint16_t w_view=0;
    uint16_t h_view=0;

    for(uint16_t i=0;i<target_scene->get_count_entities();i++){
          std::bitset<64> mask_entity=target_scene->get_mask(i);
          if(HasComponentBit(mask_entity,UI_COMP)==false){
             continue;
          }
          Transform_Data* data_transform=transf_sys->get_transform(i);

          float x=data_transform->x;
          float y=data_transform->y;
          bool flipped=data_transform->flipped;
          uint16_t width=32;
          uint16_t height=32;
          SDL_Rect rect;
          SDL_Rect fg_rect;
          uint16_t color=0;
          uint16_t fg_color=0;
          bool is_rendered=false;
          bool is_composer=false;
          bool draw_bg=false;
          bool draw_fg=false;
          uint16_t bg_color=0;
          uint16_t angle_sprite=0;
          SDL_Rect bg_rect;
          uint8_t z_index=0;
          bool can_draw_scrollbars=false;
          SDL_Rect rect_container_scroll;


          if(HasComponentBit(mask_entity,RENDER_COMP)){
             is_rendered=true;
          }
          if(HasComponentBit(mask_entity,SPRITECOMPOSER_COMP)){
             is_composer=true;
          }
          SDL_Texture* tex=NULL;
          SDL_Texture* fg_tex=NULL;
          SDL_Texture* bg_tex=NULL;
          SDL_Rect rect_sprite;
          SDL_Rect fg_sprite_rect;
          SDL_Rect bg_sprite_rect;
          if(is_rendered){
             Render_data* render_dat=render_sys->get_render(i);
             if(render_dat->visible==false){
                 continue;
             }
             z_index=render_dat->z_index;
             tex=texture_sys->get_texture_fromSprite(rect_sprite,render_dat->sprite_id);
          }
          rect.x=x;
          rect.y=y;
          if(tex!=NULL){
            width=rect_sprite.w;
            height=rect_sprite.h;
          }
          uint8_t border=RENDER_BORDER_NONE;
          uint8_t bg_border=RENDER_BORDER_NONE;
          uint16_t border_color=0;
          rect.w=width;
          rect.h=height;

          UI_header* header=ui_sys->get_UIHeader(i);

          Label* lbl=ui_sys->get_label(i);
          ScrollBar* scroll=ui_sys->get_scrollbar(i);
          bool use_color=false;
          bool ignore_ui=false;
          switch(header->main_type){
             case UI_BUTTON:{
                Button* btn=ui_sys->get_button(i);
                if(btn!=NULL){
                   color=btn->colors_states[btn->actual_state];
                   if(tex!=NULL){
                      if(is_composer){
                          SDL_Rect rect_composer;
                          draw_UiSpriteComposer(target_scene,texture_sys,i,z_index,header->centerX,header->centerY,rect_composer);
                          ignore_ui=true;
                          break;
                      }
                   }
                   else{
                     use_color=true;
                     rect.x=header->centerX-header->half_width;
                     rect.y=header->centerY-header->half_height;
                     rect.w=header->half_width*2;
                     rect.h=header->half_height*2;

                  }
                }
                break;
             }
             case UI_PANEL:{
                Panel* pnl=ui_sys->get_panel(i);
                if(pnl!=NULL){
                   can_draw_scrollbars=true;
                   if(tex!=NULL && is_composer){
                        SDL_Rect rect_composer;
                        draw_UiSpriteComposer(target_scene,texture_sys,i,z_index,header->centerX,header->centerY,rect_composer);
                        rect_container_scroll=rect_composer;
                        ignore_ui=true;
                        break;
                    }
                    else{
                      tex=NULL;
                      use_color=true;
                      color=pnl->bg_color;
                      rect.x=header->centerX-header->half_width;
                      rect.y=header->centerY-header->half_height;
                      rect.w=header->half_width*2;
                      rect.h=header->half_height*2;
                      rect_container_scroll=rect;
                      if(pnl->draw_border){
                         border=RENDER_BORDER_ONLY;
                         border_color=pnl->border_color;
                      }
                    }

                }
                break;
             }
             case UI_VALUEBAR:{
               ValueBar* bar=ui_sys->get_valueBar(i);
               if(bar!=NULL){
                  bg_rect.x=rect.x;
                  bg_rect.y=rect.y;
                  bg_rect.w=bar->width_bar;
                  bg_rect.h=bar->height_bar;
                  rect.w=bar->width_bar;
                  rect.h=bar->height_bar;
                  bg_color=bar->bg_color;
                  color=bar->bar_colors[bar->bar_state];
                  use_color=true;
                  draw_bg=true;
                  if(bar->orientation==UI_ORIENTATION_HORIZONTAL){
                     rect.w=bar->value*bar->width_bar;
                  }
                  else{
                     rect.h=bar->value*bar->height_bar;
                  }
                  tex=NULL;
                  if(bar->use_bar_colors){
                    if(bar->border_val>0){
                        bg_border=RENDER_BORDER_APPEND;
                        bg_rect.x-=bar->border_val;
                        bg_rect.y-=bar->border_val;
                        bg_rect.w+=bar->border_val*2;
                        bg_rect.h+=bar->border_val*2;

                     }
                  }
                  else{
                     draw_bg=false;
                     use_color=false;
                     draw_Valuebar_Sprites(bg_rect,rect,texture_sys,z_index,bar,ui_sys);
                  }
               }
               break;
             }
             case UI_SLIDER:{
               Slider* slider=ui_sys->get_slider(i);
               if(slider!=NULL){
                  bg_rect.x=rect.x;
                  bg_rect.y=rect.y;
                  bg_rect.w=slider->max_w;
                  bg_rect.h=slider->max_h;
                  rect.w=bg_rect.w;
                  rect.h=bg_rect.h;
                  bg_color=slider->bg_color;
                  color=slider->bar_color;
                  fg_color=slider->slider_color;
                  use_color=true;
                  draw_bg=true;
                  draw_fg=true;
                  fg_tex=texture_sys->get_texture_fromSprite(fg_sprite_rect,slider->slider_sprite);
                  float min_size=std::min(bg_rect.w,bg_rect.h);
                  fg_rect.w=min_size;
                  fg_rect.h=min_size;
                  fg_rect.x=rect.x;
                  fg_rect.y=rect.y;
                  if(slider->orientation==UI_ORIENTATION_HORIZONTAL){
                     rect.w=slider->value*slider->max_w;
                     fg_rect.x=(rect.x+rect.w)-fg_rect.w;
                     float porcent_increment=0.2;
                     uint16_t offset_y=fg_rect.h*porcent_increment;
                     fg_rect.h+=offset_y*2;
                     fg_rect.y-=offset_y;
                  }
                  else{
                     rect.h=slider->value*slider->max_h;
                     fg_rect.y=(rect.y+rect.w)-fg_rect.w;
                     float porcent_increment=0.2;
                     uint16_t offset_x=fg_rect.w*porcent_increment;
                     fg_rect.w+=offset_x*2;
                     fg_rect.x-=offset_x;
                  }
               }
                break;
             }
             case UI_CHECKBUTTON:{
                CheckButton* check=ui_sys->get_checkBtn(i);
                if(check!=NULL){
                     if(tex==NULL){
                        use_color=true;
                        draw_bg=true;
                        rect.w=header->half_width*2;
                        rect.h=header->half_height*2;
                        bg_color=check->bg_color;
                        color=0;
                        bg_rect.x=rect.x;
                        bg_rect.y=rect.y;
                        bg_rect.w=rect.w;
                        bg_rect.h=rect.h;
                        if(check->state==OPTION_DISABLED){
                             color=check->disabled_color;
                             bg_color=check->disabled_color;
                        }
                        else if(check->state==OPTION_CHECKED){
                              color=check->button_color;
                              float reduce_factor=0.125;
                              uint16_t w_reduce=rect.w*reduce_factor;
                              uint16_t h_reduce=rect.h*reduce_factor;
                              rect.w-=w_reduce*2;
                              rect.h-=h_reduce*2;
                              rect.x+=w_reduce;
                              rect.y+=h_reduce;

                        }

                     }
                }
                break;
             }
             case UI_RADIOBUTTON:{
                Radio_Button* btn=ui_sys->get_radioBtn(i);
                if(btn!=NULL){
                     if(tex==NULL){
                        use_color=true;
                        draw_bg=true;
                        rect.w=header->half_width*2;
                        rect.h=header->half_height*2;
                        bg_color=btn->bg_color;
                        color=0;
                        bg_rect.x=rect.x;
                        bg_rect.y=rect.y;
                        bg_rect.w=rect.w;
                        bg_rect.h=rect.h;
                        if(btn->state==OPTION_DISABLED){
                             color=btn->disabled_color;
                             bg_color=btn->disabled_color;
                        }
                        else if(btn->state==OPTION_CHECKED){
                              color=btn->button_color;
                              float reduce_factor=0.125;
                              uint16_t w_reduce=rect.w*reduce_factor;
                              uint16_t h_reduce=rect.h*reduce_factor;
                              rect.w-=w_reduce*2;
                              rect.h-=h_reduce*2;
                              rect.x+=w_reduce;
                              rect.y+=h_reduce;

                        }

                     }
                }
                break;
             }
             case UI_ICON:{
                if(tex!=NULL && is_composer){
                    SDL_Rect ref_rect;
                    draw_UiSpriteComposer(target_scene,texture_sys,i,z_index,header->centerX,header->centerY,ref_rect);
                    ignore_ui=true;
                }
                if(tex==NULL){
                    use_color=true;
                    color=0xFFFF;
                }
             }
             case UI_TEXTFIELD:{
                 break;
             }
             case UI_COMBOBOX:{
                ComboBox* box=ui_sys->get_comboBox(i);
                if(box!=NULL){
                    draw_bg=true;
                    use_color=true;
                    draw_fg=true;
                    bg_rect.x=rect.x;
                    bg_rect.y=rect.y;
                    bg_rect.w=box->width_box;
                    color=box->itemHover_color;
                    uint16_t sz_btn=box->min_height*box->porceSize_btn;

                    if(box->open==false){
                        bg_rect.h=box->min_height;
                        color=0;
                    }
                    else{
                        can_draw_scrollbars=true;
                        bg_rect.h=box->max_height;
                        if(box->selected_item==-1){
                             color=0;
                        }
                        else{
                           rect.h=box->min_height;
                           rect.w=box->width_box;
                           rect.y=bg_rect.y+((box->selected_item-box->offset_items)*box->min_height);
                           color=box->itemHover_color;
                        }
                        rect_container_scroll=bg_rect;
                        rect_container_scroll.y+=box->min_height;
                        rect_container_scroll.h-=box->max_height;
                    }

                    rect.w=box->width_box;
                    rect.h=box->min_height;
                    fg_rect.w=sz_btn;
                    fg_rect.h=sz_btn;
                    fg_rect.x=(bg_rect.x+bg_rect.w)-sz_btn;
                    fg_rect.y=bg_rect.y;
                    fg_color=box->color_btn;
                    bg_color=box->bg_color;
                    fg_tex=texture_sys->get_texture_fromSprite(fg_sprite_rect,box->sprite_btn);

                }
                break;
             }
             case UI_LIST:{
                List_Box* list_bx=ui_sys->get_list_box(i);
                if(list_bx!=NULL){
                     rect.h=header->half_height*2;
                     rect.w=header->half_width*2;
                     can_draw_scrollbars=true;
                     if(is_composer){
                        draw_UiSpriteComposer(target_scene,texture_sys,i,z_index,header->centerX,header->centerY,rect_container_scroll);
                     }
                     else{
                         bg_rect.x=rect.x;
                         bg_rect.y=rect.y;
                         bg_rect.h=rect.h;
                         bg_rect.w=rect.w;
                         rect_container_scroll.x=bg_rect.x;
                         rect_container_scroll.y=bg_rect.y;
                         rect_container_scroll.w=bg_rect.h;
                         rect_container_scroll.w=bg_rect.w;

                         draw_bg=true;
                         bg_color=list_bx->bg_color;
                     }
                     rect.h=list_bx->heigth_items;
                     rect.y=bg_rect.y+((list_bx->selected_item-list_bx->offset)*rect.h);
                     color=list_bx->item_color;
                     use_color=true;
                     fg_tex=texture_sys->get_texture_fromSprite(fg_sprite_rect,list_bx->icon_sprite);
                     if(fg_tex!=NULL){
                         draw_fg=true;
                         fg_rect.x=rect.x-fg_sprite_rect.w;
                         fg_rect.y=rect.y;
                         fg_rect.w=fg_sprite_rect.w;
                         fg_rect.h=fg_sprite_rect.h;
                     }
                }
                break;
             }
             default:break;

          }


          SDL_Point point_pivot;
          point_pivot.x=rect_sprite.w*0.5;
          point_pivot.y=rect_sprite.h*0.5;

          if(ignore_ui==false){
                if(draw_bg){
                   SDL_Point bg_point;
                   bg_point.x=bg_sprite_rect.w*0.5;
                   bg_point.y=bg_sprite_rect.h*0.5;
                   bool use_bg_color=true;
                   if(bg_tex!=NULL){
                      use_bg_color=false;
                   }
                   RenderCommand cmd_bg;
                   cmd_bg.rect_world=bg_rect;
                   cmd_bg.z_index=z_index-1;
                   cmd_bg.use_color=use_bg_color;
                   cmd_bg.color=bg_color;
                   cmd_bg.flipped=false;
                   cmd_bg.pivot=bg_point;
                   cmd_bg.angle=0;
                   cmd_bg.rect_sprite=bg_sprite_rect;
                   cmd_bg.render_border=bg_border;
                   cmd_bg.tex_id=bg_tex;
                   render_queue.push_back(cmd_bg);
                }
                if(draw_fg){
                   bool use_fg_color=true;
                   if(fg_tex!=NULL){
                      use_fg_color=false;
                   }
                   SDL_Point fg_point;
                   fg_point.x=fg_sprite_rect.w*0.5;
                   fg_point.y=fg_sprite_rect.h*0.5;

                   RenderCommand cmd_fg;
                   cmd_fg.rect_world=fg_rect;
                   cmd_fg.z_index=z_index+1;
                   cmd_fg.use_color=use_fg_color;
                   cmd_fg.color=fg_color;
                   cmd_fg.flipped=false;
                   cmd_fg.pivot=fg_point;
                   cmd_fg.angle=0;
                   cmd_fg.rect_sprite=fg_sprite_rect;
                   cmd_fg.render_border=RENDER_BORDER_NONE;
                   cmd_fg.tex_id=fg_tex;
                   render_queue.push_back(cmd_fg);
                }
                RenderCommand cmd_ui;
                cmd_ui.tex_id=tex;
                cmd_ui.rect_world=rect;
                cmd_ui.rect_sprite=rect_sprite;
                cmd_ui.color=color;
                cmd_ui.angle=angle_sprite;
                cmd_ui.z_index=z_index;
                cmd_ui.pivot=point_pivot;
                cmd_ui.use_color=use_color;
                cmd_ui.render_border=RENDER_BORDER_NONE;
                cmd_ui.flipped=flipped;
                render_queue.push_back(cmd_ui);

                if(border!=RENDER_BORDER_NONE){
                    RenderCommand cmd;
                    cmd.tex_id=NULL;
                    cmd.rect_world=rect;
                    cmd.color=border_color;
                    cmd.render_border=border;
                    cmd.z_index=z_index+1;
                    render_queue.push_back(cmd);
               }
          }
          //Draw Scrollbar of Components
          if(can_draw_scrollbars==true &&scroll!=NULL ){
              draw_scrollBar(scroll,texture_sys,rect_container_scroll,z_index);

          }
          //draw Text Only if has Texture
          if(lbl!=NULL ){

             RenderCommand lbl_command;
             SDL_Rect rect_font;
             SDL_Rect rect_lbl;
             rect_lbl.x=x+lbl->offsetx;
             rect_lbl.y=y+lbl->offsety;
             rect_lbl.w=0;
             rect_lbl.h=0;
             Bitmap_Font* font=font_sys->get_font(lbl->font_index);
             if(font==NULL){
                 continue;
             }
             SDL_Texture* text_texture= draw_text(font_sys,lbl,rect_font,font);
             if(text_texture==NULL){
                 continue;
             }
             rect_lbl.w=rect_font.w;
             rect_lbl.h=rect_font.h;
             uint16_t color_lbl=lbl->colors_states[lbl->actual_state];
             lbl_command.z_index=z_index+1;
             lbl_command.angle=0;
             lbl_command.flipped=false;
             lbl_command.pivot=point_pivot;
             lbl_command.render_border=RENDER_BORDER_NONE;
             lbl_command.use_color=true;
             lbl_command.color=color_lbl;
             lbl_command.rect_world=rect_lbl;
             lbl_command.rect_sprite=rect_font;
             lbl_command.tex_id=text_texture;
             render_queue.push_back(lbl_command);
          }
          std::stable_sort(render_queue.begin(),render_queue.end(),[](const RenderCommand&a, const RenderCommand& b){
             return a.z_index<b.z_index;
          });

    }

    for (const RenderCommand& cmd:render_queue){

        SDL_Texture* tex=cmd.tex_id;
        SDL_Rect rect_world=cmd.rect_world;
        if(cmd.render_border!=RENDER_BORDER_NONE){
            uint8_t r,g,b,a;
            Converter::unpackRGBA4444(cmd.color,r,g,b,a);
            SDL_SetRenderDrawColor(main_render,r,g,b,a);
            SDL_RenderDrawRect(main_render,&rect_world);
            if(cmd.render_border==RENDER_BORDER_ONLY){
                 continue;
            }

        }
        if(tex==NULL && cmd.use_color){
            uint8_t r,g,b,a;
            Converter::unpackRGBA4444(cmd.color,r,g,b,a);
            SDL_SetRenderDrawColor(main_render,r,g,b,a);
            SDL_RenderFillRect(main_render,&rect_world);
            continue;
        }
        if(tex==NULL){
             continue;
        }
        SDL_Rect rect_sprite=cmd.rect_sprite;
        SDL_Point pivot=cmd.pivot;
        uint16_t angle=cmd.angle;
        bool flipped=cmd.flipped;
        auto flip_val=SDL_FLIP_NONE;

        if(flipped){
             flip_val=SDL_FLIP_HORIZONTAL;
        }
        if(cmd.use_color){
            uint8_t r,g,b,a;
            Converter::unpackRGBA4444(cmd.color,r,g,b,a);
            SDL_SetTextureColorMod(tex,r,g,b);
            SDL_RenderCopyEx(main_render,tex,&rect_sprite,&rect_world,static_cast<double>(angle),&pivot,flip_val);
            SDL_SetTextureColorMod(tex,255,255,255);

        }
        else{
            SDL_RenderCopyEx(main_render,tex,&rect_sprite,&rect_world,static_cast<double>(angle),&pivot,flip_val);
        }

    }
    render_queue.clear();
}


//Draw a Text
SDL_Texture* Render_DrawSystem::draw_text(Fonts_System* font_sys,Label*lbl,SDL_Rect& rect,Bitmap_Font* target_font){
   if(font_sys==NULL || lbl==NULL || target_font==NULL ){
      return NULL;
   }
    Rendered_Text* last_text=font_sys->get_Rendered_Text(lbl->last_text_index);
    rect.x=0;
    rect.y=0;
    rect.w=0;
    rect.h=0;
    if(last_text!=NULL && lbl->has_modify==false){
        rect.w=last_text->width;
        rect.h=last_text->height;
        return last_text->texture_text;
    }

    std::string text=lbl->text_label;

    uint16_t width_box=lbl->label_boxWidth;
    uint16_t height_box=lbl->label_boxHeight;

    SDL_Texture* font_texture=target_font->get_texture();

    if(font_texture==NULL){
         return NULL;
    }
    SDL_Texture* initial_target=SDL_GetRenderTarget(main_render);
    SDL_Texture* new_text_texture=NULL;
    if(lbl->has_modify==true && last_text!=NULL){
        new_text_texture=last_text->texture_text;
        lbl->has_modify=false;
    }
    if(new_text_texture==NULL){
        new_text_texture=SDL_CreateTexture(main_render,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,width_box,height_box);
        SDL_SetTextureBlendMode(new_text_texture,SDL_BLENDMODE_BLEND);
    }

    SDL_SetRenderTarget(main_render,new_text_texture);
    SDL_SetRenderDrawColor(main_render,0,0,0,0);
    SDL_RenderClear(main_render);
    uint16_t last_x=0;
    uint16_t last_y=0;
    std::vector<std::string> lines;
    if(lbl->single_line){
         lines={text};
    }
    else{
       text=wrap_text(text,width_box,target_font);
       std::vector<char> ignore_chars;
       lines=Converter::str_toVect(text,'\n',ignore_chars);
    }
    uint8_t line_h=0;
    for(std::string temp_text:lines){
         if(temp_text.empty()){
             continue;
         }
         if(lbl->align!=LABEL_ALIGN_LEFT){
            uint16_t w_line=target_font->get_expected_width(temp_text);
            int16_t dif=width_box-w_line;
            if(dif>0  ){

                if(lbl->align==LABEL_ALIGN_CENTER){
                    last_x=dif/2;
                }
                else{
                    last_x=dif;
                }
            }
         }
         for(uint8_t i=0;i<temp_text.length();i++){
             char c=temp_text[i];
             Glyph* letter=target_font->get_letter(c,FONT_VARIANT_NORMAL);
             if(letter==NULL){
               continue;
            }
            if(line_h==0){
                line_h=letter->h;
            }
            SDL_Rect rect_letter;
            rect_letter.x=letter->x;
            rect_letter.y=letter->y;
            rect_letter.w=letter->w;
            rect_letter.h=letter->h;
            SDL_Rect rect_pos;
            rect_pos.x=last_x;
            rect_pos.y=last_y;
            rect_pos.w=letter->w;
            rect_pos.h=letter->h;
            SDL_RenderCopy(main_render,font_texture,&rect_letter,&rect_pos);
            last_x+=letter->w;
         }
         last_x=0;
         last_y+=line_h;
    }
    lbl->last_text_index=font_sys->save_text(new_text_texture,width_box,height_box);
    rect.w=width_box;
    rect.h=height_box;
    SDL_SetRenderTarget(main_render,initial_target);
    return new_text_texture;

}


//Draw a ValueBar Sprites
bool Render_DrawSystem::draw_Valuebar_Sprites(SDL_Rect bg_rect,SDL_Rect bar_rect,textureManager_system* texture_sys,int8_t z_index,ValueBar* bar,UI_System* ui_sys){
     if(bar==NULL || texture_sys==NULL){
          return false;
     }

     //get the main texture and reference Sprite Rect for Bar Sprites
     SDL_Rect ref_rect;
     SDL_Texture* main_tex= texture_sys->get_texture_fromSprite(ref_rect,bar->bar_sprites[VALUEBAR_STATE_COMPLETE]);
     if(main_tex==NULL){
         return false;
     }
     SDL_Rect extremeBg_left_rect=texture_sys->get_RectSprite(bar->bg_sprites[0]);
     SDL_Rect centerBg_rect=texture_sys->get_RectSprite(bar->bg_sprites[1]);
     uint16_t actual_pos=0;
     uint16_t end_pos=0;
     uint16_t end_bg=0;
     uint16_t target_size=0;
     bool bar_finished=false;
     if(bar->orientation==UI_ORIENTATION_HORIZONTAL){
         actual_pos=bar_rect.x;
         end_pos=bar_rect.x+bar_rect.w;
         target_size=ref_rect.w;
         end_bg=bg_rect.x+bg_rect.w;

     }
     else{
        actual_pos=bar_rect.y;
        end_pos=bar_rect.y+bar_rect.h;
        target_size=ref_rect.h;
        end_bg=bg_rect.y+bg_rect.h;
     }
     if(extremeBg_left_rect.w!=0 && extremeBg_left_rect.h!=0){
         SDL_Rect rect_left;
         SDL_Point point_left;
         rect_left.w=extremeBg_left_rect.w;
         rect_left.h=extremeBg_left_rect.h;
         if(bar->orientation==UI_ORIENTATION_HORIZONTAL){
            rect_left.x=bg_rect.x-rect_left.w;
            rect_left.y=bg_rect.y;

         }
         else{
            rect_left.x=bg_rect.x;
            rect_left.y=bg_rect.y-rect_left.h;
         }
         point_left.x=extremeBg_left_rect.w*0.5;
         point_left.y=extremeBg_left_rect.h*0.5;
         RenderCommand cmd_leftBg;
         cmd_leftBg.angle=0;
         cmd_leftBg.flipped=false;
         cmd_leftBg.tex_id=main_tex;
         cmd_leftBg.rect_sprite=extremeBg_left_rect;
         cmd_leftBg.rect_world=rect_left;
         cmd_leftBg.render_border=RENDER_BORDER_NONE;
         cmd_leftBg.z_index=z_index-1;
         cmd_leftBg.pivot=point_left;
         cmd_leftBg.use_color=false;
         render_queue.push_back(cmd_leftBg);
     }
     SDL_Texture* new_bar_tex=NULL;
     SDL_Rect rect_tex={0,0,bar->max_width,bar->max_height};
     SDL_Rect rect_world={bar_rect.x,bar_rect.y,bar->max_width,bar->max_height};

     Rendered_ValueBars* last_barRendered=ui_sys->get_saved_valueBar_texture(bar->saved_texture);
     SDL_Texture* initial_texture=SDL_GetRenderTarget(main_render);
     bool require_redraw=true;
     bool require_save=false;
     if(last_barRendered!=NULL){
         new_bar_tex=last_barRendered->tex;
     }
     if(new_bar_tex!=NULL && bar->last_value==bar->value){
           actual_pos=end_bg;
           require_redraw=false;
     }
     bar->last_value=bar->value;
     if(new_bar_tex==NULL){
        require_save=true;
        new_bar_tex=SDL_CreateTexture(main_render,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET,bar->max_width,bar->max_height);
        SDL_SetTextureBlendMode(new_bar_tex,SDL_BLENDMODE_BLEND);
     }

     SDL_SetRenderTarget(main_render,new_bar_tex);
     if(require_redraw){
          SDL_SetRenderDrawColor(main_render,0,0,0,0);
          SDL_RenderClear(main_render);
     }
     while(actual_pos<end_bg){
         SDL_Rect target_rect;
         SDL_Rect bg_target_rect;
         uint16_t limit_posSprite=0;
         if(bar->orientation==UI_ORIENTATION_HORIZONTAL){
            limit_posSprite=actual_pos+ref_rect.w;
            target_rect.x=actual_pos-bar_rect.x;
            target_rect.y=0;
            target_rect.w=ref_rect.w;
            target_rect.h=ref_rect.h;
            bg_target_rect.x=actual_pos-bg_rect.x;
            bg_target_rect.y=0;
            bg_target_rect.w=centerBg_rect.w;
            bg_target_rect.h=centerBg_rect.h;
         }
         else{
            limit_posSprite=actual_pos+ref_rect.h;
            target_rect.x=0;
            target_rect.y=actual_pos-bar_rect.y;
            target_rect.w=ref_rect.w;
            target_rect.h=ref_rect.h;
            bg_target_rect.x=0;
            bg_target_rect.y=actual_pos-bg_rect.y;
            bg_target_rect.w=centerBg_rect.w;
            bg_target_rect.h=centerBg_rect.h;
         }
         uint16_t target_sprite=0xFFFF;
         if(centerBg_rect.w!=0 && centerBg_rect.h!=0){
            SDL_RenderCopy(main_render,main_tex,&centerBg_rect,&bg_target_rect);
         }

         if(actual_pos>=end_pos){
            bar_finished=true;
         }
         if(bar_finished==false){

             uint16_t dif_end=limit_posSprite-end_pos;
             if(dif_end>0){
                float out_porcent=(float)dif_end/(float)target_size;
                if(out_porcent>=0.3){
                     target_sprite=bar->bar_sprites[VALUEBAR_STATE_MEDIUM];
                }
                else if(out_porcent>=0.7){
                   target_sprite=bar->bar_sprites[VALUEBAR_STATE_LOW];
                 }
             }
         }

         SDL_Rect rect_sprite=texture_sys->get_RectSprite(target_sprite);
         actual_pos+=target_size;
         if(rect_sprite.w==0 && rect_sprite.h==0){
            continue;
         }

         SDL_RenderCopy(main_render,main_tex,&rect_sprite,&target_rect);
     }

     SDL_SetRenderTarget(main_render,initial_texture);
     SDL_Point pt;
     pt.x=rect_tex.x*0.5;
     pt.y=rect_tex.y*0.5;
     RenderCommand cmd;
     cmd.angle=0;
     cmd.flipped=false;
     cmd.rect_sprite=rect_tex;
     cmd.rect_world=rect_world;
     cmd.use_color=false;
     cmd.z_index=z_index;
     cmd.pivot=pt;
     cmd.tex_id=new_bar_tex;
     render_queue.push_back(cmd);

     if(new_bar_tex!=NULL &&require_save){
          bar->saved_texture=ui_sys->save_valueBar_texture(new_bar_tex,rect_tex,rect_world);

     }
     SDL_Rect extremeBg_right_rect=texture_sys->get_RectSprite(bar->bg_sprites[2]);
     if(extremeBg_right_rect.w!=0 && extremeBg_right_rect.h!=0){
         SDL_Rect rect_right;
         rect_right.w=extremeBg_right_rect.w;
         rect_right.h=extremeBg_right_rect.h;
         if(bar->orientation==UI_ORIENTATION_HORIZONTAL){
            rect_right.x=actual_pos;
            rect_right.y=bg_rect.y;

         }
         else{
            rect_right.x=bg_rect.x;
            rect_right.y=actual_pos;
         }
         SDL_Point point_left;
         point_left.x=extremeBg_right_rect.w*0.5;
         point_left.y=extremeBg_right_rect.h*0.5;
         RenderCommand cmd_righBg;
         cmd_righBg.angle=0;
         cmd_righBg.flipped=false;
         cmd_righBg.tex_id=main_tex;
         cmd_righBg.rect_sprite=extremeBg_right_rect;
         cmd_righBg.rect_world=rect_right;
         cmd_righBg.render_border=RENDER_BORDER_NONE;
         cmd_righBg.z_index=z_index-1;
         cmd_righBg.pivot=point_left;
         cmd_righBg.use_color=false;
         render_queue.push_back(cmd_righBg);
     }
     return true;
}

//Draw a ScrollBar
void Render_DrawSystem::draw_scrollBar(ScrollBar* scroll,textureManager_system* texture_sys,SDL_Rect rect_container,int8_t z_index){
     SDL_Rect rect_scroll;
     SDL_Rect rect_bar;
     SDL_Rect left_btn;
     SDL_Rect right_btn;
     uint16_t sz_btns=scroll->size_buttons;
     left_btn.w=sz_btns;
     left_btn.h=sz_btns;
     right_btn.w=sz_btns;
     right_btn.h=sz_btns;
     rect_scroll.w=scroll->max_width_Bg;
     rect_scroll.h=scroll->max_height_bg;
     rect_bar.w=scroll->width_bar;
     rect_bar.h=scroll->height_bar;
     if(scroll->orientation==UI_ORIENTATION_HORIZONTAL){
         rect_scroll.x=rect_container.x;
         rect_scroll.y=(rect_container.y+rect_container.h)-rect_scroll.h;
         rect_bar.y=rect_scroll.y;
         rect_bar.x=rect_scroll.x+(scroll->desplacement*(rect_scroll.w-rect_bar.w));
         right_btn.x=(rect_scroll.x+rect_scroll.w)-sz_btns;
         right_btn.y=rect_scroll.y;
     }
     else{
         rect_scroll.x=(rect_container.x+rect_container.w)-rect_scroll.w;
         rect_scroll.y=rect_container.y;
         rect_bar.y=rect_scroll.y+(scroll->desplacement*(rect_scroll.h-rect_bar.h));
         rect_bar.x=rect_scroll.x;
         right_btn.x=rect_scroll.x;
         right_btn.y=(rect_scroll.y+rect_scroll.h)-sz_btns;
     }
     left_btn.x=rect_scroll.x;
     left_btn.y=rect_scroll.y;
     RenderCommand cmd_scroll,cmd_bar,cmd_left_btn,cmd_right_btn;
     cmd_scroll.tex_id=NULL;
     cmd_scroll.color=scroll->bg_color;
     cmd_scroll.rect_world=rect_scroll;
     cmd_scroll.z_index=z_index+1;
     cmd_scroll.use_color=true;
     cmd_scroll.render_border=RENDER_BORDER_NONE;
     cmd_bar.tex_id=NULL;
     cmd_bar.color=scroll->bar_colors[scroll->actual_state];
     cmd_bar.rect_world=rect_bar;
     cmd_bar.z_index=z_index+2;
     cmd_bar.use_color=true;
     cmd_bar.render_border=RENDER_BORDER_NONE;

     bool use_color=true;
     SDL_Rect rect_sprite_btns;
     SDL_Texture* tex_btns=texture_sys->get_texture_fromSprite(rect_sprite_btns,scroll->btns_sprite);
     if(tex_btns!=NULL){
        use_color=false;
     }
     cmd_left_btn.tex_id=tex_btns;
     cmd_left_btn.rect_sprite=rect_sprite_btns;
     cmd_left_btn.angle=0;
     cmd_left_btn.flipped=false;
     cmd_left_btn.color=scroll->buttons_color;
     cmd_left_btn.rect_world=left_btn;
     cmd_left_btn.z_index=z_index+3;
     cmd_left_btn.use_color=use_color;
     cmd_left_btn.render_border=RENDER_BORDER_NONE;

     cmd_right_btn.angle=0;
     cmd_right_btn.flipped=false;
     cmd_right_btn.tex_id=tex_btns;
     cmd_right_btn.rect_sprite=rect_sprite_btns;
     cmd_right_btn.color=scroll->buttons_color;
     cmd_right_btn.rect_world=right_btn;
     cmd_right_btn.z_index=z_index+3;
     cmd_right_btn.use_color=use_color;
     cmd_right_btn.render_border=RENDER_BORDER_NONE;
     render_queue.push_back(cmd_scroll);
     render_queue.push_back(cmd_bar);
     render_queue.push_back(cmd_left_btn);
     render_queue.push_back(cmd_right_btn);

}
//Draw the Sprite Composer
void Render_DrawSystem:: draw_spriteComposer(Scene* target_scene,textureManager_system* texture_sys,uint16_t index_entity,int8_t z_index,float centerX,float centerY,float angle,float initial_angle,bool flipped){
  if(target_scene==NULL){
       return;
  }
  SpriteComposer_System* composer_sys=target_scene->get_SpriteComposer_sys();
  if(composer_sys==NULL){
     return;
  }
  SpriteComposer_Data* composer_spr=composer_sys->get_spriteComposer(index_entity);
  if(composer_spr==NULL){
      return;
  }


  float total_angle=angle;
  if(initial_angle!=0){
     total_angle=fmod(angle+(-initial_angle),360.0);
     if(total_angle<0){
         total_angle+=360;
     }
  }
  float rad=Math_G::to_rad(total_angle);
  float sin=Math_G::Sin(Math_G::to_rad(total_angle));
  float cos=Math_G::Cos(Math_G::to_rad(total_angle));
  SDL_Texture* main_tex=NULL;
  for(SpriteComposer_Item sprite:composer_spr->sprites){
      float offsetx=sprite.offsetX;
      float offsety=sprite.offsetY;
      uint16_t index_sprite=sprite.id_sprite;
      SDL_Rect rect_sprite;
      if(main_tex==NULL){
          main_tex=texture_sys->get_texture_fromSprite(rect_sprite,index_sprite);
          if(main_tex==NULL){
             continue;
          }
      }
      else{
          rect_sprite=texture_sys->get_RectSprite(index_sprite);
      }

      SDL_Rect rect_obj;
      float rotatedOffsetX=(offsetx*cos)-(offsety*sin);
      float rotatedOffsetY=(offsetx*sin)+(offsety*cos);
      rect_obj.w=rect_sprite.w;
      rect_obj.h=rect_sprite.h;
      rect_obj.x=(centerX+rotatedOffsetX)-(rect_sprite.w*0.5);
      rect_obj.y=(centerY+rotatedOffsetY)-(rect_sprite.h*0.5);

      SDL_Point point_pivot;
      point_pivot.x=rect_sprite.w*0.5;
      point_pivot.y=rect_sprite.h*0.5;
      uint16_t color=0;
      render_queue.push_back({
        main_tex,
        rect_obj,
        rect_sprite,
        color,
        point_pivot,
        total_angle,
        z_index,
        false,
        flipped,
        RENDER_BORDER_NONE
       });
       std::stable_sort(render_queue.begin(),render_queue.end(),[](const RenderCommand&a, const RenderCommand& b){
            return a.z_index<b.z_index;
       });
  }

}

//Draw a UI with Sprite Composer
void Render_DrawSystem::draw_UiSpriteComposer(Scene* target_scene,textureManager_system* texture_sys,uint16_t index_entity,int8_t z_index,float centerX,float centerY,SDL_Rect& rect_composer){
   if(target_scene==NULL){
       return;
  }
  SpriteComposer_System* composer_sys=target_scene->get_SpriteComposer_sys();
  if(composer_sys==NULL){
     return;
  }
  SpriteComposer_Data* composer_spr=composer_sys->get_spriteComposer(index_entity);
  if(composer_spr==NULL){
      return;
  }
  SDL_Texture* main_tex=NULL;
 for(SpriteComposer_Item sprite:composer_spr->sprites){
      float offsetx=sprite.offsetX;
      float offsety=sprite.offsetY;
      uint16_t index_sprite=sprite.id_sprite;
      SDL_Rect rect_sprite;
      if(main_tex==NULL){
         main_tex=texture_sys->get_texture_fromSprite(rect_sprite,index_sprite);
         if(main_tex==NULL){
             continue;
         }
      }
      else{
           rect_sprite=texture_sys->get_RectSprite(index_sprite);
      }

      SDL_Rect rect_obj;
      rect_obj.w=rect_sprite.w;
      rect_obj.h=rect_sprite.h;
      rect_obj.x=(centerX+offsetx)-(rect_sprite.w*0.5);
      rect_obj.y=(centerY+offsety)-(rect_sprite.h*0.5);
      SDL_Point point_pivot;
      point_pivot.x=rect_sprite.w*0.5;
      point_pivot.y=rect_sprite.h*0.5;
      uint16_t color=0;
      render_queue.push_back({
        main_tex,
        rect_obj,
        rect_sprite,
        color,
        point_pivot,
        0,
        z_index,
        false,
        false,
        RENDER_BORDER_NONE
       });
  }

}

//Draw the Tilemap
void  Render_DrawSystem::draw_tilemap(textureManager_system* texture_sys,System_TileMap* tilemap,Camera2D* cam){
    if(redraw==REDRAW_ENTITIES && bg_texture!=NULL){
        SDL_RenderCopy(main_render,bg_texture,NULL,NULL);
        return;
    }
    SDL_Texture* initial_target=SDL_GetRenderTarget(main_render);
    if(bg_texture!=NULL){
        SDL_SetRenderTarget(main_render,bg_texture);
    }
    SDL_SetRenderDrawColor(main_render,0,0,0,255);
    SDL_RenderClear(main_render);
    float left_cam=cam->getLeft();
    float right_cam=cam->getRight();
    float top_cam=cam->getTop();
    float bottom_cam=cam->getBottom();
    uint32_t tiles_size=tilemap->get_tiles_size();
    int32_t layer_width=static_cast<int32_t>(tilemap->get_width_layer(0));
    int32_t layer_height=static_cast<int32_t>(tilemap->get_height_layer(0));
    uint8_t layers_count=tilemap->get_layers_count();
    uint32_t min_col=std::max(0,static_cast<int32_t>(std::floor(left_cam/tiles_size)));
    uint32_t max_col=std::min(layer_width-1,static_cast<int32_t>(std::floor(right_cam/tiles_size)));
    uint32_t min_row=std::max(0,static_cast<int32_t>(std::floor(bottom_cam/tiles_size)));
    uint32_t max_row=std::min(layer_width-1,static_cast<int32_t>(std::floor(top_cam/tiles_size)));
    if(max_col>=layer_width){
       max_col=layer_width-1;
    }
    if(max_row>=layer_height){
         max_row=layer_height-1;
    }
    for(uint32_t row=min_row;row<=max_row;row++){
       uint32_t row_offset=row*layer_width;
       for(uint32_t column=min_col;column<=max_col;column++){
          float world_x=static_cast<float>(column*tiles_size);
          float world_y=static_cast<float>(row*tiles_size);
          float screen_x=world_x-left_cam;
          float screen_y=world_y-bottom_cam;
          SDL_Rect rect_tile;
          rect_tile.x=screen_x;
          rect_tile.y=screen_y;
          rect_tile.w=tiles_size;
          rect_tile.h=tiles_size;
          for(uint8_t layer_index=0;layer_index<layers_count;layer_index++){
             uint16_t sprite_id=tilemap->get_sprite_tile(layer_index,row_offset+column);
             if(sprite_id==0){
                 continue;
             }
             sprite_id=sprite_id-1;
             SDL_Rect rect_sprite;
             SDL_Texture* tex=texture_sys->get_texture_fromSprite(rect_sprite,sprite_id);
             if(tex!=NULL){
                  SDL_RenderCopy(main_render,tex,&rect_sprite,&rect_tile);
             }
             else{
                  SDL_SetRenderDrawColor(main_render,255,255,255,255);
                 SDL_RenderFillRect(main_render,&rect_tile);
              }

          }
       }
    }
    SDL_SetRenderTarget(main_render,initial_target);
    if(bg_texture!=NULL){
        SDL_RenderCopy(main_render,bg_texture,NULL,NULL);
    }

}



//The Windows has Resize
void Render_DrawSystem::On_WindowResize(uint16_t new_w,uint16_t new_h){

   SDL_DestroyTexture(bg_texture);
   bg_texture=SDL_CreateTexture(main_render,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET,new_w,new_h);
   redraw=REDRAW_ALL;
}


//Get the Windows
SDL_Window* Render_DrawSystem::get_win(){
   return main_win;
}

//Get the SDL Render of the Windows
SDL_Renderer* Render_DrawSystem::get_win_render(){
  return main_render;
}
