#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H
#include<vector>
#include<stdint.h>
#include<string>

class Entity_Props;
enum UI_ORIENTATION{
   UI_ORIENTATION_VERTICAL=0,
   UI_ORIENTATION_HORIZONTAL=1,
};
enum UI_TYPES:uint8_t{
   UI_BUTTON=0,
   UI_LABEL=1,
   UI_PANEL=2,
   UI_RADIOBUTTON=3,
   UI_CHECKBUTTON=4,
   UI_COMBOBOX=5,
   UI_OPTIONBOX=6,
   UI_VALUEBAR=7,
   UI_SCROLLBAR=8,
   UI_TEXTFIELD=9,
   UI_NONE=10

};

enum LABEL_ALIGN:uint8_t{
    LABEL_ALIGN_LEFT=0,
    LABEL_ALIGN_CENTER=1,
    LABEL_ALIGN_RIGHT=2
};

enum UI_STATES{
   UI_STATE_NORMAL=0,
   UI_STATE_SELECTED=1,
   UI_STATE_INTERACT=2
};
struct UI_Event{
   uint16_t source_ev;
   uint8_t ev_type;
};

struct UI_header{
  std::vector<uint8_t> evs_Load;
  std::vector<uint8_t> evs_Select;
  std::vector<uint8_t> evs_Unselect;
  int16_t centerX;
  int16_t centerY;
  uint16_t half_width;
  uint16_t half_height;
  uint16_t flags;
  bool has_text;
};


struct Button{
  std::vector<uint16_t> colors_states;
  std::vector<uint16_t>sprites_states;
  uint8_t actual_state;
  bool is_animated;
  bool recent_selected;



};
struct Label{
  std::string text_label;
  std::vector<uint16_t> colors_states;
  int16_t offsetx;
  int16_t offsety;
  uint16_t label_boxWidth;
  uint16_t label_boxHeight;
  uint8_t actual_state;
  uint8_t font_index;
  uint8_t last_text_index;
  uint8_t align;
  bool single_line;
  bool has_modify;
  bool recent_selected;
};
struct Panel{
   uint16_t bg_color;
   uint16_t border_color;
   uint16_t sprite_bg;
};

struct ValueBar{
  float value;
  float max_value;
  uint16_t bg_sprite;
  uint16_t bar_sprite;
  uint16_t bg_color;
  uint16_t bar_color;
  uint8_t orientation;
  bool use_sprites;
  bool is_composed;


};

struct ScrollBar{
 uint16_t bg_color;
 uint16_t bar_color;
 float desplacement;
 std::vector<uint16_t> desplace_entities;
 uint8_t orientation;


};

struct Radio_Button{
   uint16_t bg_color;
   uint16_t button_color;
   uint8_t radio_id;
   std::string value;
   bool checked;
};
struct ComboBox{
  std::vector<std::string> items;
  bool open;
  uint8_t max_showItems;
  int8_t selected_item;

};
struct CheckButton{
   uint16_t bg_color;
   uint16_t button_color;
   std::string value;
   bool checked;
};

struct OptionBox{
   std::vector<uint16_t> items_affected;
   uint16_t Entity_SelectMark;
   uint16_t bg_color;
   uint16_t bg_sprite;
   uint16_t border_color;
   bool has_changed;
   uint8_t number_options;
   uint8_t direction;
};

struct UIEntity_Data{
   UI_header header;
   uint16_t index_button;
   uint16_t index_label;
   uint16_t index_valueBar;
   uint16_t index_checkButton;
   uint16_t index_panel;
   uint16_t index_scrollbar;
   uint16_t index_comboBox;
   uint16_t UI_mask;

};

struct Text_Writter_Animation{
    uint16_t label_id;
    std::string full_text;
    uint16_t visible_chars;
    float timer_effect;
    float speed;
    bool is_complete;
};

struct UI_System
{
    public:
        void Reserve_Memory(uint16_t ui_components_counts,uint16_t entities_count,uint16_t fillval);
        void Free_Memory();
        void update(float dt);
        void update_States();
        void Process_Events();
        void set_label_text(std::string text,uint16_t id_label,bool use_typeWritter=false,float speed=0.03);
        void register_Header(uint16_t id,UI_header header);
        void register_button(uint16_t id,std::vector<uint16_t> color_states,std::vector<uint16_t> sprites_states,bool is_animated);
        void register_panel(uint16_t id,Entity_Props* props);
        void register_label(uint16_t id,std::string text,std::vector<uint16_t> color_states,uint8_t align,uint16_t text_boxWidth,uint16_t text_boxHeight,bool single_line,uint16_t offsetx,uint16_t offsety,uint8_t font_index);
        void register_valueBar(uint16_t id,Entity_Props* props);
        void register_scrollBar(uint16_t id,Entity_Props* props);
        void register_comboBox(uint16_t id,Entity_Props* props);
        void register_checkButton(uint16_t id,Entity_Props* props);
        void register_RadioButton(uint16_t id,Entity_Props* props);
        UI_header* get_UIHeader(uint16_t id);
        Button* get_button(uint16_t id);
        Label* get_label(uint16_t id);
        Radio_Button* get_radioBtn(uint16_t id);
        CheckButton* get_checkBtn(uint16_t id);
        Panel* get_panel(uint16_t id);
        ValueBar* get_valueBar(uint16_t id);
        ScrollBar* get_scrollbar(uint16_t id);
        ComboBox* get_comboBox(uint16_t id);
    private:
        void update_labels(float dt);
        std::vector<Button> buttons_list;
        std::vector<Label> labels_list;
        std::vector<Radio_Button> radiobtns_list;
        std::vector<Panel> panels_list;
        std::vector<CheckButton> checkbtns_list;
        std::vector<ValueBar> valueBars_list;
        std::vector<ScrollBar> scrollbars_list;
        std::vector<ComboBox> comboBoxes_list;
        std::vector<UIEntity_Data> Entity_toDense;
        std::vector<UI_Event> evs_queue;
        std::vector<Text_Writter_Animation> list_text_writters;

};

#endif // UI_SYSTEM_H
