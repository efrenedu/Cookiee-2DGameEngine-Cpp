#ifndef FONTS_H
#define FONTS_H
#include<cstdint>
#include <vector>
#include<SDL.h>

//Numeric Index of Characters for Bit Map Fonts
enum AVAILABLES_LETTERS:uint8_t{
    FONTCHAR_A=0,
    FONTCHAR_B=1,
    FONTCHAR_C=2,
    FONTCHAR_D=3,
    FONTCHAR_E=4,
    FONTCHAR_F=5,
    FONTCHAR_G=6,
    FONTCHAR_H=7,
    FONTCHAR_I=8,
    FONTCHAR_J=9,
    FONTCHAR_K=10,
    FONTCHAR_L=11,
    FONTCHAR_M=12,
    FONTCHAR_N=13,
    FONTCHAR_O=14,
    FONTCHAR_P=15,
    FONTCHAR_Q=16,
    FONTCHAR_R=17,
    FONTCHAR_S=18,
    FONTCHAR_T=19,
    FONTCHAR_U=20,
    FONTCHAR_V=21,
    FONTCHAR_W=22,
    FONTCHAR_X=23,
    FONTCHAR_Y=24,
    FONTCHAR_Z=25,
    FONTCHAR_a=26,
    FONTCHAR_b=27,
    FONTCHAR_c=28,
    FONTCHAR_d=29,
    FONTCHAR_e=30,
    FONTCHAR_f=31,
    FONTCHAR_g=32,
    FONTCHAR_h=33,
    FONTCHAR_i=34,
    FONTCHAR_j=35,
    FONTCHAR_k=36,
    FONTCHAR_l=37,
    FONTCHAR_m=38,
    FONTCHAR_n=39,
    FONTCHAR_o=40,
    FONTCHAR_p=41,
    FONTCHAR_q=42,
    FONTCHAR_r=43,
    FONTCHAR_s=44,
    FONTCHAR_t=45,
    FONTCHAR_u=46,
    FONTCHAR_v=47,
    FONTCHAR_w=48,
    FONTCHAR_x=49,
    FONTCHAR_y=50,
    FONTCHAR_z=51,
    FONTCHAR_0=52,
    FONTCHAR_1=53,
    FONTCHAR_2=54,
    FONTCHAR_3=55,
    FONTCHAR_4=56,
    FONTCHAR_5=57,
    FONTCHAR_6=58,
    FONTCHAR_7=59,
    FONTCHAR_8=60,
    FONTCHAR_9=61,
    FONTCHAR_plus=62,
    FONTCHAR_minus=63,
    FONTCHAR_multiply=64,
    FONTCHAR_divide=65,
    FONTCHAR_dot=66,
    FONTCHAR_colon=67,
    FONTCHAR_DOLLAR=68,
    FONTCHAR_NUMERAL=69,
    FONTCHAR_PORCENT=70,
    FONTCHAR_AMPERSANG=71,
    FONTCHAR_OPEN_QUESTION=69,
    FONTCHAR_CLOSE_QUESTION=70,
    FONTCHAR_OPEN_ADMIRATION=71,
    FONTCHAR_CLOSE_ADMIRATION=72

};

//Rect and Id of a Font Character
struct Char_Font{
   float x;
   float y;
   float w;
   float h;
   uint32_t font_id;
};

//Store the Data of a Font from a Image
struct Bitmap_Font{

  public:
       void load_font(const char* src,uint32_t hash_name,uint8_t spacing,uint8_t separation);
       SDL_Texture* get_texture(uint8_t letter);
       Char_Font get_letter(uint8_t letter_id);
  private:
      std::vector<Char_Font> letters_list;



};

//System for Manage the Fonts
struct Fonts_System{
  public:
    void Init_System();
    void free_System();
    void add_font(const char* src,uint32_t hash_name);
  private:
    std::vector<Bitmap_Font>list_fonts;
};

#endif // FONTS_H
