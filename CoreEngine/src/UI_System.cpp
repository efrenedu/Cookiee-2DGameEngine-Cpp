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
  radio_groups.clear();
  saved_bars_textures.clear();
}


//Update the Sprite Data of Widgets dependents of Sprite Data of Render Sprite and SpriteComposer Sprites
void UI_System::update_Widgets_Sprites( Scene* target_scene,std::vector<uint16_t> ui_entities){
   if(target_scene==NULL ){
       return;
   }
   SpriteComposer_System* composer_sys=target_scene->get_SpriteComposer_sys();
   Render_System* render_sys=target_scene->get_render_sys();
   for(uint16_t i=0;i<ui_entities.size();i++){
      uint16_t index=ui_entities[i];
      if(index>=target_scene->get_count_entities()){
          continue;
      }
      bitset<64> mask=target_scene->get_mask(index);
      if(HasComponentBit(mask,UI_COMP)==false){
          continue;
      }
      UI_header header=Entity_toDense[index].header;
      if(header.active_state==UI_ACTIVE_STATE_INACTIVE){
         continue;
      }
      Render_data* render_dat=render_sys->get_render(index);
      if(render_dat==NULL){
          continue;
      }
      switch(header.main_type){
         case UI_BUTTON:{
            Button* btn=get_button(index);
            SpriteComposer_Data* composer=NULL;
            if(HasComponentBit(mask,SPRITECOMPOSER_COMP)){
                 composer=composer_sys->get_spriteComposer(index);
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
             UI_Icon* icon=get_Icon(index);
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
             CheckButton* check=get_checkBtn(index);
             if(check!=NULL){
                uint16_t target_sprite=check->sprites_states[check->state];
                if(target_sprite>=0xFFFF){
                    continue;
                }
                if(target_sprite!=render_dat->sprite_id){
                   EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
                }
                render_dat->sprite_id=target_sprite;
             }
              break;
          }
          case UI_RADIOBUTTON:{
             Radio_Button* btn=get_radioBtn(index);
             if(btn!=NULL){
                uint16_t target_sprite=btn->visual.sprites_states[btn->data.state];
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
void UI_System::update(float dt,std::vector<uint16_t> ui_entities){
   update_labels_TextWritters(dt);
   for(uint16_t i=0;i<ui_entities.size();i++){
       uint16_t index=ui_entities[i];
       if(index>=Entity_toDense.size()){
           continue;
       }
       if(Entity_toDense[index].header.active_state==UI_ACTIVE_STATE_INACTIVE && Entity_toDense[i].header.main_type!=UI_PANEL){
           continue;
       }
       uint16_t parent_panel=Entity_toDense[index].header.id_panel;
       int16_t scroll_x=0;
       int16_t scroll_y=0;
       Panel* parent_panel_data=NULL;
       if(parent_panel<Entity_toDense.size()){
          parent_panel_data=get_panel(parent_panel);
          if(parent_panel_data!=NULL){
             scroll_x=parent_panel_data->offset_scrollX;
             scroll_y=parent_panel_data->offset_scrollY;
          }
       }
       switch(Entity_toDense[index].header.main_type){
          case UI_BUTTON:{
             Button* btn=get_button(index);
             Label* lbl=get_label(index);
             if(btn!=NULL){
                 update_buttons(index,Entity_toDense[index].header,btn,lbl,scroll_x,scroll_y,dt);
             }
             break;
          }
          case UI_LIST:{
              List_Box* l_box=get_list_box(index);
              ScrollBar* scroll=get_scrollbar(index);
              if(l_box!=NULL){
                 update_listBoxs(index,Entity_toDense[index].header,l_box,scroll,scroll_x,scroll_y,dt);
              }
              break;
          }
          case UI_COMBOBOX:{
              ComboBox* c_box=get_comboBox(index);
              ScrollBar* scroll=get_scrollbar(index);
              Label* lbl=get_label(index);
              if(c_box!=NULL){
                 update_ComboBoxes(index,Entity_toDense[index].header,c_box,lbl,scroll,scroll_x,scroll_y,dt);
              }
              break;
          }
          case UI_RADIOBUTTON:{
              Radio_Button* btn=get_radioBtn(index);
              Label* lbl=get_label(index);
              if(btn!=NULL){
                 int8_t btn_index=btn->data.index_radioGroup;
                 std::vector<uint16_t> available_focus=get_RadioButton_Available_Focus(btn->data.radioGroup_id,btn_index);
                 bool has_select=update_Radiobuttons(index,Entity_toDense[index].header,btn,lbl,scroll_x,scroll_y,dt,available_focus);
                 if(has_select){
                   OnRadio_Button_Select(btn->data.radioGroup_id,btn_index);
                 }
              }
              break;
          }
          case UI_CHECKBUTTON:{
              CheckButton* check=get_checkBtn(index);
              Label* lbl=get_label(index);
              if(check!=NULL){
                 update_checkbuttons(index,Entity_toDense[index].header,check,lbl,scroll_x,scroll_y,dt);
              }
              break;
          }
          case UI_PANEL:{

              ScrollBar* scroll=get_scrollbar(index);
              Panel* pnl=get_panel(index);
              if(pnl!=NULL){
                update_panels(index,&Entity_toDense[index].header,pnl,dt);

                if(scroll!=NULL && Entity_toDense[index].header.active_state!=UI_ACTIVE_STATE_INACTIVE){
                    update_ScrollBars(index,Entity_toDense[index].header,scroll,scroll_x,scroll_y,dt);
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
              Slider* slider=get_slider(index);
              if(slider!=NULL){
                 update_sliders(index,Entity_toDense[index].header,slider,scroll_x,scroll_y,dt);
              }
              break;
          }
          default:break;

       }

   }
}

//A Radio Button has selected
void UI_System::OnRadio_Button_Select(uint8_t radio_group_id,uint8_t btn_index){
    if(radio_groups.find(radio_group_id)==radio_groups.end()){
          return;
    }
    std::vector<uint16_t> target_radio_group=radio_groups[radio_group_id];
    for(uint8_t i=0;i<target_radio_group.size();i++){
       if(i==btn_index){
          continue;
       }
       uint16_t target_sibling=target_radio_group[i];
       if(target_sibling>=Entity_toDense.size()){
          continue;
       }
       if(Entity_toDense[target_sibling].header.active_state!=UI_ACTIVE_STATE_ACTIVE){
          continue;
       }
       Radio_Button* sibling_btn=get_radioBtn(target_sibling);
       Label* lbl_sibling=get_label(target_sibling);
       if(sibling_btn==NULL){
           continue;
       }
       if(sibling_btn->data.state!=OPTION_CHECKED){
          continue;
       }
       sibling_btn->data.state=OPTION_UNCHECKED;
       if(lbl_sibling!=NULL){
          lbl_sibling->visual.text_color=sibling_btn->visual.text_colors[OPTION_UNCHECKED];
       }
       EventPayload payload;
       payload.sender_type=UI_EVENT_SENDER_RADIOBUTTON;
       payload.value_data=sibling_btn->data.index_radioGroup;
       payload.state_data=false;
       EngineContext::get().get_uiEvent().trigget_event(sibling_btn->event_data.callfunction,payload);

   }
}

//get the  sibling of selected RadioButton or CheckButton
std::vector<uint16_t> UI_System::get_RadioButton_Available_Focus(uint8_t radio_group_id,uint8_t id_btn){

    uint16_t preview=0xFFFF;
    uint16_t next=0xFFFF;
    if(radio_groups.find(radio_group_id)==radio_groups.end()){
        std::vector<uint16_t> res={next,preview};
        return res;
    }
    bool found_next=false;
    bool found_preview=false;
    int8_t next_index=id_btn+1;
    int8_t preview_index=id_btn-1;
    std::vector<uint16_t> target_radio_group=radio_groups[radio_group_id];

    while(next_index<target_radio_group.size() && found_next==false){
          uint16_t index=target_radio_group[next_index];
          uint8_t state_btn=Entity_toDense[index].header.active_state;
          if(state_btn==UI_ACTIVE_STATE_ACTIVE){
               next=index;
               found_next=true;
          }
          next_index++;
    }
    while(preview_index>=0 && preview_index<target_radio_group.size() && found_preview==false){
          uint16_t index=target_radio_group[preview_index];
          uint8_t state_btn=Entity_toDense[index].header.active_state;
          if(state_btn==UI_ACTIVE_STATE_ACTIVE){
               preview=index;
               found_preview=true;
          }
          preview_index--;
    }

    std::vector<uint16_t> res={next,preview};
    return res;
}



//Methods for Update Dinamic UI Components
void UI_System:: update_buttons(uint16_t id,UI_header header,Button* btn,Label* lbl,int16_t scroll_x,int16_t scroll_y,float dt){
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
      if(mouse_inCollisionWidget(mouse_pos[0],mouse_pos[1],header.centerX-scroll_x,header.centerY-scroll_y,header.half_width,header.half_height)==true){
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
       uint16_t key_trigger=inpt.get_ActionKey(btn->data_event.trigger_key);
       uint8_t target_state=UI_STATE_NORMAL;
       uint8_t trigger_key_st=inpt.get_keyState(key_trigger);
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

//Update State and Events of Listbox
void UI_System:: update_listBoxs(uint16_t id,UI_header header,List_Box* l_box,ScrollBar* scroll,int16_t scroll_x,int16_t scroll_y,float dt){
   if(l_box==NULL){
      return;
   }
   if(header.active_state==UI_ACTIVE_STATE_DISABLED){
      return;
   }

   uint8_t input_drive=EngineContext::get().get_input().get_input_drive();
   Input_System inpt=EngineContext::get().get_input();
   bool trigger_ev=false;
   bool trigger_hover=false;
   int16_t select_item=l_box->data.selected_item;

   if((input_drive & (1U<<INPUT_DRIVE_MOUSE))!=0 && l_box->data_event.useKey_Inputs==false){

       uint16_t scroll_w=0;
       if(scroll!=NULL && l_box->layout.direction_items==UI_ORIENTATION_VERTICAL){
          scroll_w=scroll->layout.width_bar;
          float last_desplacement=scroll->desplacement;
          update_ScrollBars(id,header,scroll,scroll_x,scroll_y,dt);
          if(scroll->desplacement!=last_desplacement){
              uint16_t max_itemsScroll=l_box->data.items.size()-l_box->layout.max_item_show;
              if(max_itemsScroll>0){
                uint16_t target_offset=(max_itemsScroll+1)*scroll->desplacement;
                if(target_offset>=max_itemsScroll+1){
                     target_offset=max_itemsScroll;
                }
                l_box->layout.offset=target_offset;
              }
          }
       }


       std::vector<uint16_t> mouse_pos=inpt.get_mousePosition();
       int16_t cx=(header.centerX-scroll_x)-(scroll_w*0.5);
       int16_t cy=header.centerY-scroll_y;
       uint16_t half_w=(header.half_width)-(scroll_w*0.5);
       uint16_t half_h=header.half_height;
       if(mouse_inCollisionWidget(mouse_pos[0],mouse_pos[1],cx,cy,half_w,half_h)==true){
            int16_t localPos=0;
            uint16_t target_dim=0;
            if(l_box->layout.direction_items==UI_ORIENTATION_VERTICAL){
                localPos=mouse_pos[1]-(cy-half_h);
                target_dim=(l_box->data.items.size()>=l_box->layout.max_item_show)?half_h*2:(l_box->data.items.size()*l_box->layout.heigth_items)+(l_box->data.items.size()*l_box->layout.Separation_ItemsY);
            }
            else{
                localPos=mouse_pos[0]-(cx-half_w);
                target_dim=(l_box->data.items.size()>=l_box->layout.max_item_show)?half_w*2:(l_box->data.items.size()*l_box->layout.width_items)+(l_box->data.items.size()*l_box->layout.Separation_ItemsX);
            }
            if(localPos>=0.0 && localPos<(target_dim) && l_box->data.items.size()>0){
                float normalizedVal=(float)localPos/(float)(target_dim);
                int16_t visible_count=static_cast<int16_t>(std::min(l_box->layout.max_item_show,static_cast<uint16_t>(l_box->data.items.size())));
                int16_t rowInView=static_cast<int16_t>(normalizedVal*visible_count);
                int16_t target_index=l_box->layout.offset+rowInView;
                if(target_index>=l_box->data.items.size()){
                    target_index=l_box->data.items.size()-1;
                }
                if(l_box->data.items_states[target_index]!=UI_STATE_DISABLED){
                      select_item=target_index;
                }
                else{
                    select_item=-1;
                }
            }
            if(inpt.get_mouseState(LEFT_BUTTON)==INPUT_DOWN && select_item!=-1){
               trigger_ev=true;
            }
       }
       else{
          select_item=-1;
       }
       if(l_box->data.selected_item!=select_item){
            if(select_item!=-1){
               l_box->data.items_states[select_item]=UI_STATE_HOVER;
               trigger_hover=true;
            }
            if(l_box->data.selected_item!=-1){
               l_box->data.items_states[l_box->data.selected_item]=UI_STATE_NORMAL;
            }
            l_box->data.selected_item=select_item;
            EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
        }
   }
   else if((input_drive & (1U<<INPUT_DRIVE_KEYBOARD))!=0 && l_box->data_event.useKey_Inputs==true){

        if(l_box->data_event.isSelected==false){
           return;
        }
        uint16_t up_key=inpt.get_ActionKey(l_box->data_event.up_key);
        uint16_t down_key=inpt.get_ActionKey(l_box->data_event.down_key);
        uint16_t trigger_key=inpt.get_ActionKey(l_box->data_event.trigger_key);
        uint8_t state_up=inpt.get_keyState(up_key);
        uint8_t state_down=inpt.get_keyState(down_key);
        uint8_t state_trigger=inpt.get_keyState(trigger_key);
        if(state_down==INPUT_DOWN){
            bool search_index=true;
            uint16_t increment=1;
            while(search_index){
                int16_t target_index=select_item+increment;
                if(target_index>=l_box->data.items.size()){
                    search_index=false;
                    continue;
                }
                if(l_box->data.items_states[target_index]!=UI_STATE_DISABLED){
                      select_item=target_index;
                      search_index=false;
                }
                increment++;
            }

        }
        else if(state_up==INPUT_DOWN){
            bool search_index=true;
            uint16_t increment=1;
            while(search_index){
                int16_t target_index=select_item-increment;
                if(target_index<0){
                    search_index=false;
                    continue;
                }
                if(l_box->data.items_states[target_index]!=UI_STATE_DISABLED){
                      select_item=target_index;
                      search_index=false;
                }
                increment++;
            }
        }
        if(state_trigger==INPUT_DOWN){
            trigger_ev=true;
        }
        if(l_box->data.selected_item!=select_item){
            trigger_hover=true;
            if(l_box->data.selected_item!=-1){
               l_box->data.items_states[l_box->data.selected_item]=UI_STATE_NORMAL;
            }
            l_box->data.selected_item=select_item;
            l_box->data.items_states[select_item]=UI_STATE_HOVER;

            EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
            if(select_item>=l_box->layout.max_item_show){
               l_box->layout.offset=select_item-(l_box->layout.max_item_show-1);
            }
            else{
               l_box->layout.offset=0;
            }
        }

        if(scroll!=NULL){
           uint16_t max_itemsScroll=l_box->data.items.size()-l_box->layout.max_item_show;
           if(max_itemsScroll>0){
              float new_desplacement=(float)l_box->layout.offset/(float)max_itemsScroll;
              scroll->desplacement=new_desplacement;
           }

        }
   }
   if(trigger_ev==true || trigger_hover==true ){
      EventPayload payload;
      payload.entity_source=id;
      std::string call_function=(trigger_ev)?l_box->data_event.callfunction:l_box->data_event.callHoverFunction;
      payload.sender_type=UI_EVENT_SENDER_LISTBOX;
      payload.value_data=l_box->data.selected_item;
      EngineContext::get().get_uiEvent().trigget_event(call_function,payload);
   }
}

//Update State and Events ComboBox
void UI_System:: update_ComboBoxes(uint16_t id,UI_header header,ComboBox* c_box,Label* lbl,ScrollBar* scroll,int16_t scroll_x,int16_t scroll_y,float dt){
   if(c_box==NULL){
      return;
   }
   if(header.active_state==UI_ACTIVE_STATE_DISABLED){
      return;
   }

   uint8_t input_drive=EngineContext::get().get_input().get_input_drive();
   bool has_modify=false;
   bool trigger_ev=false;
   if((input_drive & (1U<<INPUT_DRIVE_MOUSE))!=0 && c_box->data_event.useKeys_Input==false){
     if(scroll!=NULL){
          float last_desplacement=scroll->desplacement;
          update_ScrollBars(id,header,scroll,scroll_x,scroll_y,dt);
          if(scroll->desplacement!=last_desplacement){
              uint16_t max_itemsScroll=c_box->data.items.size()-c_box->layout.max_showItems;
              if(max_itemsScroll>0){
                uint16_t target_offset=(max_itemsScroll+1)*scroll->desplacement;
                if(target_offset>=max_itemsScroll+1){
                     target_offset=max_itemsScroll;
                }
                c_box->layout.offset_items=target_offset;
              }
          }
      }
      int16_t cx=(header.centerX-scroll_x);
      int16_t cy=header.centerY-scroll_y;
      uint16_t half_width=(c_box->layout.width_box*0.5);
      uint16_t half_heigth=c_box->layout.min_height*0.5;
      if(c_box->layout.open){
          half_heigth=(c_box->layout.max_height*0.5);
          cy+=half_heigth;
      }
      std::vector<uint16_t> mouse_pos=EngineContext::get().get_input().get_mousePosition();
      bool click_mouse=EngineContext::get().get_input().get_mouseState(LEFT_BUTTON)==INPUT_DOWN;
      if(mouse_inCollisionWidget(mouse_pos[0],mouse_pos[1],cx,cy,half_width,half_heigth)==true){
         if(c_box->layout.open){
            if(scroll!=NULL){
                uint16_t scroll_w=scroll->layout.max_width_Bg;
                uint16_t scroll_px=(header.centerX+header.half_width)-scroll_w;
                if(mouse_pos[0]>=scroll_px && mouse_pos[0]<=scroll_px+scroll_w){
                     return;
                }
            }
            int16_t localY=0;
            uint16_t target_size=(c_box->data.items.size()>=c_box->layout.max_showItems)?half_heigth*2:c_box->data.items.size()*c_box->layout.min_height;
            localY=mouse_pos[1]-(cy-half_heigth);
            if(localY>=0.0 && localY<target_size && c_box->data.items.size()>0){
                float normalizedVal=(float)localY/(float)target_size;
                int8_t visible_count=static_cast<int8_t>(std::min(c_box->layout.max_showItems,static_cast<uint8_t>(c_box->data.items.size())));
                int8_t rowInView=static_cast<int8_t>(normalizedVal*visible_count);
                int8_t target_index=c_box->layout.offset_items+rowInView;
                if(target_index>=c_box->data.items.size()){
                    target_index=c_box->data.items.size()-1;
                }
                if(target_index!=c_box->data.selected_item){
                    if(c_box->data.selected_item!=-1){
                       c_box->data.items_states[c_box->data.selected_item]=UI_STATE_NORMAL;
                    }
                    c_box->data.selected_item=target_index;
                    c_box->data.items_states[target_index]=UI_STATE_HOVER;
                    has_modify=true;
                }
            }
         }
         if(click_mouse){
            if(c_box->layout.open){
               trigger_ev=true;
               if(lbl!=NULL){
                  lbl->state.text_label=c_box->data.items[c_box->data.selected_item];
                  lbl->state.has_modify=true;
               }
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
  else if((input_drive & (1U<<INPUT_DRIVE_KEYBOARD))!=0 && c_box->data_event.useKeys_Input==true){
        if( c_box->data_event.selected==false){
            return;
        }

        Input_System inpt=EngineContext::get().get_input();
        uint16_t trigger_key=inpt.get_ActionKey(c_box->data_event.trigget_key);
        uint16_t up_key=inpt.get_ActionKey(c_box->data_event.Up_key);
        uint16_t down_key=inpt.get_ActionKey(c_box->data_event.Down_key);

        uint8_t trigger_state=inpt.get_keyState(trigger_key);
        if(c_box->layout.open){
           uint8_t down_key_st=inpt.get_keyState(down_key);
           uint8_t up_key_st=inpt.get_keyState(up_key);
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
               if(c_box->data.selected_item!=-1){
                  c_box->data.items_states[c_box->data.selected_item]=UI_STATE_NORMAL;
               }
               c_box->data.selected_item=next_index;
               c_box->data.items_states[next_index]=UI_STATE_HOVER;
               has_modify=true;
           }
        }
        if(trigger_state==INPUT_DOWN){
            if(c_box->layout.open){
               trigger_ev=true;
               if(lbl!=NULL){
                  lbl->state.text_label=c_box->data.items[c_box->data.selected_item];
                  lbl->state.has_modify=true;
               }
            }
            c_box->layout.open=!c_box->layout.open;
            has_modify=true;
        }

        if(c_box->data.selected_item>=c_box->layout.max_showItems){
            c_box->layout.offset_items=c_box->data.selected_item-(c_box->layout.max_showItems-1);
        }
         else{
             c_box->layout.offset_items=0;
        }


        if(scroll!=NULL){
           uint16_t max_itemsScroll=c_box->data.items.size()-c_box->layout.max_showItems;
           if(max_itemsScroll>0){
              float new_desplacement=(float)c_box->layout.offset_items/(float)max_itemsScroll;
              scroll->desplacement=new_desplacement;
           }

        }

   }
   if(has_modify){
      EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      if(trigger_ev){
         EventPayload payload;
         payload.entity_source=id;
         payload.value_data=c_box->data.selected_item;
         payload.sender_type=UI_EVENT_SENDER_COMBOBOX;
         EngineContext::get().get_uiEvent().trigget_event(c_box->data_event.callfunction,payload);
      }
   }
}
void UI_System:: update_sliders(uint16_t id,UI_header header,Slider* slider,int16_t scroll_x,int16_t scroll_y,float dt){
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
     int16_t cx=header.centerX-scroll_x;
     int16_t cy=header.centerY-scroll_y;
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
        Input_System inpt=EngineContext::get().get_input();
        uint16_t up_key=inpt.get_ActionKey(slider->event_data.key_upValue);
        uint16_t down_key=inpt.get_ActionKey(slider->event_data.key_downValue);
        uint8_t state_upValue_Key=inpt.get_keyState(up_key);
        uint8_t state_DownValue_Key=inpt.get_keyState(down_key);
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
      payload.entity_source=id;
      payload.sender_type=UI_EVENT_SENDER_SLIDER;
      payload.value_data=desplz;
      EngineContext::get().get_uiEvent().trigget_event(slider->event_data.callFunction,payload);
   }
}
void UI_System:: update_ScrollBars(uint16_t id,UI_header header,ScrollBar* scroll,int16_t scroll_x,int16_t scroll_y,float dt){
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

       int16_t c_barX=header.centerX-scroll_x;
       int16_t c_barY=header.centerY-scroll_y;
       uint16_t halfw_bar=scroll->layout.width_bar*0.5;
       uint16_t halfh_bar=scroll->layout.height_bar*0.5;
       if(scroll->layout.orientation==UI_ORIENTATION_HORIZONTAL){
           c_barX=(((header.centerX-scroll_x)-header.half_width)+scroll->layout.size_buttons)+(scroll->desplacement*(scroll->layout.max_width_Bg-scroll->layout.width_bar));
           c_barX+=halfw_bar;
           c_barY+=header.half_height-halfh_bar;
       }
       else{
           c_barY=(((header.centerY-scroll_y)-header.half_height)+scroll->layout.size_buttons)+(scroll->desplacement*(scroll->layout.max_height_bg-scroll->layout.height_bar));
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
        Input_System inpt=EngineContext::get().get_input();
        uint16_t up_key=inpt.get_ActionKey(scroll->data_event.upValue_key);
        uint16_t down_key=inpt.get_ActionKey(scroll->data_event.downValue_key);

        float step=scroll->data_event.step;
        uint8_t state_upValue_Key=inpt.get_keyState(up_key);
        uint8_t state_DownValue_Key=inpt.get_keyState(down_key);
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
void UI_System::update_panels(uint16_t id,UI_header* header,Panel* pnl,float dt){
   if(header->active_state==UI_ACTIVE_STATE_DISABLED){
       return;
   }
   if(pnl->data_event.useKey_Inputs){
       Input_System inpt=EngineContext::get().get_input();
       uint16_t open_key=inpt.get_ActionKey(pnl->data_event.open_key);
       uint16_t close_key=inpt.get_ActionKey(pnl->data_event.close_key);
       uint8_t key_open=inpt.get_keyState(open_key);
       uint8_t key_close=inpt.get_keyState(close_key);
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

//Update RadioButtons Events and States
bool UI_System::update_Radiobuttons(uint16_t id,UI_header header,Radio_Button* btn,Label* lbl,int16_t scroll_x,int16_t scroll_y,float dt,std::vector<uint16_t> available_focus){
   if(btn==NULL){
       return false;
   }
   bool has_selected=false;
   if(header.active_state==UI_ACTIVE_STATE_DISABLED){
       if(btn->data.state!=UI_STATE_DISABLED){
         btn->data.state=OPTION_DISABLED;
         if(lbl!=NULL){
             lbl->visual.text_color=btn->visual.text_colors[OPTION_DISABLED];
         }
         EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
       }
       return false;
   }
   if(btn->event_data.sleep_count>0.0f){
      float limit=0.1;
      float val=btn->event_data.sleep_count;
      val+=dt;
      if(val>=limit){
         val=0.0f;
      }
      btn->event_data.sleep_count=val;
      return false;
   }
   bool trigger_ev=false;
   uint8_t input_drive=EngineContext::get().get_input().get_input_drive();
   Input_System inpt=EngineContext::get().get_input();

   if((input_drive & (1U<<INPUT_DRIVE_MOUSE))!=0 && btn->event_data.useKeys_Input==false){
      std::vector<uint16_t> mouse_pos=inpt.get_mousePosition();
      uint8_t next_state=btn->data.state;
      if(mouse_inCollisionWidget(mouse_pos[0],mouse_pos[1],header.centerX,header.centerY,header.half_width,header.half_height)==true){

         if(next_state==OPTION_UNCHECKED || next_state==OPTION_HOVER){
              next_state=OPTION_HOVER;
              if(inpt.get_mouseState(LEFT_BUTTON)==INPUT_DOWN){
                 next_state=OPTION_CHECKED;
                 trigger_ev=true;
                 has_selected=true;
              }
         }

      }
      else{
        if(next_state==OPTION_HOVER){
            next_state=OPTION_UNCHECKED;
        }
      }
      if(next_state!=btn->data.state){
          btn->data.state=next_state;
          if(lbl!=NULL){
             lbl->visual.text_color=btn->visual.text_colors[next_state];
          }
          EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      }
   }
   else if((input_drive & (1U<<INPUT_DRIVE_KEYBOARD))!=0 && btn->event_data.useKeys_Input==true){
       uint8_t next_state=btn->data.state;
       if(btn->event_data.selected==false){
           if(next_state==OPTION_HOVER || next_state==OPTION_CHECKED){
               btn->data.state=OPTION_UNCHECKED;
               if(lbl!=NULL){
                  lbl->visual.text_color=btn->visual.text_colors[OPTION_UNCHECKED];
               }
               EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
           }
           return false;
       }

       if(next_state!=OPTION_CHECKED){
          next_state=OPTION_CHECKED;
       }
       uint16_t up_key=inpt.get_ActionKey(btn->event_data.nextBtn_key);
       uint16_t down_key=inpt.get_ActionKey(btn->event_data.previewBtn_key);

       if(inpt.get_keyState(up_key)==INPUT_DOWN){
          if(available_focus[0]<Entity_toDense.size()){
              Radio_Button* next_focus=get_radioBtn(available_focus[0]);
              next_focus->event_data.selected=true;
              next_focus->event_data.sleep_count=dt;
              if(next_state!=OPTION_CHECKED){
                next_state=OPTION_UNCHECKED;
              }
              btn->event_data.selected=false;
              EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
          }
       }
       else if(inpt.get_keyState(down_key)==INPUT_DOWN){
         if(available_focus[1]<Entity_toDense.size()){
              Radio_Button* preview_focus=get_radioBtn(available_focus[1]);
              preview_focus->event_data.sleep_count=dt;
              preview_focus->event_data.selected=true;
              btn->event_data.selected=false;
              if(next_state!=OPTION_CHECKED){
                next_state=OPTION_UNCHECKED;
              }
              EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
          }
       }

       if(next_state!=btn->data.state){
          btn->data.state=next_state;
          if(lbl!=NULL){
             lbl->visual.text_color=btn->visual.text_colors[next_state];
          }
          EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
       }

   }
   if(trigger_ev){
       EventPayload payload;
       payload.entity_source=id;
       payload.state_data=(btn->data.state==UI_STATE_INTERACT)?true:false;
       payload.sender_type=UI_EVENT_SENDER_RADIOBUTTON;
       payload.value_data=btn->data.index_radioGroup;
       EngineContext::get().get_uiEvent().trigget_event(btn->event_data.callfunction,payload);

   }
   return has_selected;
}

//Update CheckButton Events and States
void UI_System::update_checkbuttons(uint16_t id,UI_header header,CheckButton* btn,Label* lbl,int16_t scroll_x,int16_t scroll_y,float dt){
    if(btn==NULL){
       return;
   }

   if(header.active_state==UI_ACTIVE_STATE_DISABLED){
       if(btn->state!=OPTION_DISABLED){
         btn->state=OPTION_DISABLED;
         if(lbl!=NULL){
            lbl->visual.text_color=btn->colors_text[OPTION_DISABLED];
         }
         EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
       }
       return;
   }
   if(btn->event_data.sleep_count>0.0f){
      float limit=0.1;
      float val= btn->event_data.sleep_count;
      val+=dt;
      if(btn->event_data.sleep_count>=limit){
        val=0.0f;
      }
      btn->event_data.sleep_count=val;
      return;
   }
   bool trigger_ev=false;
   uint8_t input_drive=EngineContext::get().get_input().get_input_drive();
   Input_System inpt=EngineContext::get().get_input();

   if((input_drive & (1U<<INPUT_DRIVE_MOUSE))!=0 && btn->event_data.useKeys_Input==false){
      std::vector<uint16_t> mouse_pos=inpt.get_mousePosition();
      uint8_t next_state=btn->state;
      if(mouse_inCollisionWidget(mouse_pos[0],mouse_pos[1],header.centerX,header.centerY,header.half_width,header.half_height)==true){

         if(next_state==OPTION_UNCHECKED || next_state==OPTION_HOVER){
              next_state=OPTION_HOVER;
              if(inpt.get_mouseState(LEFT_BUTTON)==INPUT_DOWN){
                 next_state=OPTION_CHECKED;
                 trigger_ev=true;
              }
         }
         else if(next_state==OPTION_CHECKED){
            if(inpt.get_mouseState(LEFT_BUTTON)==INPUT_DOWN){
                 next_state=OPTION_HOVER;
                 trigger_ev=true;
              }
         }
      }
      else{
        if(next_state==OPTION_HOVER){
            next_state=OPTION_UNCHECKED;
        }
      }
      if(next_state!=btn->state){
          btn->state=next_state;
          if(lbl!=NULL){
            lbl->visual.text_color=btn->colors_text[next_state];
          }
          EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
      }
   }
   else if((input_drive & (1U<<INPUT_DRIVE_KEYBOARD))!=0 && btn->event_data.useKeys_Input==true){
       uint8_t next_state=btn->state;
       if(btn->event_data.selected==false){
           if(next_state==OPTION_HOVER){
               btn->state=OPTION_UNCHECKED;
               if(lbl!=NULL){
                  lbl->visual.text_color=btn->colors_text[OPTION_UNCHECKED];
               }
               EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
           }
           return;
       }

       if(next_state==OPTION_UNCHECKED){
          next_state=OPTION_HOVER;
       }
       uint16_t up_key=inpt.get_ActionKey(btn->event_data.nextBtn_key);
       uint16_t down_key=inpt.get_ActionKey(btn->event_data.previewBtn_key);
       uint16_t trigger_key=inpt.get_ActionKey(btn->event_data.trigget_key);

       bool has_loss_focus=false;
       if(inpt.get_keyState(up_key)==INPUT_DOWN){
          if(btn->event_data.next_focus<Entity_toDense.size()){
              uint8_t state_focus=Entity_toDense[btn->event_data.next_focus].header.active_state;
              if(state_focus==UI_ACTIVE_STATE_ACTIVE){
                  CheckButton* next_btn=get_checkBtn(btn->event_data.next_focus);
                  if(next_btn!=NULL){
                      next_btn->event_data.selected=true;
                      next_btn->event_data.sleep_count=dt;
                  }
                  btn->event_data.selected=false;
                  if(next_state!=OPTION_CHECKED){
                     next_state=OPTION_UNCHECKED;
                  }
                  has_loss_focus=true;
              }
          }
       }
       else if(inpt.get_keyState(down_key)==INPUT_DOWN){
          if(btn->event_data.preview_focus<Entity_toDense.size()){
              uint8_t state_focus=Entity_toDense[btn->event_data.preview_focus].header.active_state;
              if(state_focus==UI_ACTIVE_STATE_ACTIVE){
                CheckButton* preview_btn=get_checkBtn(btn->event_data.preview_focus);
                if(preview_btn!=NULL){
                    preview_btn->event_data.selected=true;
                    preview_btn->event_data.sleep_count=dt;
                }
                 btn->event_data.selected=false;
                 if(next_state!=OPTION_CHECKED){
                     next_state=OPTION_UNCHECKED;
                 }
                  has_loss_focus=true;
              }
          }
       }
       if(inpt.get_keyState(trigger_key)==INPUT_DOWN && has_loss_focus==false){
          if(next_state==OPTION_HOVER){
              next_state=OPTION_CHECKED;
              trigger_ev=true;
          }
          else if(next_state==OPTION_CHECKED){
             next_state=OPTION_HOVER;
             trigger_ev=true;
          }
       }
       if(next_state!=btn->state){
          btn->state=next_state;
           if(lbl!=NULL){
              lbl->visual.text_color=btn->colors_text[next_state];
          }
          EngineContext::get().get_drawContext().request_redraw(REDRAW_ENTITIES);
       }
   }
   if(trigger_ev){
       EventPayload payload;
       payload.entity_source=id;
       payload.state_data=(btn->state==UI_STATE_INTERACT)?true:false;
       payload.sender_type=UI_EVENT_SENDER_CHECKBUTTON;
       payload.value_data=btn->event_data.arg_event;
       EngineContext::get().get_uiEvent().trigget_event(btn->event_data.callfunction,payload);

   }
}

//update labels Effects
void UI_System::update_labels_TextWritters(float dt){
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
void UI_System ::register_checkButton(uint16_t id,std::vector<uint16_t> colors,std::vector<uint16_t> colors_text,std::vector<uint16_t> sprites_states,uint8_t initial_state,CheckButton_Event check_evs){
  CheckButton check_btn;
  check_btn.event_data=check_evs;
  check_btn.state=initial_state;
  check_btn.sprites_states=sprites_states;
  check_btn.colors_states=colors;
  check_btn.colors_text=colors_text;
  check_btn.event_data.sleep_count=0.0f;
  uint16_t next_index=checkbtns_list.size();
  Entity_toDense[id].index_checkButton=next_index;
  Entity_toDense[id].UI_mask|=(1U<<UI_CHECKBUTTON);
  checkbtns_list.push_back(check_btn);

}

//Register a Radio Button
void UI_System::register_RadioButton(uint16_t id,Radio_Button_Visual visual, Radio_Button_Data data,Radio_Button_Event event_data){

   Radio_Button btn;
   uint16_t next_index=radiobtns_list.size();
   btn.visual=visual;
   btn.data=data;
   btn.event_data=event_data;
   btn.event_data.sleep_count=0.0f;
   if(radio_groups.find(btn.data.radioGroup_id)!=radio_groups.end()){
      std::vector<uint16_t> list_radios=radio_groups[btn.data.radioGroup_id];
      btn.data.index_radioGroup=list_radios.size();
      list_radios.push_back(id);
      radio_groups[btn.data.radioGroup_id]=list_radios;
   }
   else{
      std::vector<uint16_t> list_radios;
      btn.data.index_radioGroup=0;
      list_radios={id};
      radio_groups[btn.data.radioGroup_id]=list_radios;
   }
   Entity_toDense[id].index_radioBtn=next_index;
   radiobtns_list.push_back(btn);
   Entity_toDense[id].UI_mask|=(1U<<UI_RADIOBUTTON);

}

//Register a ComboBox
void UI_System:: register_comboBox(uint16_t id,ComboBox_Visual visual,ComboBox_layout layout,std::vector<std::string> items,ComboBox_Event box_ev){
   ComboBox box;
   box.layout=layout;
   box.visual=visual;
   box.data.items=items;
   box.data.items_states.assign(items.size(),UI_STATE_NORMAL);
   box.data_event=box_ev;
   box.layout.offset_items=0;
   box.layout.open=false;
   box.data.selected_item=0;
   if(items.size()>0){
      box.data.items_states[0]=UI_STATE_HOVER;
   }
   box.visual.saved_texts.assign(items.size(),0xFF);
   uint16_t next_index=comboBoxes_list.size();
   Entity_toDense[id].index_comboBox=next_index;
   Entity_toDense[id].UI_mask|=(1U<<UI_COMBOBOX);
   comboBoxes_list.push_back(box);
}

//Register a ListBox
void UI_System::register_listBox(uint16_t id,ListBox_Layout listbx_layout,ListBox_Visual listbx_visual,std::vector<std::string> items,std::vector<uint8_t> items_states,ListBox_Events listbox_evs){
   List_Box list_bx;
   list_bx.layout=listbx_layout;
   list_bx.visual=listbx_visual;
   list_bx.visual.saved_Itemstext_textures.assign(items.size(),0xFF);
   list_bx.layout.offset=0;
   list_bx.data.items=items;
   list_bx.data.selected_item=0;
   if(items_states.size()>0){
      items_states[0]=UI_STATE_HOVER;
   }
   list_bx.data.items_states=items_states;


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
