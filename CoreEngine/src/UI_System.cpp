#include "UI_System.h"
#include<iostream>
#include<math_structs.h>
#include<UI_event.h>
#include<Scene.h>
#include<EngineContext.h>



//Reserve Memory for the System
void UI_System::Reserve_Memory(uint16_t ui_components_count,uint16_t entities_count,uint16_t fillval){
  UIEntity_Data ui_dat;
  ui_dat.index_button=fillval;
  ui_dat.index_checkButton=fillval;
  ui_dat.index_comboBox=fillval;
  ui_dat.index_label=fillval;
  ui_dat.index_panel=fillval;
  ui_dat.index_scrollbar=fillval;
  ui_dat.index_valueBar=fillval;
  ui_dat.UI_mask=0;
  Entity_toDense.assign(entities_count,ui_dat);

}

//Free System Memory
void UI_System::Free_Memory(){
  Entity_toDense.clear();
  buttons_list.clear();
  labels_list.clear();
  radiobtns_list.clear();
  checkbtns_list.clear();
  comboBoxes_list.clear();
  panels_list.clear();
  valueBars_list.clear();
  scrollbars_list.clear();
  list_text_writters.clear();
  listBox_list.clear();
  list_sliders.clear();
  list_icons.clear();
  for(Rendered_ValueBars target:saved_bars_textures){
     if(target.tex!=NULL){
          SDL_DestroyTexture(target.tex);
     }
  }
  saved_bars_textures.clear();
}


//Update the Sprite Data of Widgets dependents of Sprite Data of Render Sprite and SpriteComposer Sprites
void UI_System::update_Widgets_Sprites( Scene* target_scene){
   if(target_scene==NULL ){
       return;
   }
   SpriteComposer_System* composer_sys=target_scene->get_SpriteComposer_sys();
   Render_System* render_sys=target_scene->get_render_sys();
   for(uint16_t i=0;i<target_scene->get_count_entities();i++){
      bitset<64> mask=target_scene->get_mask(i);
      if(HasComponentBit(mask,UI_COMP)==false){
          continue;
      }
      UI_header header=Entity_toDense[i].header;
      if(header.active_state==UI_ACTIVE_STATE_INACTIVE){
         continue;
      }
      Render_data* render_dat=render_sys->get_render(i);
      if(render_dat==NULL){
          continue;
      }
      switch(header.main_type){
         case UI_BUTTON:{
            Button* btn=get_button(i);
            SpriteComposer_Data* composer=NULL;
            if(HasComponentBit(mask,SPRITECOMPOSER_COMP)){
                 composer=composer_sys->get_spriteComposer(i);
            }
            if(btn!=NULL){
                uint16_t targer_sprite=btn->sprites_states[btn->actual_state];
                bool need_redraw=false;
                if(targer_sprite!=render_dat->sprite_id){
                    need_redraw=true;
                }
                render_dat->sprite_id=targer_sprite;
                if(composer!=NULL){
                    if(composer->sprites.size()==3){
                       //Only Button with 3 Sprites Composer
                       composer->sprites[0].id_sprite=targer_sprite-1;
                       composer->sprites[1].id_sprite=targer_sprite;
                       composer->sprites[2].id_sprite=targer_sprite+1;
                    }
                 }
                 if(need_redraw){
                   EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
                 }
             }
          }
          case UI_ICON:{
             UI_Icon* icon=get_Icon(i);
             if(icon!=NULL){
                uint16_t target_sprite=icon->sprites_states[icon->actual_state];
                if(target_sprite!=render_dat->sprite_id){
                    EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
                 }
                 render_dat->sprite_id=target_sprite;

             }
             break;
          }
          case UI_CHECKBUTTON:{
             CheckButton* check=get_checkBtn(i);
             if(check!=NULL){
                uint16_t target_sprite=check->sprites_states[check->state];
                if(target_sprite!=render_dat->sprite_id){
                   EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
                }
                render_dat->sprite_id=target_sprite;
             }
              break;
          }
          case UI_RADIOBUTTON:{
             Radio_Button* btn=get_radioBtn(i);
             if(btn!=NULL){
                uint16_t target_sprite=btn->sprites_states[btn->state];
                if(target_sprite!=render_dat->sprite_id){
                   EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
                }
                render_dat->sprite_id=target_sprite;
             }
             break;
          }
          default:break;
      }
   }


}

//Update the System
void UI_System::update(float dt){
   update_labels(dt);
   for(uint16_t i=0;i<Entity_toDense.size();i++){
       if(Entity_toDense[i].header.active_state==UI_ACTIVE_STATE_INACTIVE && Entity_toDense[i].header.main_type!=UI_PANEL){
           continue;
       }
       switch(Entity_toDense[i].header.main_type){
          case UI_BUTTON:{
             Button* btn=get_button(i);
             Label* lbl=get_label(i);
             if(btn!=NULL){
                 update_buttons(Entity_toDense[i].header,btn,lbl,dt);
             }
             break;
          }
          case UI_LIST:{
              List_Box* l_box=get_list_box(i);
              Label* lbl=get_label(i);
              if(l_box!=NULL){
                 update_listBoxs(Entity_toDense[i].header,l_box,lbl,dt);
              }
              break;
          }
          case UI_COMBOBOX:{
              ComboBox* c_box=get_comboBox(i);
              Label* lbl=get_label(i);
              if(c_box!=NULL){
                 update_ComboBoxes(Entity_toDense[i].header,c_box,lbl,dt);
              }
              break;
          }
          case UI_RADIOBUTTON:{
              break;
          }
          case UI_CHECKBUTTON:{
              break;
          }
          case UI_PANEL:{

              ScrollBar* scroll=get_scrollbar(i);
              Panel* pnl=get_panel(i);
              if(pnl!=NULL){
                update_panels(&Entity_toDense[i].header,pnl,dt);

                if(scroll!=NULL && Entity_toDense[i].header.active_state!=UI_ACTIVE_STATE_INACTIVE){
                    update_ScrollBars(Entity_toDense[i].header,scroll,dt);
                    if(scroll->layout.orientation==UI_ORIENTATION_HORIZONTAL){
                          pnl->offset_scrollX=scroll->desplacement*scroll->layout.width_area;
                    }
                    else{
                        pnl->offset_scrollY=scroll->desplacement*scroll->layout.height_area;
                    }
                }
              }

              break;
          }
          case UI_SLIDER:{
              Slider* slider=get_slider(i);
              if(slider!=NULL){
                 update_sliders(Entity_toDense[i].header,slider,dt);
              }
              break;
          }
          default:break;

       }

   }
}

//Methods for Update Dinamic UI Components
void UI_System:: update_buttons(UI_header header,Button* btn,Label* lbl,float dt){
   if(btn==NULL){
      return;
   }
   if(header.active_state==UI_ACTIVE_STATE_DISABLED){
       if(btn->actual_state!=UI_STATE_DISABLED){
         btn->actual_state=UI_STATE_DISABLED;
         EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
       }
       return;
   }
   bool trigger_ev=false;
   uint8_t input_drive=EngineContext::get().get_input().get_input_drive();
   float remanent_time=0.0f;
   bool can_change_lbl_text=false;
   bool cooldown_available=(btn->data_event.show_cooldown_time==true && lbl!=NULL)==true;

   for(uint8_t i=0;i<btn->data_event.count_timers.size();i++){
      float val=btn->data_event.count_timers[i];
      if(val==0.0f){
         continue;
      }
      val+=dt;
      if(val>=btn->data_event.wait_timers[i]){
          val=0.0f;
          if(i==BUTTON_TIMER_TRIGGER_EV){
              trigger_ev=true;
          }
          else if(i==BUTTON_TIMER_COOLDOWN){
              can_change_lbl_text=true;
              remanent_time=0.0f;
          }
      }
      if(cooldown_available){
        if(i==BUTTON_TIMER_COOLDOWN && val>0.0f){
            remanent_time=btn->data_event.wait_timers[i]-val;
        }
      }
      btn->data_event.count_timers[i]=val;
   }

   if(cooldown_available && btn->data_event.count_timers[BUTTON_TIMER_SHOWTEXT_COOLDOWN]==0.0f && remanent_time>0.0f){
      btn->data_event.count_timers[BUTTON_TIMER_SHOWTEXT_COOLDOWN]=dt;
      can_change_lbl_text=true;
   }
   if((input_drive & (1U<<INPUT_DRIVE_MOUSE))!=0 && btn->data_event.use_keyInput==false){
      Input_System inpt=EngineContext::get().get_input();
      std::vector<uint16_t> mouse_pos=inpt.get_mousePosition();
      uint8_t target_state=UI_STATE_NORMAL;
      uint8_t mouse_st=inpt.get_mouseState(LEFT_BUTTON);
      if(btn->data_event.recent_interact==true && mouse_st==INPUT_NONE){
          btn->data_event.recent_interact=false;
          btn->data_event.count_timers[BUTTON_TIMER_INTERACT]=dt;
      }
      if(mouse_inCollisionWidget(mouse_pos[0],mouse_pos[1],header.centerX,header.centerY,header.half_width,header.half_height)==true){
         target_state=UI_STATE_HOVER;
         if(mouse_st==INPUT_DOWN && btn->data_event.count_timers[BUTTON_TIMER_COOLDOWN]==0.0 &&  btn->data_event.count_timers[BUTTON_TIMER_TRIGGER_EV]==0.0){
              target_state=UI_STATE_INTERACT;
              btn->data_event.count_timers[BUTTON_TIMER_TRIGGER_EV]=dt;
              btn->data_event.recent_interact=true;
              if(btn->data_event.wait_timers[BUTTON_TIMER_COOLDOWN]>0.0f){
                  btn->data_event.count_timers[BUTTON_TIMER_COOLDOWN]=dt;
              }
              if(cooldown_available==true){
                  btn->data_event.initial_labelText=lbl->state.text_label;
                  lbl->state.text_label=to_string(btn->data_event.wait_timers[BUTTON_TIMER_COOLDOWN]);
                  lbl->state.has_modify=true;
              }
         }
      }
      if(btn->data_event.recent_interact || btn->data_event.count_timers[BUTTON_TIMER_INTERACT]!=0.0){
          target_state=UI_STATE_INTERACT;
      }
      else if(btn->data_event.count_timers[BUTTON_TIMER_COOLDOWN]!=0.0){
          target_state=UI_STATE_DISABLED;
      }

      if(btn->actual_state!=target_state){
          btn->actual_state=target_state;
          if(lbl!=NULL){
               lbl->visual.text_color=btn->colors_labels[btn->actual_state];
               lbl->state.has_modify=true;
          }
          EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      }
      if(can_change_lbl_text && cooldown_available==true){
          if(remanent_time>0.0){
              lbl->state.text_label=to_string(remanent_time);
          }
          else{
             lbl->state.text_label=btn->data_event.initial_labelText;
          }
          lbl->state.has_modify=true;
          EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      }
   }

   if((input_drive & (1U<<INPUT_DRIVE_KEYBOARD))!=0 && btn->data_event.use_keyInput==true){

       if(btn->data_event.selected==false && btn->data_event.always_selected==false){
         return;
       }
       Input_System inpt=EngineContext::get().get_input();
       uint8_t target_state=UI_STATE_NORMAL;
       uint8_t trigger_key_st=inpt.get_keyState(btn->data_event.trigger_key);
       if(btn->data_event.recent_interact==true && trigger_key_st==INPUT_NONE){
          btn->data_event.recent_interact=false;
          btn->data_event.count_timers[BUTTON_TIMER_INTERACT]=dt;
      }
       if(trigger_key_st==INPUT_DOWN && btn->data_event.count_timers[BUTTON_TIMER_COOLDOWN]==0.0f &&  btn->data_event.count_timers[BUTTON_TIMER_TRIGGER_EV]==0.0f){
            target_state=UI_STATE_INTERACT;
            btn->data_event.count_timers[BUTTON_TIMER_TRIGGER_EV]=dt;
            btn->data_event.recent_interact=true;
            if(btn->data_event.wait_timers[BUTTON_TIMER_COOLDOWN]>0.0f){
               btn->data_event.count_timers[BUTTON_TIMER_COOLDOWN]=dt;
            }
            if(cooldown_available==true){
               btn->data_event.initial_labelText=lbl->state.text_label;
               lbl->state.text_label=to_string(btn->data_event.wait_timers[BUTTON_TIMER_COOLDOWN]);
               lbl->state.has_modify=true;
            }
       }
       if(btn->data_event.recent_interact || btn->data_event.count_timers[BUTTON_TIMER_INTERACT]!=0.0){
          target_state=UI_STATE_INTERACT;
      }
      else if(btn->data_event.count_timers[BUTTON_TIMER_COOLDOWN]!=0.0){
          target_state=UI_STATE_DISABLED;
      }
       if(target_state!=btn->actual_state){
           btn->actual_state=target_state;
           if(lbl!=NULL){
              lbl->visual.text_color=btn->colors_labels[btn->actual_state];
               lbl->state.has_modify=true;
           }
           EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
       }
        if(can_change_lbl_text && cooldown_available==true){
          if(remanent_time>0.0){
              lbl->state.text_label=to_string(remanent_time);
          }
          else{
             lbl->state.text_label=btn->data_event.initial_labelText;
          }
          lbl->state.has_modify=true;
          EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      }
   }
   if(trigger_ev){
      EventPayload payload;
      payload.value_data=btn->data_event.arg_func;
      payload.sender_type=UI_EVENT_SENDER_BUTTON;
      EngineContext::get().get_uiEvent().trigget_event(btn->data_event.callfunction,payload);
   }


}
void UI_System:: update_listBoxs(UI_header header,List_Box* l_box,Label* lbl,float dt){
   if(l_box==NULL){
      return;
   }
   uint8_t input_drive=EngineContext::get().get_input().get_input_drive();
   Input_System inpt=EngineContext::get().get_input();
   bool trigger_ev=false;
   bool trigger_hover=false;
   uint8_t select_item=l_box->data.selected_item;

   if((input_drive & (1U<<INPUT_DRIVE_MOUSE))!=0 && l_box->data_event.useKey_Inputs==false){
       std::vector<uint16_t> mouse_pos=inpt.get_mousePosition();
       if(mouse_inCollisionWidget(mouse_pos[0],mouse_pos[1],header.centerX,header.centerY,header.half_width,header.half_height)==true){
            uint16_t temp_y=mouse_pos[1]-(header.centerY-header.half_height);
            float porcent=(float)temp_y/(float)l_box->layout.heigth_items;
            int8_t next_index=(porcent*l_box->layout.max_item_show);
            if(next_index>=l_box->layout.max_item_show){
                next_index=l_box->layout.max_item_show-1;
            }
            select_item=next_index;
            if(inpt.get_mouseState(LEFT_BUTTON)==INPUT_DOWN){
               trigger_ev=true;
            }
       }
       if(l_box->data.selected_item!=select_item){
            trigger_hover=true;
            l_box->data.selected_item=select_item;
            EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
        }
   }
   else if((input_drive & (1U<<INPUT_DRIVE_KEYBOARD))!=0 && l_box->data_event.useKey_Inputs==true){
        if(l_box->data_event.isSelected==false){
           return;
        }
        uint8_t state_up=inpt.get_keyState(l_box->data_event.up_key);
        uint8_t state_down=inpt.get_keyState(l_box->data_event.down_key);
        uint8_t state_trigger=inpt.get_keyState(l_box->data_event.trigger_key);
        if(state_down==INPUT_DOWN){
            if( select_item+1<l_box->data.items.size()){
                 select_item+=1;
            }
        }
        else if(state_up==INPUT_DOWN){
            if(select_item-1>=0){
                 select_item-=1;
            }
        }
        if(state_trigger==INPUT_DOWN){
            trigger_ev=true;
        }
        if(l_box->data.selected_item!=select_item){
            trigger_hover=true;
            l_box->data.selected_item=select_item;
            EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
        }
   }
   if(trigger_ev==true || trigger_hover==true ){
      EventPayload payload;
      std::string call_function=(trigger_ev)?l_box->data_event.callfunction:l_box->data_event.callHoverFunction;
      payload.sender_type=UI_EVENT_SENDER_LISTBOX;
      payload.value_data=l_box->data_event.args_EvItems[l_box->data.selected_item];
      EngineContext::get().get_uiEvent().trigget_event(call_function,payload);
   }

   if(select_item>=l_box->layout.max_item_show){
      l_box->layout.offset=select_item-(l_box->layout.max_item_show-1);
   }
   else{
      l_box->layout.offset=0;
   }
   if(lbl!=NULL){
        //update label of ListBox
        uint8_t items_showed=0;
        std::string text_show="";
        uint8_t limit=l_box->layout.max_item_show;
        for(uint8_t i=l_box->layout.offset;i<l_box->data.items.size();i++){
            text_show+=l_box->data.items[i]+'\n';
            items_showed++;
            if(items_showed>=limit){
                i=l_box->data.items.size();
            }
        }
        if(text_show!=lbl->state.text_label){
            lbl->state.text_label=text_show;
            lbl->state.has_modify=true;
        }
    }
}
void UI_System:: update_ComboBoxes(UI_header header,ComboBox* c_box,Label* lbl,float dt){
   if(c_box==NULL){
      return;
   }

   uint8_t input_drive=EngineContext::get().get_input().get_input_drive();
   bool has_modify=false;
   bool trigger_ev=false;
   if((input_drive & (1U<<INPUT_DRIVE_KEYBOARD))!=0){
      int16_t cx=header.centerX;
      int16_t cy=header.centerY;
      uint16_t half_width=c_box->layout.width_box*0.5;
      uint16_t half_heigth=c_box->layout.min_height*0.5;
      int16_t upper_y=cy-half_heigth;
      if(c_box->layout.open){
          cy+=(c_box->layout.max_height*0.5)-(c_box->layout.min_height*0.5);
          half_heigth=c_box->layout.max_height*0.5;
      }
      std::vector<uint16_t> mouse_pos=EngineContext::get().get_input().get_mousePosition();
      bool click_mouse=EngineContext::get().get_input().get_mouseState(LEFT_BUTTON)==INPUT_DOWN;
      if(mouse_inCollisionWidget(mouse_pos[0],mouse_pos[1],cx,cy,half_width,half_heigth)==true){
         if(c_box->layout.open){
            uint16_t temp_y=mouse_pos[1]-upper_y;
            float porcent=(float)temp_y/(float)c_box->layout.max_height;
            int8_t next_index=(porcent*c_box->layout.max_showItems);
            if(next_index>=c_box->layout.max_showItems){
                next_index=c_box->layout.max_showItems-1;
            }
            c_box->data.selected_item=next_index;
         }
         if(click_mouse){
            if(c_box->layout.open){
               trigger_ev=true;
            }
            c_box->layout.open=!c_box->layout.open;
            has_modify=true;
         }
      }
      else{
         if(click_mouse && c_box->layout.open){
               c_box->layout.open=false;
               has_modify=true;
         }
      }
   }
   if((input_drive & (1U<<INPUT_DRIVE_MOUSE))!=0){
        if(c_box->data_event.useKeys_Input==false || c_box->data_event.selected==false){
            return;
        }

        uint8_t trigger_state=EngineContext::get().get_input().get_keyState(c_box->data_event.trigget_key);
        if(c_box->layout.open){
           uint8_t down_key_st=EngineContext::get().get_input().get_keyState(c_box->data_event.Down_key);
           uint8_t up_key_st=EngineContext::get().get_input().get_keyState(c_box->data_event.Up_key);
           int8_t next_index=c_box->data.selected_item;

           if( down_key_st==INPUT_DOWN){
              if(next_index+1< c_box->data.items.size()){
                 next_index+=1;
              }
           }
           if( up_key_st==INPUT_DOWN ){
              if(next_index-1>=0){
                 next_index-=1;
              }
           }
           if(next_index!=c_box->data.selected_item){
              has_modify=true;
              c_box->data.selected_item=next_index;
           }
        }
        if(trigger_state==INPUT_DOWN){
            if(c_box->layout.open){
               trigger_ev=true;
            }
            c_box->layout.open=!c_box->layout.open;
            has_modify=true;
        }

   }
   if(has_modify){
      EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      if(trigger_ev){
         EventPayload payload;
         payload.value_data=c_box->data_event.items_evs[c_box->data.selected_item];
         payload.sender_type=UI_EVENT_SENDER_COMBOBOX;
         payload.text_data=c_box->data.items[c_box->data.selected_item];
         EngineContext::get().get_uiEvent().trigget_event(c_box->data_event.callfunction,payload);
      }
   }

   //Others offset of Combobox
   if(c_box->data.selected_item>=c_box->layout.max_showItems){
       c_box->layout.offset_items=c_box->data.selected_item-(c_box->layout.max_showItems-1);
   }
    else{
        c_box->layout.offset_items=0;
   }
   if(lbl!=NULL){
        //update label of Combobox
        if(c_box->layout.open==false){
            lbl->state.text_label=c_box->data.items[c_box->data.selected_item];
            lbl->state.has_modify=true;
            return;
        }
        uint8_t items_showed=0;
        std::string text_show="";
        uint8_t limit=(c_box->layout.open==false)?1:c_box->layout.max_showItems;
        for(uint8_t i=c_box->layout.offset_items;i<c_box->data.items.size();i++){
            text_show+=c_box->data.items[i]+'\n';
            items_showed++;
            if(items_showed>=limit){
                i=c_box->data.items.size();
            }
        }
        if(text_show!=lbl->state.text_label){
            lbl->state.text_label=text_show;
            lbl->state.has_modify=true;
        }
    }
}
void UI_System:: update_sliders(UI_header header,Slider* slider,float dt){
   if(slider==NULL){
      return;
   }
   if(header.active_state==UI_ACTIVE_STATE_DISABLED){
      if(slider->visual.actual_state!=UI_STATE_DISABLED){
        slider->visual.actual_state=UI_STATE_DISABLED;
        EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      }
      return;
   }
   uint8_t input_drive=EngineContext::get().get_input().get_input_drive();
   float desplz=slider->state.value;
   if((input_drive & (1U<<INPUT_DRIVE_MOUSE))!=0 && slider->event_data.useKeys==false){
     uint8_t target_state=UI_STATE_NORMAL;
     int16_t cx=header.centerX;
     int16_t cy=header.centerY;
     uint16_t min_sz=std::min(slider->layout.max_h,slider->layout.max_w);
     uint16_t halfw=min_sz*0.5;
     uint16_t half_h=halfw;
     if(slider->layout.orientation==UI_ORIENTATION_HORIZONTAL){
         cx=(header.centerX-header.half_width)+(slider->state.value*slider->layout.max_w)-halfw;
     }
     else{
        cy=(header.centerY+header.half_height)-(slider->state.value*slider->layout.max_h)-half_h;

     }
     float step=slider->event_data.step_val;
     std::vector<uint16_t> mouse_pos=EngineContext::get().get_input().get_mousePosition();
     uint8_t mouse_st=EngineContext::get().get_input().get_mouseState(LEFT_BUTTON);
     if(mouse_st==INPUT_NONE || mouse_st==INPUT_UP){
         slider->event_data.dragging=false;
     }
     if(mouse_inCollisionWidget(mouse_pos[0],mouse_pos[1],cx,cy,halfw,half_h)==true || slider->event_data.dragging==true){
          target_state=UI_STATE_HOVER;
          std::vector<int16_t> mouse_move=EngineContext::get().get_input().get_mouseMove();
          if(mouse_st==INPUT_DOWN || mouse_st==INPUT_PRESSED){
               slider->event_data.dragging=true;
               target_state=UI_STATE_INTERACT;
               if(slider->layout.orientation==UI_ORIENTATION_HORIZONTAL){
                   desplz+=(mouse_move[0]>0)?step*dt:-step*dt;
               }
               else{
                   desplz-=(mouse_move[1]>0)?step*dt:-step*dt;
               }
          }
     }
     if(target_state!=slider->visual.actual_state){
        slider->visual.actual_state=target_state;
        EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      }
   }
   else if((input_drive & (1U<<INPUT_DRIVE_KEYBOARD))!=0 ){
        if(slider->event_data.useKeys==false || slider->event_data.selected==false){
            return;
        }
        uint8_t target_state=UI_STATE_NORMAL;
        float step=slider->event_data.step_val;
        uint8_t state_upValue_Key=EngineContext::get().get_input().get_keyState(slider->event_data.key_upValue);
        uint8_t state_DownValue_Key=EngineContext::get().get_input().get_keyState(slider->event_data.key_downValue);
        if(state_upValue_Key==INPUT_DOWN || state_upValue_Key==INPUT_PRESSED){
             desplz+=step*dt;
             target_state=UI_STATE_INTERACT;
        }
        else if(state_DownValue_Key==INPUT_DOWN || state_DownValue_Key==INPUT_PRESSED){
             desplz-=step*dt;
             target_state=UI_STATE_INTERACT;
        }
        if(target_state!=slider->visual.actual_state){
             slider->visual.actual_state=target_state;
             EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
        }

   }
   if(desplz!=slider->state.value){
      desplz=Math_G::Clamp(desplz,0.0,1.0);
      slider->state.value=desplz;
      EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      EventPayload payload;
      payload.entity_target=0xFFFF;
      payload.sender_type=UI_EVENT_SENDER_SLIDER;
      payload.value_data=desplz;
      EngineContext::get().get_uiEvent().trigget_event(slider->event_data.callFunction,payload);
   }
}
void UI_System:: update_ScrollBars(UI_header header,ScrollBar* scroll,float dt){
   if(scroll==NULL){
      return;
   }
   if(header.active_state==UI_ACTIVE_STATE_DISABLED){
       scroll->visual.actual_state=UI_STATE_DISABLED;
       EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
       return;
   }
   uint8_t input_drive=EngineContext::get().get_input().get_input_drive();
   float desplz=scroll->desplacement;
   if((input_drive & (1U<<INPUT_DRIVE_MOUSE))!=0 && scroll->data_event.use_keysInput==false){

       int16_t c_barX=header.centerX;
       int16_t c_barY=header.centerY;
       uint16_t halfw_bar=scroll->layout.width_bar*0.5;
       uint16_t halfh_bar=scroll->layout.height_bar*0.5;
       if(scroll->layout.orientation==UI_ORIENTATION_HORIZONTAL){
           c_barX=((header.centerX-header.half_width)+scroll->layout.size_buttons)+(scroll->desplacement*(scroll->layout.max_width_Bg-scroll->layout.width_bar));
           c_barX+=halfw_bar;
           c_barY+=header.half_height-halfh_bar;
       }
       else{
           c_barY=((header.centerY-header.half_height)+scroll->layout.size_buttons)+(scroll->desplacement*(scroll->layout.max_height_bg-scroll->layout.height_bar));
           c_barY+=halfh_bar;
           c_barX+=header.half_width-halfw_bar;
       }
       std::vector<uint16_t> mouse_pos=EngineContext::get().get_input().get_mousePosition();
       uint8_t next_state=UI_STATE_NORMAL;
       uint8_t st_mouse=EngineContext::get().get_input().get_mouseState(LEFT_BUTTON);
       if(st_mouse==INPUT_NONE || st_mouse==INPUT_UP){
          scroll->data_event.dragging=false;
       }
       if(mouse_inCollisionWidget(mouse_pos[0],mouse_pos[1],c_barX,c_barY,halfw_bar,halfh_bar)==true || scroll->data_event.dragging==true){
           next_state=UI_STATE_HOVER;
            if(st_mouse==INPUT_PRESSED || st_mouse==INPUT_DOWN){
               next_state=UI_STATE_INTERACT;
               scroll->data_event.dragging=true;
           }
           if(next_state==UI_STATE_INTERACT){
               std::vector<int16_t> mouse_move=EngineContext::get().get_input().get_mouseMove();
               if(scroll->layout.orientation==UI_ORIENTATION_HORIZONTAL){
                  desplz+=(mouse_move[0]>0)?scroll->data_event.step*dt:-scroll->data_event.step*dt;
               }
               else{
                   desplz+=(mouse_move[1]>0)?scroll->data_event.step*dt:-scroll->data_event.step*dt;
               }
           }

       }
       if(next_state!=scroll->visual.actual_state){
           scroll->visual.actual_state=next_state;
           EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);

       }

   }
   else if((input_drive & (1U<<INPUT_DRIVE_KEYBOARD))!=0){
       if(scroll->data_event.use_keysInput==false || scroll->data_event.is_selected==false){
              return;
        }
        float step=scroll->data_event.step;
        uint8_t state_upValue_Key=EngineContext::get().get_input().get_keyState(scroll->data_event.upValue_key);
        uint8_t state_DownValue_Key=EngineContext::get().get_input().get_keyState(scroll->data_event.downValue_key);
        uint8_t target_state=UI_STATE_NORMAL;
        if(scroll->layout.orientation==UI_ORIENTATION_VERTICAL){
              step*=-1;
        }
        if(state_upValue_Key==INPUT_DOWN || state_upValue_Key==INPUT_PRESSED){
             desplz+=step*dt;
             target_state=UI_STATE_INTERACT;
        }
        else if(state_DownValue_Key==INPUT_DOWN || state_DownValue_Key==INPUT_PRESSED){
             desplz-=step*dt;
             target_state=UI_STATE_INTERACT;
        }
        scroll->visual.actual_state=target_state;
   }
   if(desplz!=scroll->desplacement){
      desplz=Math_G::Clamp(desplz,0.0,1.0);
      EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      scroll->desplacement=desplz;
   }
}

//Update Panels Events and States
void UI_System::update_panels(UI_header* header,Panel* pnl,float dt){
   if(header->active_state==UI_ACTIVE_STATE_DISABLED){
       return;
   }
   if(pnl->data_event.useKey_Inputs){
       uint8_t key_open=EngineContext::get().get_input().get_keyState(pnl->data_event.open_key);
       uint8_t key_close=EngineContext::get().get_input().get_keyState(pnl->data_event.close_key);
       if(key_open==INPUT_DOWN){
           pnl->data_event.recent_open=true;
           header->active_state=UI_ACTIVE_STATE_ACTIVE;
       }
       else if(key_close==INPUT_DOWN){
           header->active_state=UI_ACTIVE_STATE_INACTIVE;
           pnl->data_event.recent_close=true;
       }
   }
   if(pnl->data_event.recent_open==true || pnl->data_event.recent_close==true){
       pnl->data_event.recent_open=false;
       pnl->data_event.recent_close=false;
       std::string call_func=(pnl->data_event.recent_open==true)?pnl->data_event.callfunction:pnl->data_event.closeCallFunction;
       EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
       EventPayload payload;
       payload.value_data=pnl->data_event.arg_function;
       payload.sender_type=UI_EVENT_SENDER_PANEL;
       EngineContext::get().get_uiEvent().trigget_event(call_func,payload);
   }

}
//update labels
void UI_System::update_labels(float dt){
   int8_t remove_writer=-1;
   for(uint8_t i=0;i<list_text_writters.size();i++){
       Text_Writter_Animation temp_writer=list_text_writters[i];
       if(temp_writer.is_complete && remove_writer==-1){
           remove_writer=i;
           continue;
       }
       float actual_time=temp_writer.timer_effect;
       actual_time+=dt;
       if(actual_time>=temp_writer.speed){
           actual_time=0.0;
           uint16_t total_chars=temp_writer.full_text.length();
           temp_writer.visible_chars+=1;
           if(temp_writer.visible_chars>=total_chars){
              temp_writer.is_complete=true;
           }
           std::string next_text=temp_writer.full_text.substr(0,temp_writer.visible_chars);
           uint16_t index=Entity_toDense[temp_writer.label_id].index_label;
           labels_list[index].state.text_label=next_text;
           labels_list[index].state.has_modify=true;
       }
       temp_writer.timer_effect=actual_time;
       list_text_writters[i]=temp_writer;
   }
   if(remove_writer!=-1){
       list_text_writters[remove_writer]=list_text_writters.back();
       list_text_writters.pop_back();
   }

}

//Set a Text On the Label, Optionally with Text Writter Effect
void UI_System::set_label_text(std::string text,uint16_t id_label,bool use_typeWritter,float speed){
   if(id_label>=Entity_toDense.size()){
       return;
   }
   if((Entity_toDense[id_label].UI_mask & (1U<<UI_LABEL))==0){
        return;
   }
   if(use_typeWritter){

     Text_Writter_Animation text_writter;
     text_writter.label_id=id_label;
     text_writter.full_text=text;
     text_writter.is_complete=false;
     text_writter.timer_effect=0.0;
     text_writter.visible_chars=0;
     text_writter.speed=speed;
     list_text_writters.push_back(text_writter);
   }
   else{
      uint16_t index=Entity_toDense[id_label].index_label;
      labels_list[index].state.text_label=text;
      labels_list[index].state.has_modify=true;
   }
}

//Register a the Header Data of a UI Entity
void UI_System::register_Header(uint16_t id,UI_header header){
     Entity_toDense[id].header=header;
}
//Register a Button
void UI_System::register_button(uint16_t id,std::vector<uint16_t> color_states,std::vector<uint16_t> sprites_states,std::vector<uint16_t> colors_labels,bool is_animated,Button_Event button_evs){
   uint16_t next_id=buttons_list.size();
   Entity_toDense[id].index_button=next_id;
   Button btn;
   btn.data_event=button_evs;
   btn.colors_states=color_states;
   btn.colors_labels=colors_labels;
   btn.sprites_states=sprites_states;
   btn.is_animated=is_animated;
   btn.actual_state=UI_STATE_NORMAL;
   btn.recent_selected=false;
   buttons_list.push_back(btn);
   Entity_toDense[id].UI_mask|=(1U<<UI_BUTTON);
}

//Register a Label
void UI_System::register_label(uint16_t id,std::string text,uint16_t color_text,uint8_t align,uint16_t text_boxWidth,uint16_t text_boxHeight,bool single_line,uint16_t offsetx,uint16_t offsety,uint8_t font_index){

   Label lbl;
   lbl.state.text_label=text;
   lbl.state.has_modify=false;
   lbl.visual.font_index=font_index;
   lbl.visual.texture_label=0xFF;
   lbl.visual.text_color=color_text;
   lbl.layout.offsetx=offsetx;
   lbl.layout.offsety=offsety;
   lbl.layout.label_boxHeight=text_boxHeight;
   lbl.layout.label_boxWidth=text_boxWidth;
   lbl.layout.align=align;
   lbl.layout.single_line=single_line;

   uint16_t next_id=labels_list.size();
   Entity_toDense[id].index_label=next_id;
   labels_list.push_back(lbl);
   Entity_toDense[id].UI_mask|=(1U<<UI_LABEL);
}

//register a Panel
void UI_System::register_panel(uint16_t id,bool draw_border,uint16_t bg_color,uint16_t border_color,Panel_Events panel_ev){

   uint16_t next_id=panels_list.size();
   Entity_toDense[id].index_panel=next_id;
   Panel pnl;
   pnl.data_event=panel_ev;
   pnl.bg_color=bg_color;
   pnl.border_color=border_color;
   pnl.draw_border=draw_border;
   pnl.offset_scrollX=0;
   pnl.offset_scrollY=0;
   Entity_toDense[id].UI_mask|=(1U<<UI_PANEL);
   panels_list.push_back(pnl);

}

//Register a Value Bar
void UI_System::register_valueBar(uint16_t id,float initial_val,std::vector<uint16_t> bar_colors,std::vector<uint16_t> bar_sprites, uint16_t border_val,uint16_t bg_color,std::vector<uint16_t> bg_sprites,uint16_t max_width,uint16_t max_height,uint8_t orientation,bool use_bar_colors,uint16_t width_bar,uint16_t height_bar)
{
   uint16_t next_id=valueBars_list.size();
   Entity_toDense[id].index_valueBar=next_id;
   ValueBar bar;
   bar.visuals.color_style.colors_state=bar_colors;
   bar.visuals.color_style.bg_color=bg_color;
   bar.visuals.color_style.border=border_val;
   bar.visuals.color_style.use_color=use_bar_colors;
   bar.visuals.spriteStyle.saved_texture=0xFF;
   bar.visuals.spriteStyle.sprites_bar=bar_sprites;
   bar.visuals.spriteStyle.sprites_bg=bg_sprites;
   bar.layout.height=height_bar;
   bar.layout.width=width_bar;
   bar.layout.max_height=max_height;
   bar.layout.max_width=max_width;
   bar.layout.orientation=orientation;
   bar.state.value_bar=initial_val;
   bar.state.last_value=initial_val;

   if(initial_val>=0.7){
      bar.visuals.actual_state=VALUEBAR_STATE_COMPLETE;
   }
   else if(initial_val>=0.3){
      bar.visuals.actual_state=VALUEBAR_STATE_MEDIUM;
   }
   else{
      bar.visuals.actual_state=VALUEBAR_STATE_LOW;
   }
   Entity_toDense[id].UI_mask|=(1U<<UI_VALUEBAR);
   valueBars_list.push_back(bar);
}

//Register a Slider
void UI_System::register_slider(uint16_t id,float initial_val,std::vector<uint16_t> bar_color,std::vector<uint16_t> bg_color,std::vector<uint16_t> slider_color,std::vector<uint16_t> slider_sprite,uint16_t max_width,uint16_t max_height,uint8_t orientation,Slider_Event slider_ev){
   Slider slider;
   slider.visual.bar_color=bar_color;
   slider.visual.bg_color=bg_color;
   slider.visual.slider_color=slider_color;
   slider.visual.slider_sprite=slider_sprite;
   slider.layout.max_h=max_height;
   slider.layout.max_w=max_width;
   slider.layout.orientation=orientation;
   slider.state.value=initial_val;
   slider.state.last_value=initial_val;
   slider.event_data=slider_ev;
   slider.visual.actual_state=0;

   uint16_t next_index=list_sliders.size();
   Entity_toDense[id].index_slider=next_index;
   Entity_toDense[id].UI_mask|=(1U<<UI_SLIDER);
   list_sliders.push_back(slider);

}
//Register a Check Button
void UI_System ::register_checkButton(uint16_t id,std::vector<uint16_t> colors,std::vector<uint16_t> sprites_states,uint8_t initial_state,std::string intern_data){
  CheckButton check_btn;
  check_btn.state=initial_state;
  check_btn.sprites_states=sprites_states;
  check_btn.value=intern_data;
  if(colors.size()>=3){
     check_btn.bg_color=colors[0];
     check_btn.button_color=colors[1];
     check_btn.disabled_color=colors[2];
  }
  uint16_t next_index=checkbtns_list.size();
  Entity_toDense[id].index_checkButton=next_index;
  Entity_toDense[id].UI_mask|=(1U<<UI_CHECKBUTTON);
  checkbtns_list.push_back(check_btn);

}

//Register a Radio Button
void UI_System::register_RadioButton(uint16_t id,std::vector<uint16_t> colors,std::vector<uint16_t> sprites_states,uint8_t initial_state,std::string intern_data,uint8_t id_radio){
   Radio_Button btn;
   uint16_t next_index=radiobtns_list.size();
   btn.state=initial_state;
   btn.sprites_states=sprites_states;
   btn.value=intern_data;
   if(colors.size()>=3){
     btn.bg_color=colors[0];
     btn.button_color=colors[1];
     btn.disabled_color=colors[2];
   }
   btn.radio_id=id_radio;
   Entity_toDense[id].index_radioBtn=next_index;
   radiobtns_list.push_back(btn);
   Entity_toDense[id].UI_mask|=(1U<<UI_RADIOBUTTON);
}

//Register a ComboBox
void UI_System:: register_comboBox(uint16_t id,std::vector<uint16_t> colors,uint16_t max_width,uint16_t min_height,uint16_t max_height,std::vector<std::string> items,uint8_t max_show_items,float porcentSize_btn,uint16_t sprite_btn,ComboBox_Event box_ev){
   ComboBox box;
   box.layout.max_height=max_height;
   box.layout.min_height=min_height;
   box.layout.width_box=max_width;
   box.data.items=items;
   box.data_event=box_ev;
   box.layout.offset_items=0;
   box.layout.open=false;
   box.data.selected_item=0;
   box.layout.max_showItems=max_show_items;
   box.layout.porceSize_btn=porcentSize_btn;
   if(colors.size()>=4){
      box.visual.color_style.bg_color=colors[0];
      box.visual.color_style.itemHover_color=colors[1];
      box.visual.color_style.itemTextHover_color=colors[2];
      box.visual.color_style.color_btn=colors[3];
   }
   box.visual.sprite_btn=sprite_btn;
   uint16_t next_index=comboBoxes_list.size();
   Entity_toDense[id].index_comboBox=next_index;
   Entity_toDense[id].UI_mask|=(1U<<UI_COMBOBOX);
   comboBoxes_list.push_back(box);
}

//Register a ListBox
void UI_System::register_listBox(uint16_t id,uint16_t bg_color,uint16_t item_color,uint16_t icon_sprite,std::vector<std::string> items,uint8_t max_show_items,uint16_t height_items,ListBox_Events listbox_evs){
   List_Box list_bx;
   list_bx.visual.bg_color=bg_color;
   list_bx.visual.item_color=item_color;
   list_bx.visual.icon_sprite=icon_sprite;
   list_bx.layout.heigth_items=height_items;
   list_bx.layout.max_item_show=max_show_items;
   list_bx.layout.offset=0;
   list_bx.data.items=items;
   list_bx.data.selected_item=0;
   list_bx.data_event=listbox_evs;
   uint16_t next_index=listBox_list.size();
   Entity_toDense[id].index_list=next_index;
   Entity_toDense[id].UI_mask|=(1U<<UI_LIST);
   listBox_list.push_back(list_bx);

}

//Register a ScrollBar
void UI_System:: register_scrollBar(uint16_t id,uint16_t width_bg,uint16_t height_bg,uint16_t area_w,uint16_t area_h,uint16_t color_bg,std::vector<uint16_t> colors_bar,uint8_t orientation,float porcentSz_btns,uint16_t btns_sprite,uint16_t btns_color,ScrollBar_Event scroll_ev){
   ScrollBar bar;
   bar.data_event=scroll_ev;
   bar.layout.orientation=orientation;
   bar.visual.actual_state=UI_STATE_NORMAL;
   if(area_w==0 || area_h==0 || width_bg==0 || height_bg==0){
      return;
   }
   uint16_t min_sz=std::min(width_bg,height_bg);
   bar.layout.size_buttons=min_sz*porcentSz_btns;
   bar.visual.buttons_sprite=btns_sprite;
   if(orientation==UI_ORIENTATION_HORIZONTAL){
     width_bg-=(bar.layout.size_buttons*2);
     float porcent_w=Math_G::Clamp((float)width_bg/(float)area_w,0.00,1.0);
     bar.layout.width_bar=width_bg*porcent_w;
     bar.layout.height_bar=height_bg;

   }
   else{
      height_bg-=(bar.layout.size_buttons*2);
      float porcent_h=Math_G::Clamp((float)height_bg/(float)area_h,0.00,1.0);
      bar.layout.height_bar=height_bg*porcent_h;
      bar.layout.width_bar=width_bg;
   }
   bar.layout.max_width_Bg=width_bg;
   bar.layout.max_height_bg=height_bg;
   bar.layout.width_area=area_w;
   bar.layout.height_area=area_h;
   bar.visual.colorStyle.buttons_color=btns_color;
   bar.visual.colorStyle.bg_color=color_bg;
   bar.visual.colorStyle.bar_colors=colors_bar;
   bar.visual.actual_state=UI_STATE_NORMAL;
   bar.desplacement=0;

   uint16_t next_index=scrollbars_list.size();
   Entity_toDense[id].index_scrollbar=next_index;
   Entity_toDense[id].UI_mask|=(1U<<UI_SCROLLBAR);
   scrollbars_list.push_back(bar);

}

//Register a Icon Image
void UI_System::register_Icon(uint16_t id,std::vector<uint16_t> list_sprites,bool can_changeSprite){
   UI_Icon icon;
   icon.can_changeSprite=can_changeSprite;
   icon.sprites_states=list_sprites;
   icon.actual_state=UI_STATES::UI_STATE_NORMAL;
   uint16_t next_index=list_icons.size();
   Entity_toDense[id].index_icon=next_index;
   Entity_toDense[id].UI_mask|=(1U<<UI_ICON);
   list_icons.push_back(icon);
}

//return True if the Mouse is In Collision with Widget Rect
bool UI_System::mouse_inCollisionWidget(uint16_t mouse_x,uint16_t mouse_y,int16_t widget_cx,int16_t widget_cy,uint16_t widget_halfwidth,uint16_t widget_halfheight){
  if(mouse_x>=widget_cx-widget_halfwidth && mouse_x<=widget_cx+widget_halfwidth){
    if(mouse_y>=widget_cy-widget_halfheight && mouse_y<=widget_cy+widget_halfheight){
         return true;
    }
  }
   return false;
}


//getter Methods
UI_header* UI_System::get_UIHeader(uint16_t id){
   if(id>=Entity_toDense.size()){
       return NULL;
   }
   return &Entity_toDense[id].header;
}

Button* UI_System::get_button(uint16_t id){
    if((Entity_toDense[id].UI_mask & (1U<<UI_BUTTON))!=0){
         return &buttons_list[Entity_toDense[id].index_button];
    }
    return NULL;
}

Label* UI_System::get_label(uint16_t id){
    if((Entity_toDense[id].UI_mask & (1U<<UI_LABEL))!=0){
         return &labels_list[Entity_toDense[id].index_label];
    }
    return NULL;
}

Panel* UI_System::get_panel(uint16_t id){
   if((Entity_toDense[id].UI_mask & (1U<<UI_PANEL))!=0){
         return &panels_list[Entity_toDense[id].index_panel];
    }
    return NULL;
}

ValueBar* UI_System::get_valueBar(uint16_t id){
    if((Entity_toDense[id].UI_mask & (1U<<UI_VALUEBAR))!=0){
         return &valueBars_list[Entity_toDense[id].index_valueBar];
    }
    return NULL;
}

CheckButton* UI_System::get_checkBtn(uint16_t id){
   if((Entity_toDense[id].UI_mask & (1U<<UI_CHECKBUTTON))!=0){
         return &checkbtns_list[Entity_toDense[id].index_checkButton];
    }
    return NULL;
}

Radio_Button* UI_System::get_radioBtn(uint16_t id){
   if((Entity_toDense[id].UI_mask & (1U<<UI_RADIOBUTTON))!=0){
         return &radiobtns_list[Entity_toDense[id].index_radioBtn];
    }
    return NULL;
}
ComboBox* UI_System::get_comboBox(uint16_t id){
    if((Entity_toDense[id].UI_mask & (1U<<UI_COMBOBOX))!=0){
         return &comboBoxes_list[Entity_toDense[id].index_comboBox];
    }
    return NULL;
}
UI_Icon* UI_System::get_Icon(uint16_t id){
    if((Entity_toDense[id].UI_mask & (1U<<UI_ICON))!=0){
         return &list_icons[Entity_toDense[id].index_icon];
    }
    return NULL;
}

ScrollBar* UI_System::get_scrollbar(uint16_t id){
    if((Entity_toDense[id].UI_mask & (1U<<UI_SCROLLBAR))!=0){
         return &scrollbars_list[Entity_toDense[id].index_scrollbar];
    }
    return NULL;
}

Slider* UI_System::get_slider(uint16_t id){
    if((Entity_toDense[id].UI_mask & (1U<<UI_SLIDER))!=0){
         return &list_sliders[Entity_toDense[id].index_slider];
    }
    return NULL;
}

List_Box* UI_System::get_list_box(uint16_t id){
    if((Entity_toDense[id].UI_mask & (1U<<UI_LIST))!=0){
         return &listBox_list[Entity_toDense[id].index_list];
    }
    return NULL;
}

//Save a Texture of a ValueBar
uint8_t UI_System::save_valueBar_texture(SDL_Texture* tex,SDL_Rect tex_rect,SDL_Rect world_rect){
   Rendered_ValueBars target;
   target.tex=tex;
   target.rect_tex=tex_rect;
   target.rect_world=world_rect;
   uint8_t index=saved_bars_textures.size();
   saved_bars_textures.push_back(target);
   return index;
}

//Get the Saved Texture of a ValueBar
Rendered_ValueBars* UI_System::get_saved_valueBar_texture(uint8_t index){
     if(index>=saved_bars_textures.size()){
         return NULL;
     }
     return &saved_bars_textures[index];
}
