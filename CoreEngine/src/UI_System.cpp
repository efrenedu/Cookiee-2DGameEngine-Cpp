#include "UI_System.h"
#include<iostream>
#include<math_structs.h>

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
//Update the System
void UI_System::update(float dt){
   update_labels(dt);
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
           labels_list[index].text_label=next_text;
           labels_list[index].has_modify=true;
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
      labels_list[index].text_label=text;
      labels_list[index].has_modify=true;
   }
}

//Register a the Header Data of a UI Entity
void UI_System::register_Header(uint16_t id,UI_header header){
     Entity_toDense[id].header=header;
}
//Register a Button
void UI_System::register_button(uint16_t id,std::vector<uint16_t> color_states,std::vector<uint16_t> sprites_states,bool is_animated){
   uint16_t next_id=buttons_list.size();
   Entity_toDense[id].index_button=next_id;
   Button btn;
   btn.colors_states=color_states;
   btn.sprites_states=sprites_states;
   btn.is_animated=is_animated;
   btn.actual_state=UI_STATE_NORMAL;
   btn.recent_selected=false;
   buttons_list.push_back(btn);
   Entity_toDense[id].UI_mask|=(1U<<UI_BUTTON);
}

//Register a Label
void UI_System::register_label(uint16_t id,std::string text,std::vector<uint16_t> color_states,uint8_t align,uint16_t text_boxWidth,uint16_t text_boxHeight,bool single_line,uint16_t offsetx,uint16_t offsety,uint8_t font_index){
   uint16_t next_id=labels_list.size();
   Entity_toDense[id].index_label=next_id;
   Label lbl;
   lbl.text_label=text;
   lbl.recent_selected=false;
   lbl.actual_state=UI_STATE_NORMAL;
   lbl.colors_states=color_states;
   lbl.offsetx=offsetx;
   lbl.offsety=offsety;
   lbl.font_index=font_index;
   lbl.label_boxWidth=text_boxWidth;
   lbl.label_boxHeight=text_boxHeight;
   lbl.align=align;
   lbl.single_line=single_line;
   lbl.last_text_index=0xFF;
   lbl.has_modify=false;
   labels_list.push_back(lbl);
   Entity_toDense[id].UI_mask|=(1U<<UI_LABEL);
}

//register a Panel
void UI_System::register_panel(uint16_t id,bool draw_border,uint16_t bg_color,uint16_t border_color){

   uint16_t next_id=panels_list.size();
   Entity_toDense[id].index_panel=next_id;
   Panel pnl;
   pnl.bg_color=bg_color;
   pnl.border_color=border_color;
   pnl.draw_border=draw_border;
   Entity_toDense[id].UI_mask|=(1U<<UI_PANEL);
   panels_list.push_back(pnl);

}

//Register a Value Bar
void UI_System::register_valueBar(uint16_t id,float initial_val,std::vector<uint16_t> bar_colors,std::vector<uint16_t> bar_sprites, uint16_t border_val,uint16_t bg_color,std::vector<uint16_t> bg_sprites,uint16_t max_width,uint16_t max_height,uint8_t orientation,bool use_bar_colors,uint16_t width_bar,uint16_t height_bar)
{
   uint16_t next_id=valueBars_list.size();
   Entity_toDense[id].index_valueBar=next_id;
   ValueBar bar;
   bar.bar_colors=bar_colors;
   bar.bar_sprites=bar_sprites;
   bar.bg_color=bg_color;
   bar.border_val=border_val;
   bar.bg_sprites=bg_sprites;
   bar.orientation=orientation;
   bar.value=initial_val;
   bar.max_width=max_width;
   bar.max_height=max_height;
   bar.use_bar_colors=use_bar_colors;
   bar.width_bar=width_bar;
   bar.height_bar=height_bar;
   bar.last_value=initial_val;
   if(initial_val>=0.7){
      bar.bar_state=VALUEBAR_STATE_COMPLETE;
   }
   else if(initial_val>=0.3){
       bar.bar_state=VALUEBAR_STATE_MEDIUM;
   }
   else{
        bar.bar_state=VALUEBAR_STATE_LOW;
   }
   Entity_toDense[id].UI_mask|=(1U<<UI_VALUEBAR);
   valueBars_list.push_back(bar);
}

//Register a Slider
void UI_System::register_slider(uint16_t id,float initial_val,uint16_t bar_color,uint16_t bg_color,uint16_t slider_color,uint16_t slider_sprite,uint16_t max_width,uint16_t max_height,uint8_t orientation){
   Slider slider;
   slider.bar_color=bar_color;
   slider.bg_color=bg_color;
   slider.max_h=max_height;
   slider.max_w=max_width;
   slider.orientation=orientation;
   slider.value=initial_val;
   slider.slider_color=slider_color;
   slider.slider_sprite=slider_sprite;
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
void UI_System:: register_comboBox(uint16_t id,std::vector<uint16_t> colors,uint16_t max_width,uint16_t min_height,uint16_t max_height,std::vector<std::string> items,uint8_t max_show_items,float porcentSize_btn,uint16_t sprite_btn){
   ComboBox box;
   box.max_height=max_height;
   box.min_height=min_height;
   box.width_box=max_width;
   box.items=items;
   box.offset_items=0;
   box.open=false;
   box.selected_item=-1;
   box.max_showItems=max_show_items;
   box.porceSize_btn=porcentSize_btn;
   if(colors.size()>=4){
      box.bg_color=colors[0];
      box.itemHover_color=colors[1];
      box.itemTextHover_color=colors[2];
      box.color_btn=colors[3];
   }
   box.sprite_btn=sprite_btn;
   uint16_t next_index=comboBoxes_list.size();
   Entity_toDense[id].index_comboBox=next_index;
   Entity_toDense[id].UI_mask|=(1U<<UI_COMBOBOX);
   comboBoxes_list.push_back(box);
}

//Register a ListBox
void UI_System::register_listBox(uint16_t id,uint16_t bg_color,uint16_t item_color,uint16_t icon_sprite,std::vector<std::string> items,uint8_t max_show_items,uint16_t height_items){
   List_Box list_bx;
   list_bx.bg_color=bg_color;
   list_bx.item_color=item_color;
   list_bx.icon_sprite=icon_sprite;
   list_bx.heigth_items=height_items;
   list_bx.items=items;
   list_bx.max_items_show=max_show_items;
   list_bx.offset=0;
   list_bx.selected_item=0;
   uint16_t next_index=listBox_list.size();
   Entity_toDense[id].index_list=next_index;
   Entity_toDense[id].UI_mask|=(1U<<UI_LIST);
   listBox_list.push_back(list_bx);

}

//Register a ScrollBar
void UI_System:: register_scrollBar(uint16_t id,uint16_t width_bg,uint16_t height_bg,uint16_t area_w,uint16_t area_h,uint16_t color_bg,std::vector<uint16_t> colors_bar,uint8_t orientation,float porcentSz_btns,uint16_t btns_sprite,uint16_t btns_color){
   ScrollBar bar;
   bar.max_width_Bg=width_bg;
   bar.max_height_bg=height_bg;
   bar.actual_state=UI_STATE_NORMAL;
   bar.area_width=area_w;
   bar.area_height=area_h;
   if(area_w==0 || area_h==0 || width_bg==0 || height_bg==0){
      return;
   }
   bar.desplacement=0;
   bar.orientation=orientation;
   bar.bg_color=color_bg;
   bar.bar_colors=colors_bar;
   uint16_t next_index=scrollbars_list.size();
   uint16_t min_sz=std::min(width_bg,height_bg);
   bar.size_buttons=min_sz*porcentSz_btns;
   if(orientation==UI_ORIENTATION_HORIZONTAL){
     float porcent_w=Math_G::Clamp((float)width_bg/(float)area_w,0.00,1.0);
     bar.width_bar=width_bg*porcent_w;
     bar.height_bar=height_bg;
   }
   else{
      float porcent_h=Math_G::Clamp((float)height_bg/(float)area_h,0.00,1.0);
      bar.height_bar=height_bg*porcent_h;
      bar.width_bar=width_bg;
   }
   bar.btns_sprite=btns_sprite;
   bar.buttons_color=btns_color;
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
