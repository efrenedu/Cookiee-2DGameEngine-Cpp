#ifndef TRANSFORM_SYSTEM_H
#define TRANSFORM_SYSTEM_H
#include <vector>
#include<cstdint>

//Store the Transform Data
struct Transform_Data{
    uint16_t scale;
    float angle;
    float lastRadians;
    float accumlated_radians;
    float x;
    float y;
    bool flipped;
};

//System for Transform Component
struct Transform_System{
    public:
        void reserve_memory(int16_t num_elements);
        void free_system();
        static constexpr const char*props_names[]={"position_x","position_y","scale","rotation","flip"};
        void register_transform(float px,float py,int16_t t_scale,float angle,bool flip_val);
        Transform_Data* get_transform(uint16_t id);
    private:
        std::vector<Transform_Data> transform_list;

};

#endif // TRANSFORM_SYSTEM_H
