#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H
#include<vector>
#include<stdint.h>
#include<string>
#include<SDL.h>

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
   UI_ICON=10,
   UI_LIST=11,
   UI_SLIDER=12,
   UI_NONE=13

};

enum OPTION_STATE:uint8_t{
    OPTION_CHECKED=0,
    OPTION_UNCHECKED=1,
    OPTION_DISABLED=2
};

enum LABEL_ALIGN:uint8_t{
    LABEL_ALIGN_LEFT=0,
    LABEL_ALIGN_CENTER=1,
    LABEL_ALIGN_RIGHT=2
};

enum VALUEBAR_STATES:uint8_t{
    VALUEBAR_STATE_COMPLETE=0,
    VALUEBAR_STATE_MEDIUM=1,
    VALUEBAR_STATE_LOW=2
};

enum UI_STATES:uint8_t{
   UI_STATE_NORMAL=0,
   UI_STATE_HOVER=1,
   UI_STATE_INTERACT=2,
   UI_STATE_DISABLED=3
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
  uint8_t main_type;
  bool has_text;
};

struct List_Box{
   uint16_t bg_color;
   uint16_t item_color;
   uint16_t icon_sprite;
   std::vector<std::string> items;
   uint16_t max_items_show;
   uint16_t heigth_items;
   uint8_t offset;
   uint8_t selected_item;
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
   bool draw_border;
};

struct Slider{
    uint16_t bar_color;
    uint16_t slider_sprite;
    uint16_t slider_color;
    uint16_t bg_color;
    float value;
    uint16_t max_w;
    uint16_t max_h;
    uint8_t orientation;
};
struct ValueBar{
  std::vector<uint16_t> bar_colors;
  std::vector<uint16_t> bar_sprites;
  std::vector<uint16_t> bg_sprites;
  float value;
  float last_value;
  uint16_t bg_color;
  uint16_t border_val;
  uint16_t max_width;
  uint16_t max_height;
  uint16_t width_bar;
  uint16_t height_bar;
  uint8_t saved_texture;
  uint8_t bar_state;
  uint8_t orientation;
  bool use_bar_colors;

};

struct ScrollBar{
 std::vector<uint16_t> bar_colors;
 std::vector<uint16_t> desplace_entities;
 uint16_t btns_sprite;
 uint16_t buttons_color;
 uint16_t bg_color;
 uint16_t max_width_Bg;
 uint16_t max_height_bg;
 uint16_t area_width;
 uint16_t area_height;
 uint16_t width_bar;
 uint16_t height_bar;
 uint16_t size_buttons;
 float desplacement;
 uint8_t actual_state;
 uint8_t orientation;


};

struct Radio_Button{
   uint16_t bg_color;
   uint16_t button_color;
   uint16_t disabled_color;
   std::vector<uint16_t> sprites_states;
   uint8_t radio_id;
   std::string value;
   uint8_t state;
};
struct ComboBox{
  std::vector<std::string> items;
  uint16_t bg_color;
  uint16_t itemHover_color;
  uint16_t itemTextHover_color;
  uint16_t color_btn;
  uint16_t sprite_btn;
  uint16_t width_box;
  uint16_t max_height;
  uint16_t min_height;
  float porceSize_btn;
  bool open;
  uint8_t max_showItems;
  uint8_t offset_items;
  int8_t selected_item;

};
struct CheckButton{
   uint16_t bg_color;
   uint16_t button_color;
   uint16_t disabled_color;
   std::vector<uint16_t> sprites_states;
   std::string value;
   uint8_t state;
};

struct UI_Icon{
   std::vector<uint16_t> sprites_states;
   bool can_changeSprite;
   uint8_t actual_state;
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
   uint16_t index_radioBtn;
   uint16_t index_icon;
   uint16_t index_slider;
   uint16_t index_list;
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

//for Save Rendered Complex Texture of Value Bars and use it later for Render the bar
struct Rendered_ValueBars{
  SDL_Texture* tex;
  SDL_Rect rect_tex;
  SDL_Rect rect_world;

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
        void register_panel(uint16_t id,bool draw_border,uint16_t bg_color,uint16_t border_color);
        void register_label(uint16_t id,std::string text,std::vector<uint16_t> color_states,uint8_t align,uint16_t text_boxWidth,uint16_t text_boxHeight,bool single_line,uint16_t offsetx,uint16_t offsety,uint8_t font_index);
        void register_valueBar(uint16_t id,float initial_val,std::vector<uint16_t> bar_colors,std::vector<uint16_t> bar_sprites, uint16_t border_val,uint16_t bg_color,std::vector<uint16_t> bg_sprites,uint16_t max_width,uint16_t max_height,uint8_t orientation,bool use_bar_colors,uint16_t width_bar,uint16_t height_bar);
        void register_slider(uint16_t id,float initial_val,uint16_t bar_color,uint16_t bg_color,uint16_t slider_color,uint16_t slider_sprite,uint16_t max_width,uint16_t max_height,uint8_t orientation);
        void register_listBox(uint16_t id,uint16_t bg_color,uint16_t itemColor,uint16_t icon_sprite,std::vector<std::string> items,uint8_t max_show_items,uint16_t heigth_items);
        void register_scrollBar(uint16_t id,uint16_t width_bg,uint16_t height_bar,uint16_t area_w,uint16_t area_h,uint16_t color_bg,std::vector<uint16_t> colors_bar,uint8_t oirentation,float porcent_szBtns,uint16_t btns_sprite,uint16_t color_buttons);
        void register_comboBox(uint16_t id,std::vector<uint16_t> colors,uint16_t max_width,uint16_t min_height,uint16_t max_height,std::vector<std::string> items,uint8_t max_show_items,float porcentSize_btn,uint16_t sprite_btn);
        void register_checkButton(uint16_t id,std::vector<uint16_t> colors,std::vector<uint16_t> sprites_states,uint8_t initial_state,std::string intern_data);
        void register_RadioButton(uint16_t id,std::vector<uint16_t> colors,std::vector<uint16_t> sprites_states,uint8_t initial_state,std::string intern_data,uint8_t id_radio);
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
        std::vector<UI_Icon> list_icons;
        std::vector<Slider> list_sliders;
        std::vector<List_Box> listBox_list;
        std::vector<Rendered_ValueBars> saved_bars_textures;

};

#endif // UI_SYSTEM_H
