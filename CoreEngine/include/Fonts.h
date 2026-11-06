#ifndef FONTS_H
#define FONTS_H
#include<cstdint>
#include <vector>
#include<SDL.h>
#include<SDL_image.h>
#include<sstream>


//Numeric Index of Characters for Bit Map Fonts
enum AVAILABLES_LETTERS:uint8_t{
    FONTCHAR_EMPTY=0,
    FONTCHAR_EXCLAMATION=1,
    FONTCHAR_COMILLA_DOUBLE=2,
    FONTCHAR_NUMERAL=3,
    FONTCHAR_DOLLAR=4,
    FONTCHAR_PORCENT=5,
    FONTCHAR_AMPERSANT=6,
    FONTCHAR_COMILLA_SINGLE=7,
    FONTCHAR_LEFT_PARENTESIS=8,
    FONTCHAR_RIGHT_PARENTESIS=9,
    FONTCHAR_ASTERISC=10,
    FONTCHAR_PLUS=11,
    FONTCHAR_COLLON=12,
    FONTCHAR_MINUS=13,
    FONTCHAR_DOT=14,
    FONTCHAR_DIAGONAL_BAR=15,
    FOTCHAR_0=16,
    FONTCHAR_1=17,
    FONTCHAR_2=18,
    FONTCHAR_3=19,
    FONTCHAR_4=20,
    FONTCHAR_5=21,
    FONTCHAR_6=22,
    FONTCHAR_7=23,
    FONTCHAR_8=24,
    FONTCHAR_9=25,
    FONTCHAR_DOUBLE_DOT=26,
    FONTCHAR_DOUT_AND_COLLLON=27,
    FONTCHAR_LOWER_THAN=28,
    FONTCHAR_EQUALS=29,
    FONTCHAR_GREATER_THAN=30,
    FONTCHAR_INTERROGATION=31,
    FONTCHAR_ARROBA_SIMBOL=32,
    FONTCHAR_A=33,
    FONTCHAR_B=34,
    FONTCHAR_C=35,
    FONTCHAR_D=36,
    FONTCHAR_E=37,
    FONTCHAR_F=38,
    FONTCHAR_G=39,
    FONTCHAR_H=40,
    FONTCHAR_I=41,
    FONTCHAR_J=42,
    FONTCHAR_K=43,
    FONTCHAR_L=44,
    FONTCHAR_M=45,
    FONTCHAR_N=46,
    FONTCHAR_O=47,
    FONTCHAR_P=48,
    FONTCHAR_Q=49,
    FONTCHAR_R=50,
    FONTCHAR_S=51,
    FONTCHAR_T=52,
    FONTCHAR_U=53,
    FONTCHAR_V=54,
    FONTCHAR_W=55,
    FONTCHAR_X=56,
    FONTCHAR_Y=57,
    FONTCHAR_Z=58,
    FONTCHAR_OPEN_CORCHEA=59,
    FONTCHAR_INVERTED_DIAGONAL_BAR=60,
    FONTCHAR_CLOSE_CORCHEA=61,
    FONTCHAR_ACENTO_CIRCUNFLEJO=62,
    FONTCHAR_GUION_BAJO=63,
    FONTCHAR_ACENTO_GRAVE=64,
    FONTCHAR_a=65,
    FONTCHAR_b=66,
    FONTCHAR_c=67,
    FONTCHAR_d=68,
    FONTCHAR_e=69,
    FONTCHAR_f=70,
    FONTCHAR_g=71,
    FONTCHAR_h=72,
    FONTCHAR_i=73,
    FONTCHAR_j=74,
    FONTCHAR_k=75,
    FONTCHAR_l=76,
    FONTCHAR_m=77,
    FONTCHAR_n=78,
    FONTCHAR_o=79,
    FONTCHAR_p=80,
    FONTCHAR_q=81,
    FONTCHAR_r=82,
    FONTCHAR_s=83,
    FONTCHAR_t=84,
    FONTCHAR_u=85,
    FONTCHAR_v=86,
    FONTCHAR_w=87,
    FONTCHAR_x=88,
    FONTCHAR_y=89,
    FONTCHAR_z=90,
    FONTCHAR_LEFT_KEY=91,
    FONTCHAR_VERTICAL_BAR=92,
    FONTCHAR_RIGHT_KEY=93,
    FONTCHAR_TILDE=94,
    FONTCHAR_LIMIT


};

//Varaint types of Fonts
enum FONT_VARIANT_TYPES:uint8_t{
   FONT_VARIANT_NORMAL=0,
   FONT_VARIANT_BOLD=1,
   FONT_VARIANT_ITALIC=2,
   FONT_VARIANT_SHADOW=3,
   FONT_VARIANT_BORDERS=4,
   FONT_MAX_VARIANT
};


//Data for Fonts Customized
struct customFont_Data{
   uint8_t id_ascii;
   uint16_t id_letter;
   uint16_t pos_x;
   uint16_t pos_y;

};


//Rect a Font Character On Texture
struct Glyph{
   uint16_t x;
   uint16_t y;
   uint16_t w;
   uint16_t h;
};


//Data of a Rendered Text
struct Rendered_Text{
   SDL_Texture* texture_text;
   uint16_t width;
   uint16_t height;
};

//Config Params of a Font
struct Font_params{
   std::vector<bool>exist_variant;
   std::vector<uint16_t> offsetX_variants;
   std::vector<uint16_t>offsetY_variants;
   std::vector<customFont_Data> custom_Font_dat;
   uint8_t width_chars;
   uint8_t height_chars;
   uint8_t separation_letters;
   uint8_t margin_texture;
   uint8_t columns_font;
   uint8_t rows_font;

};


//Store the Data of a Font from a Image
struct Bitmap_Font{

  public:
       void load_font(const char* src,uint32_t hash_name,Font_params params,SDL_Renderer* target_render);
       SDL_Texture* get_texture();
       Glyph* get_letter(uint8_t letter_id,uint8_t variant_id=0);
       Glyph* get_letter(char c,uint8_t variant_id=0);
       uint32_t get_hash();
       uint16_t get_expected_width(std::string str);

  private:
      void set_glyphs(Font_params params,uint16_t w_texture,uint16_t h_texture);
      void set_glyphs_Customized(Font_params params,uint16_t w_texture,uint16_t h_texture);
      std::vector<Glyph> letters_list;
      SDL_Texture* font_texture;
      uint32_t hash_id;



};

//System for Manage the Fonts
struct Fonts_System{
  public:
    void reserve_memory(uint8_t num_fonts);
    void free_System();
    void add_font(const char* src,uint32_t hash_name,Font_params params,SDL_Renderer* target_render);
    uint8_t get_FontId(uint32_t hash_name);
    Bitmap_Font* get_font(uint8_t id);
    uint8_t save_text(SDL_Texture* tex, uint16_t w,uint16_t h);
    Rendered_Text* get_Rendered_Text(uint8_t index);

  private:
    std::vector<Bitmap_Font>list_fonts;
    std::vector<Rendered_Text> renderedTexts_catalog;
};

//requerid for wrap text
struct text_token{
   std::string text;
   bool is_space;
};

//support function for wrap texts
std::string wrap_text(std::string text,uint16_t max_width,Bitmap_Font* font);


#endif // FONTS_H
