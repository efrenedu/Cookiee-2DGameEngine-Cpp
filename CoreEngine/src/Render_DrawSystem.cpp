#include "Render_DrawSystem.h"

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
   uint32_t flags_render=SDL_RENDERER_ACCELERATED;
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
    for(uint16_t i=0;i<num_enities;i++){
          Transform_Data* data_transform=transf_sys->get_transform(i);
          float x=data_transform->x;
          float y=data_transform->y*-1;
          uint16_t angle=data_transform->angle;
          bool flipped=data_transform->flipped;
          uint16_t scale=data_transform->scale;
          uint16_t width=32;
          uint16_t height=32;
          float screen_x=x-left_cam;
          float screen_y=y-bottom_cam;
          SDL_Rect rect_obj;
          uint32_t mask_entity=target_scene->get_mask(i);
          bool is_collider=false;
          bool is_rendered=false;
          if(HasComponentBit(mask_entity,COLLIDER_COMP)){
             is_collider=true;
          }
          if(HasComponentBit(mask_entity,RENDER_COMP)){
             is_rendered=true;

          }
          if(is_rendered==false &&(is_collider==true && draw_colliders_borders==true)==false){
             continue;
          }
          if(is_collider==true && draw_colliders_borders==true){
             SDL_Rect collider_rect;
             collider_rect.x=screen_x;
             collider_rect.y=screen_y;
             Collider_System* collider_sys=target_scene->get_collider_sys();
             Collider_Data* collider=collider_sys->get_collider(i);
             SDL_SetRenderDrawColor(main_render,255,0,0,255);
             if(collider->collider_type==FIGURE_TYPE::FIGURE_CIRCLE){
                collider_rect.w=collider->shape.circle.radius*2;
                collider_rect.h=collider->shape.circle.radius*2;

                SDL_Point pt;
                render_queue.push_back({
                     NULL,
                     collider_rect,
                     rect_obj,
                     pt,
                     0,
                     100,
                     false,true
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
                     pt,
                     0,
                     100,
                     false,true
                 });
                 std::stable_sort(render_queue.begin(),render_queue.end(),[](const RenderCommand&a, const RenderCommand& b){
                        return a.z_index<b.z_index;
                });

             }
          }
          if(is_rendered==false){
            continue;
          }
          Render_data* render_dat=render_sys->get_render(i);
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
          }
          rect_obj.w=width;
          rect_obj.h=height;
          bool on_Camera=(x+width>=left_cam)&&(x<=right_cam)&&(y+height>=bottom_cam) &&(y<=top_cam);
          if(on_Camera==false){
              continue;
          }
          SDL_Point point_pivot;
          point_pivot.x=rect_sprite.w*0.5;
          point_pivot.y=rect_sprite.h*0.5;
          render_queue.push_back({
              tex,
              rect_obj,
              rect_sprite,
              point_pivot,
              angle,
              render_dat->z_index,
              flipped,
              false
          });
          std::stable_sort(render_queue.begin(),render_queue.end(),[](const RenderCommand&a, const RenderCommand& b){
                 return a.z_index<b.z_index;
          });

    }
    for (const RenderCommand& cmd:render_queue){

        SDL_Texture* tex=cmd.tex_id;
        SDL_Rect rect_world=cmd.rect_world;
        bool render_colliderborder=cmd.render_ColliderborderOnly;
        if(tex==NULL){
            SDL_SetRenderDrawColor(main_render,255,255,255,255);
            if(render_colliderborder){
                SDL_RenderDrawRect(main_render,&rect_world);
            }
            else{
                 SDL_RenderFillRect(main_render,&rect_world);
            }
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

}

//Draw UI
void Render_DrawSystem::draw_UI(textureManager_system* texture_sys,Scene* target_scene,uint16_t ui_id){

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
    uint8_t layers_count=tilemap->get_layers_count();
    uint32_t min_col=std::max(0,static_cast<int32_t>(std::floor(left_cam/tiles_size)));
    uint32_t max_col=std::min(layer_width-1,static_cast<int32_t>(std::floor(right_cam/tiles_size)));
    uint32_t min_row=std::max(0,static_cast<int32_t>(std::floor(bottom_cam/tiles_size)));
    uint32_t max_row=std::min(layer_width-1,static_cast<int32_t>(std::floor(top_cam/tiles_size)));

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
