#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H
#include<vector>
#include<stdint.h>
#include<string>
#include<SDL.h>
#include<bitset>
#include<unordered_map>
class Input_System;
class Entity_Props;
class Scene;


//Orientation for Bars
enum UI_ORIENTATION{
   UI_ORIENTATION_VERTICAL=0,
   UI_ORIENTATION_HORIZONTAL=1,
};

//UI Types
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
   UI_ICON=10,
   UI_LIST=11,
   UI_SLIDER=12,
   UI_NONE=13

};

//States of CheckButton or Radio Buttons
enum OPTION_STATE:uint8_t{
    OPTION_UNCHECKED=0,
    OPTION_HOVER=1,
    OPTION_CHECKED=2,
    OPTION_DISABLED=3
};

//Label Text Align
enum LABEL_ALIGN:uint8_t{
    LABEL_ALIGN_LEFT=0,
    LABEL_ALIGN_CENTER=1,
    LABEL_ALIGN_RIGHT=2
};

//Bar State of ValueBar for plain color bars
enum VALUEBAR_STATES:uint8_t{
    VALUEBAR_STATE_COMPLETE=0,
    VALUEBAR_STATE_MEDIUM=1,
    VALUEBAR_STATE_LOW=2
};

//Visual State of UI
enum UI_STATES:uint8_t{
   UI_STATE_NORMAL=0,
   UI_STATE_HOVER=1,
   UI_STATE_INTERACT=2,
   UI_STATE_DISABLED=3,
};

enum BUTTON_TIMERS:uint8_t{
    BUTTON_TIMER_COOLDOWN=0,
    BUTTON_TIMER_INTERACT=1,
    BUTTON_TIMER_TRIGGER_EV=2,
    BUTTON_TIMER_SHOWTEXT_COOLDOWN=3
};

//Active State of UI
enum UI_ACTIVE_STATE:uint8_t{
     UI_ACTIVE_STATE_ACTIVE=0,
     UI_ACTIVE_STATE_INACTIVE=1,
     UI_ACTIVE_STATE_DISABLED=2,

};

struct UI_header{
  int16_t centerX;
  int16_t centerY;
  uint16_t half_width;
  uint16_t half_height;
  uint16_t id_panel;
  uint8_t active_state;
  uint8_t main_type;
};


//Layout Data of ListBox
struct ListBox_Layout{
   uint16_t max_item_show;
   uint16_t heigth_items;
   uint16_t width_items;
   uint16_t Separation_ItemsX;
   uint16_t Separation_ItemsY;
   uint8_t direction_items;
   uint16_t offset;
};

//Visual Data of ListBox
struct ListBox_Visual{
   std::vector<uint16_t> bg_color;
   std::vector<uint16_t> items_colors;
   uint16_t item_bg_color;
   std::vector<uint8_t> saved_Itemstext_textures;
   uint8_t font_id;
   uint16_t icon_sprite;
};

//Event Data of ListBox
struct ListBox_Events{
   std::string callfunction;
   std::string callHoverFunction;
   uint8_t up_key;
   uint8_t down_key;
   uint8_t trigger_key;
   bool useKey_Inputs;
   bool isSelected;

};
//ListBox Data
struct ListBox_Data{
   std::vector<std::string> items;
   std::vector<uint8_t>items_states;
   int16_t selected_item;
};
//ListBox Component
struct List_Box{
   ListBox_Events data_event;
   ListBox_Layout layout;
   ListBox_Visual visual;
   ListBox_Data data;
};

//Button Event Data
struct Button_Event{
    std::vector<float> wait_timers;
    std::vector<float> count_timers;
    std::string callfunction;
    std::string initial_labelText;
    float arg_func;
    uint8_t trigger_key;
    bool use_keyInput;
    bool selected;
    bool always_selected;
    bool recent_interact;
    bool show_cooldown_time;

};

struct Button{
  Button_Event data_event;
  std::vector<uint16_t> colors_states;
  std::vector<uint16_t> colors_labels;
  std::vector<uint16_t>sprites_states;
  uint8_t actual_state;
  bool is_animated;
  bool recent_selected;

};

//Label Layout Data
struct Label_Layout{
   int16_t offsetx;
   int16_t offsety;
   uint16_t label_boxWidth;
   uint16_t label_boxHeight;
   uint8_t align;
   bool single_line;
};

//Label Visual Data
struct Label_Visual{
   uint16_t text_color;
   uint8_t font_index;
   uint8_t texture_label;
};

//Label State Data
struct Label_State{
   std::string text_label;
   bool has_modify;
};

//Label Component
struct Label{
  Label_Layout layout;
  Label_Visual visual;
  Label_State state;
};

struct Panel_Events{
   std::string callfunction;
   std::string closeCallFunction;
   float arg_function;
   uint8_t open_key;
   uint8_t close_key;
   bool recent_open;
   bool recent_close;
   bool useKey_Inputs;
   bool isSelected;
};
//Panel Component
struct Panel{
   Panel_Events data_event;
   uint16_t bg_color;
   uint16_t border_color;
   float offset_scrollX;
   float offset_scrollY;
   bool draw_border;
};

//Slider Visual Data
struct Slider_Visual{
    uint8_t actual_state;
    std::vector<uint16_t> bar_color;
    std::vector<uint16_t> slider_sprite;
    std::vector<uint16_t> slider_color;
    std::vector<uint16_t> bg_color;
};

//Slider Layout data
struct Slider_Layout{
    uint16_t max_w;
    uint16_t max_h;
    uint8_t orientation;
};

//Slider State
struct Slider_State{
   float value;
   float last_value;
};

//Slider Event Data
struct Slider_Event{
   std::string callFunction;
   float step_val;
   uint8_t key_upValue;
   uint8_t key_downValue;
   bool useKeys;
   bool dragging;
   bool selected;
};

//Slider Component
struct Slider{
    Slider_Visual visual;
    Slider_Layout layout;
    Slider_State state;
    Slider_Event event_data;
};

//Visual Data of Value Bar
struct ValueBar_Visual{
    struct{
       std::vector<uint16_t> colors_state;
       uint16_t bg_color;
       uint16_t border;
       bool use_color;
    }color_style;
    struct{
       std::vector<uint16_t> sprites_bar;
       std::vector<uint16_t> sprites_bg;
       uint8_t saved_texture;
    }spriteStyle;
    uint8_t actual_state;

};

//Layout Data of ValueBar
struct ValueBar_Layout{
   uint16_t width;
   uint16_t height;
   uint16_t max_width;
   uint16_t max_height;
   uint8_t orientation;
};

//State Data of Value Bar
struct ValueBar_State{
   float value_bar;
   float last_value;
};

//ValueBar Component
struct ValueBar{
  ValueBar_Visual visuals;
  ValueBar_Layout layout;
  ValueBar_State state;
};

//Visual Data of ScrollBar
struct ScrollBar_visual{
  struct{
     std::vector<uint16_t> bar_colors;
     uint16_t buttons_color;
     uint16_t bg_color;
  }colorStyle;
  uint16_t buttons_sprite;
  uint8_t actual_state;


};

//Layout Data of Scrollbar
struct ScrollBar_layout{
    uint16_t max_width_Bg;
    uint16_t max_height_bg;
    uint16_t width_area;
    uint16_t height_area;
    uint16_t width_bar;
    uint16_t height_bar;
    uint16_t size_buttons;
    uint8_t orientation;
};
//Event Data of ScrollBar
struct ScrollBar_Event{
   float step;
   uint8_t upValue_key;
   uint8_t downValue_key;
   bool dragging;
   bool use_keysInput;
   bool is_selected;
};


//ScrollBar Component
struct ScrollBar{
 ScrollBar_Event data_event;
 ScrollBar_visual visual;
 ScrollBar_layout layout;
 float desplacement;

};

//RadioButton Event Data
struct Radio_Button_Event{
   std::string callfunction;
   float sleep_count;
   uint8_t nextBtn_key;
   uint8_t previewBtn_key;
   bool useKeys_Input;
   bool selected;
};

//Data of RadioButton
struct Radio_Button_Data{
    uint8_t index_radioGroup;
    uint8_t radioGroup_id;
    uint8_t state;
};

//Visual Data of RadioButton
struct Radio_Button_Visual{
   std::vector<uint16_t> sprites_states;
   std::vector<uint16_t> colors;
   std::vector<uint16_t> text_colors;
};

//RadioButton Component
struct Radio_Button{
   Radio_Button_Event event_data;
   Radio_Button_Data data;
   Radio_Button_Visual visual;
};

//Layout Data of ComboBox
struct ComboBox_layout{
  uint16_t width_box;
  uint16_t max_height;
  uint16_t min_height;
  float porceSize_btn;
  uint8_t max_showItems;
  int8_t offset_items;
  bool open;
};

//Visual Data of ComboBox
struct ComboBox_Visual{
  struct{
    std::vector<uint16_t> items_colors;
    std::vector<uint16_t> bg_colors;
    std::vector<uint16_t> button_colors;
    uint16_t itemgBg_color;
  }color_style;
  std::vector<uint16_t> sprite_btn;
  std::vector<uint8_t> saved_texts;
  uint8_t font_id;

};

//ComboBox Data
struct ComboBox_Data{
   std::vector<std::string> items;
   std::vector<uint8_t> items_states;
   int8_t selected_item;
};

//Combobox Event Data
struct ComboBox_Event{
   std::string callfunction;
   uint8_t trigget_key;
   uint8_t Up_key;
   uint8_t Down_key;
   bool useKeys_Input;
   bool selected;

};
//ComboBox Component
struct ComboBox{
  ComboBox_Event data_event;
  ComboBox_layout layout;
  ComboBox_Visual visual;
  ComboBox_Data data;

};

//CheckButton Event Data
struct CheckButton_Event{
   std::string callfunction;
   float arg_event;
   uint16_t next_focus;
   uint16_t preview_focus;
   float sleep_count=0.0f;
   uint8_t trigget_key;
   uint8_t nextBtn_key;
   uint8_t previewBtn_key;
   bool useKeys_Input;
   bool selected;
};


//CheckButton Component
struct CheckButton{
   CheckButton_Event event_data;
   std::vector<uint16_t> sprites_states;
   std::vector<uint16_t> colors_states;
   std::vector<uint16_t> colors_text;
   uint8_t state;
};

//Icon Component
struct UI_Icon{
   std::vector<uint16_t> sprites_states;
   bool can_changeSprite;
   uint8_t actual_state;
};

//UI Index Data and Header of Each Entity
struct UIEntity_Data{
   UI_header header;
   uint16_t index_button;
   uint16_t index_label;
   uint16_t index_valueBar;
   uint16_t index_checkButton;
   uint16_t index_panel;
   uint16_t index_scrollbar;
   uint16_t index_comboBox;
   uint16_t index_radioBtn;
   uint16_t index_icon;
   uint16_t index_slider;
   uint16_t index_list;
   uint16_t UI_mask;

};

//For Text Writter Effect on Labels
struct Text_Writter_Animation{
    uint16_t label_id;
    std::string full_text;
    uint16_t visible_chars;
    float timer_effect;
    float speed;
    bool is_complete;
};

//for Save Rendered Complex Texture of Value Bars and use it later for Render the bar
struct Rendered_ValueBars{
  SDL_Texture* tex;
  SDL_Rect rect_tex;
  SDL_Rect rect_world;

};

//UI System for Manage UI Components
struct UI_System
{
    public:
        void Reserve_Memory(uint16_t ui_components_counts,uint16_t entities_count,uint16_t fillval);
        void Free_Memory();
        void update(float dt,std::vector<uint16_t> ui_entities);
        void update_Widgets_Sprites(Scene* target_scene,std::vector<uint16_t> ui_entities);
        void Process_Events();
        void set_label_text(std::string text,uint16_t id_label,bool use_typeWritter=false,float speed=0.03);
        void register_Header(uint16_t id,UI_header header);
        void register_button(uint16_t id,std::vector<uint16_t> color_states,std::vector<uint16_t> sprites_states,std::vector<uint16_t> colors_labels,bool is_animated,Button_Event button_evs);
        void register_panel(uint16_t id,bool draw_border,uint16_t bg_color,uint16_t border_color,Panel_Events panel_ev);
        void register_label(uint16_t id,std::string text,std::uint16_t color_text,uint8_t align,uint16_t text_boxWidth,uint16_t text_boxHeight,bool single_line,uint16_t offsetx,uint16_t offsety,uint8_t font_index);
        void register_valueBar(uint16_t id,float initial_val,std::vector<uint16_t> bar_colors,std::vector<uint16_t> bar_sprites, uint16_t border_val,uint16_t bg_color,std::vector<uint16_t> bg_sprites,uint16_t max_width,uint16_t max_height,uint8_t orientation,bool use_bar_colors,uint16_t width_bar,uint16_t height_bar);
        void register_slider(uint16_t id,float initial_val,std::vector<uint16_t> bar_color,std::vector<uint16_t> bg_color,std::vector<uint16_t> slider_color,std::vector<uint16_t> slider_sprite,uint16_t max_width,uint16_t max_height,uint8_t orientation,Slider_Event slider_ev);
        void register_listBox(uint16_t id,ListBox_Layout listbx_layout,ListBox_Visual listbx_visual,std::vector<std::string> items,std::vector<uint8_t> items_states,ListBox_Events listbox_evs);
        void register_scrollBar(uint16_t id,uint16_t width_bg,uint16_t height_bar,uint16_t area_w,uint16_t area_h,uint16_t color_bg,std::vector<uint16_t> colors_bar,uint8_t oirentation,float porcent_szBtns,uint16_t btns_sprite,uint16_t color_buttons,ScrollBar_Event scroll_ev);
        void register_comboBox(uint16_t id,ComboBox_Visual visual,ComboBox_layout layout,std::vector<std::string> items,ComboBox_Event box_ev);
        void register_checkButton(uint16_t id,std::vector<uint16_t> colors,std::vector<uint16_t> colors_text,std::vector<uint16_t> sprites_states,uint8_t initial_state,CheckButton_Event check_evs);
        void register_RadioButton(uint16_t id,Radio_Button_Visual visual, Radio_Button_Data data,Radio_Button_Event event_data);
        void register_Icon(uint16_t id,std::vector<uint16_t> list_sprites,bool can_changeSprite);
        uint8_t save_valueBar_texture(SDL_Texture* tex,SDL_Rect text_rect,SDL_Rect world_rect);
        Rendered_ValueBars* get_saved_valueBar_texture(uint8_t index);
        UI_header* get_UIHeader(uint16_t id);
        Button* get_button(uint16_t id);
        Label* get_label(uint16_t id);
        Radio_Button* get_radioBtn(uint16_t id);
        CheckButton* get_checkBtn(uint16_t id);
        Panel* get_panel(uint16_t id);
        ValueBar* get_valueBar(uint16_t id);
        ScrollBar* get_scrollbar(uint16_t id);
        ComboBox* get_comboBox(uint16_t id);
        UI_Icon* get_Icon(uint16_t id);
        Slider* get_slider(uint16_t id);
        List_Box* get_list_box(uint16_t id);

    private:
        bool mouse_inCollisionWidget(uint16_t mouse_x,uint16_t mouse_y,int16_t widget_cx,int16_t widget_cy,uint16_t widget_halfwidth,uint16_t widget_halfheight);
        void update_buttons(uint16_t id,UI_header header,Button* btn,Label*lbl,int16_t scroll_x,int16_t scroll_y,float dt);
        void update_listBoxs(uint16_t id,UI_header header,List_Box* l_box,ScrollBar* scroll,int16_t scroll_x,int16_t scroll_y,float dt);
        void update_ComboBoxes(uint16_t id,UI_header header,ComboBox* c_box,Label* lbl,ScrollBar* scroll,int16_t scroll_x,int16_t scroll_y,float dt);
        void update_sliders(uint16_t id,UI_header header,Slider* slider,int16_t scroll_x,int16_t scroll_y,float dt);
        void update_ScrollBars(uint16_t id,UI_header header,ScrollBar* scroll,int16_t scroll_x,int16_t scroll_y,float dt);
        void update_labels_TextWritters(float dt);
        void update_panels(uint16_t id,UI_header* header,Panel* pnl,float dt);
        void update_checkbuttons(uint16_t id,UI_header header,CheckButton* btn,Label* lbl,int16_t scroll_x,int16_t scroll_y,float dt);
        bool update_Radiobuttons(uint16_t id,UI_header header,Radio_Button* btn,Label* lbl,int16_t scroll_x,int16_t scroll_y,float dt,std::vector<uint16_t> available_focus);
        void OnRadio_Button_Select(uint8_t radio_group_id,uint8_t btn_index);
        std::vector<uint16_t> get_RadioButton_Available_Focus(uint8_t radio_group,uint8_t btn_index);
        std::vector<Button> buttons_list;
        std::vector<Label> labels_list;
        std::vector<Radio_Button> radiobtns_list;
        std::vector<Panel> panels_list;
        std::vector<CheckButton> checkbtns_list;
        std::vector<ValueBar> valueBars_list;
        std::vector<ScrollBar> scrollbars_list;
        std::vector<ComboBox> comboBoxes_list;
        std::vector<UIEntity_Data> Entity_toDense;
        std::vector<Text_Writter_Animation> list_text_writters;
        std::vector<UI_Icon> list_icons;
        std::vector<Slider> list_sliders;
        std::vector<List_Box> listBox_list;
        std::vector<Rendered_ValueBars> saved_bars_textures;
        std::unordered_map<uint8_t,std::vector<uint16_t>> radio_groups;
};

#endif // UI_SYSTEM_H
