#include "UI_System.h"
#include<iostream>

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
