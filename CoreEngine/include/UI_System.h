#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H
#include<vector>
#include<stdint.h>
#include<string>

class Entity_Props;
enum UI_TYPES:uint8_t{
   UI_BUTTON=0,
   UI_LABEL=1,
   UI_PANEL=2,
   UI_RADIOBUTTON=3,
   UI_CHECKBUTTON=4,
   UI_COMBOBOX=5,
   UI_SLIDER=6,
   UI_CANVS=7,
   UI_VALUEBAR=8,
   UI_SCROLLBAR=9,
   UI_TEXTFIELD=10,

};
struct UI_Event{
   uint16_t source_ev;
   uint8_t ev_type;
};

struct UI_header{
  std::vector<uint8_t> evs_Load;
  std::vector<uint8_t> evs_Interact;
  std::vector<uint8_t> evs_Over;
  uint16_t posX_canvas;
  uint16_t posY_canvas;
  uint16_t widhOn_Canvas;
  uint16_t heightOn_Canvas;
  uint16_t flags;
  uint8_t ui_type;
};
struct Button{
  UI_header header;
  std::vector<uint16_t> colors_states;
  std::vector<uint16_t>sprites_states;
  uint16_t label_id;
  bool is_animated;


};
struct Label{
  UI_header header;
  std::string text_label;
  std::vector<uint16_t> colors_states;
};
struct Panel{
 UI_header header;
 uint16_t bg_color;
 uint16_t border_color;
 uint16_t sprite_bg;
};

struct ValueBar{
  UI_header header;
  float value;
  float max_value;
  uint16_t label_id;
  uint16_t bg_sprite;
  uint16_t bar_sprite;
  uint16_t bg_color;
  uint16_t bar_color;
  uint8_t orientation;
  bool use_sprites;
  bool is_composed;


};

struct ScrollBar{
 UI_header header;
 uint16_t bg_color;
 uint16_t bar_color;
 float desplacement;
 std::vector<uint16_t> desplace_entities;
 uint8_t orientation;


};

struct Radio_Button{
   UI_header header;
   uint16_t bg_color;
   uint16_t button_color;
   uint16_t label_id;
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
   UI_header header;
   uint16_t bg_color;
   uint16_t button_color;
   uint16_t label_id;
   std::string value;
   bool checked;
};

struct UIEntity_Data{
   uint16_t index_button;
   uint16_t index_label;
   uint16_t index_valueBar;
   uint16_t index_checkButton;
   uint16_t index_panel;
   uint16_t index_scrollbar;
   uint16_t index_comboBox;
   uint16_t UI_mask;

};
struct UI_System
{
    public:
        void Reserve_Memory(std::vector<uint16_t> ui_components_counts,uint16_t entities_count);
        void Free_Memory();
        void update_States();
        void Process_Events();
        void register_button(uint16_t id,Entity_Props* props);
        void register_panel(uint16_t id,Entity_Props* props);
        void register_label(uint16_t id,Entity_Props* props);
        void register_valueBar(uint16_t id,Entity_Props* props);
        void register_scrollBar(uint16_t id,Entity_Props* props);
        void register_comboBox(uint16_t id,Entity_Props* props);
        void register_checkButton(uint16_t id,Entity_Props* props);
        void register_RadioButton(uint16_t id,Entity_Props* props);
        Button* get_button(uint16_t id);
        Label* get_label(uint16_t id);
        Radio_Button* get_radioBtn(uint16_t id);
        CheckButton* get_checkBtn(uint16_t id);
        Panel* get_panel(uint16_t id);
        ValueBar* get_valueBar(uint16_t id);
        ScrollBar* get_scrollbar(uint16_t id);
        ComboBox* get_comboBox(uint16_t id);
    private:
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

};

#endif // UI_SYSTEM_H
