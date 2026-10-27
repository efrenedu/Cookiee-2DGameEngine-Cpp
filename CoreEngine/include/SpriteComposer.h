#ifndef SPRITECOMPOSER_H
#define SPRITECOMPOSER_H
#include<vector>
#include<cstdint>

struct SpriteComposer_Item{
   float offsetX;
   float offsetY;
   uint16_t id_sprite;
   uint8_t draw_mode;
};

struct SpriteComposer_Data{
   std::vector<SpriteComposer_Item> sprites;
};

struct SpriteComposer_System{
    public:
        void Reserve(uint16_t count_Components,uint16_t count_entitites,uint16_t fillval);
        void Free_Memory();
        void Regiser_Entity(uint16_t id,std::vector<SpriteComposer_Item> list_sprites);
        SpriteComposer_Data* get_spriteComposer(uint16_t id);
    private:
        std::vector<uint16_t>EntitytoDense;
        std::vector<SpriteComposer_Data> spritesComposer_list;
};

#endif // SPRITECOMPOSER_H
