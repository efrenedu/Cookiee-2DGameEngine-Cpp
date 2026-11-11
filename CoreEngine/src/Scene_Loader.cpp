#include "Scene_Loader.h"
#include<iostream>
#include<fstream>
#include<Scene.h>

//Load Scene Frome Tiled TMX File
bool Scene_Loader::LoadSceneTiledXML(const char* filepath,SDL_Renderer* render_win,Scene* target_scene,float& w_mapPixels,float& h_mapPixels){
    pugi::xml_document doc;

    // Carga el archivo directamente desde disco a la memoria
    pugi::xml_parse_result result = doc.load_file(filepath);
    if (!result) {
        std::cout << "Error al cargar el XML: " << result.description() << endl;
        return false;
    }

    // Nodo raíz: <Scene>
    std::map<uint8_t,uint16_t> comps_count;
    pugi::xml_node sceneNode = doc.child("map");
    uint16_t entities_count=0;
    uint8_t tileset_count=0;
    uint8_t layers_tilemap=0;
    uint8_t num_physic_materials=0;
    const char* entity_node_name="object";
    const char* entity_layer_name="objectgroup";
    const char* tiles_layer_name="layer";
    uint16_t w_tile=sceneNode.attribute("tilewidth").as_uint();
    uint16_t h_tile=sceneNode.attribute("tileheight").as_uint();
    uint32_t w_map=sceneNode.attribute("width").as_uint();
    uint32_t h_map=sceneNode.attribute("height").as_uint();
    w_mapPixels=w_map*w_tile;
    h_mapPixels=h_map*h_tile;
    uint8_t num_sounds=0;
    uint8_t num_musics=0;
    uint8_t num_fonts=0;
    std::unordered_map<uint32_t,uint16_t> hashEntities;
    //Read of Number of Components and Number of Objects
    for(pugi::xml_node tileset_node=sceneNode.child("tileset"); tileset_node; tileset_node = tileset_node.next_sibling("tileset")) {
        tileset_count++;
    }
    for(pugi::xml_node layer_map=sceneNode.child(tiles_layer_name); layer_map; layer_map = layer_map.next_sibling(tiles_layer_name)) {
        layers_tilemap++;
    }
    for(pugi::xml_node layer_entity=sceneNode.child(entity_layer_name); layer_entity; layer_entity = layer_entity.next_sibling(entity_layer_name)) {
        for (pugi::xml_node entityNode = layer_entity.child(entity_node_name); entityNode; entityNode = entityNode.next_sibling(entity_node_name)) {

            pugi::xml_attribute type_attr=entityNode.attribute("type");
            pugi::xml_attribute name_attr=entityNode.attribute("name");

            bool not_entity=false;
            if(type_attr){
                uint32_t hash_typeAttr=hashString(type_attr.as_string());
                switch(hash_typeAttr){
                   case hashString("music"):not_entity=true;num_musics++;break;
                   case hashString("audio"):not_entity=true;num_sounds++;break;
                   case hashString("physic_material"):not_entity=true;num_physic_materials++;break;
                   case hashString("camera"):not_entity=true;num_fonts++;break;
                   case hashString("font"):not_entity=true;break;
                   default: break;
                }
            }
            if(not_entity){
                 continue;
            }
            uint32_t hash_name=hashString(name_attr.as_string());
            hashEntities[hash_name]=entities_count;
            entities_count++;
            comps_count[TRANSFORM_COMP]+=1;
            pugi::xml_node properties_node=entityNode.child("properties");
            for(pugi::xml_node property_node=properties_node.child("property");property_node;property_node=property_node.next_sibling("property")){
                const char* property_id=property_node.attribute("name").as_string();
                bool valid=false;
                uint8_t type_comp=ComponentName_to_ComponentIndex(property_id);
                if(type_comp==UKNOW_COMP){
                    continue;
                }
                auto it=comps_count.find(type_comp);
                if(it!=comps_count.end()){
                    comps_count[type_comp]+=1;
                }
                else{
                    comps_count[type_comp]=1;
                }
            }
        }
    }

    target_scene->reserve_memory(entities_count,comps_count);
    System_TileMap* tilemap_sys=target_scene->get_system_tilemap();
    textureManager_system* tex_sys=target_scene->get_texture_system();
    Audio_System* audio_sys=target_scene->get_Audio_sys();
    Physic_Material_System* physic_sys=target_scene->get_physic_material_sys();
    Fonts_System* font_sys=target_scene->get_font_sys();

    tilemap_sys->reserve_memory(layers_tilemap,w_map,h_map,w_tile);
    tex_sys->reserve_memory(tileset_count);
    audio_sys->reserve_memory(num_sounds,num_musics);
    physic_sys->reserve_memory(num_physic_materials);
    font_sys->reserve_memory(num_fonts);

    //Read Tilesets
    for(pugi::xml_node tileset_node=sceneNode.child("tileset"); tileset_node; tileset_node = tileset_node.next_sibling("tileset")) {


       string src_temp=tileset_node.attribute("source").as_string();
       src_temp=src_temp.substr(src_temp.find_last_of("/\\") + 1);
       string src="assets\\tilesets\\"+src_temp;
       uint16_t first_gid=tileset_node.attribute("firstgid").as_uint(16);
       tex_sys->add_fromTileset(src.c_str(),render_win,first_gid);
    }
    //Read TileMap
    uint8_t layer_actual=0;
    for(pugi::xml_node layer_map=sceneNode.child(tiles_layer_name); layer_map; layer_map = layer_map.next_sibling(tiles_layer_name)) {
       pugi::xml_node data_node=layer_map.child("data");
       if(data_node){
            string data=data_node.text().as_string();
            vector<string>list_indexs=Converter::str_toVect(data);
            for(uint32_t i=0;i<list_indexs.size();i++){
                uint16_t val_num=Converter::strTo_Int(list_indexs[i]);
                tilemap_sys->set_tile(layer_actual,i,val_num);
            }
            layer_actual++;
       }
    }
    //Read Entities
    uint16_t index_entity=0;
    for(pugi::xml_node layer_entity=sceneNode.child(entity_layer_name); layer_entity; layer_entity = layer_entity.next_sibling(entity_layer_name)) {
        for (pugi::xml_node entityNode = layer_entity.child(entity_node_name); entityNode; entityNode = entityNode.next_sibling(entity_node_name)) {
            std::bitset<64> mask = 0;
            Entity_Props props;
            bool not_entity=false;
            //set Transform
            props.set_value("width",entityNode.attribute("width").as_string());
            props.set_value("height",entityNode.attribute("height").as_string());
            bool has_texid=false;
            bool is_ui=false;
            bool initial_visible=true;
            Add_ComponentBit(mask,TRANSFORM_COMP);
            props.set_value("Ref_position_y",to_string(entityNode.attribute("y").as_float()).c_str());
            props.set_value("Ref_position_x",to_string(entityNode.attribute("x").as_float()).c_str());

            props.set_value("position_x",entityNode.attribute("x").as_string());
            if(pugi::xml_attribute gid_attr=entityNode.attribute("gid")){
               has_texid=true;
               float h_val=entityNode.attribute("height").as_float();
               float w_val=entityNode.attribute("width").as_float();
               float temp_y=(entityNode.attribute("y").as_float()-h_val)*-1;
               temp_y-=(h_val*0.5);
               float temp_x=entityNode.attribute("x").as_float()+(w_val*0.5);
               props.set_value("position_y",to_string(temp_y).c_str());
               props.set_value("position_x",to_string(temp_x).c_str());
               props.set_value("visible","true");
               props.set_value("gid",entityNode.attribute("gid").as_string());

            }
            else{
                float h_val=entityNode.attribute("height").as_float();
                float w_val=entityNode.attribute("width").as_float();
                float temp_y=entityNode.attribute("y").as_float()*-1;
                temp_y-=(h_val*0.5);
                float temp_x=entityNode.attribute("x").as_float()+(w_val*0.5);

                props.set_value("position_y",to_string(temp_y).c_str());
                props.set_value("position_x",to_string(temp_x).c_str());

                props.set_value("visible","false");

            }
            string name_entity="object"+to_string(index_entity);
            string tag_entity="default";
            props.set_value("rotation","0");
            props.set_value("scale","1");
            props.set_value("flip","false");

            if(pugi::xml_attribute rot_attr=entityNode.attribute("rotation")){
               props.set_value("rotation",entityNode.attribute("rotation").as_string());
            }
            if(pugi::xml_attribute visible_attr=entityNode.attribute("visible")){
               initial_visible=Converter::strTo_Boolean(entityNode.attribute("visible").as_string());
               props.set_value("visible",entityNode.attribute("visible").as_string());
            }
            if(pugi::xml_attribute name_attr=entityNode.attribute("name")){
               name_entity=entityNode.attribute("name").as_string();

            }

            uint32_t hash_name=hashString(name_entity.c_str());
            props.set_value("name",to_string(hash_name));

            uint32_t hash_tag=0;
            if(pugi::xml_attribute tag_attr=entityNode.attribute("type")){
               tag_entity=entityNode.attribute("type").as_string();
               hash_tag=hashString(tag_entity.c_str());

            }
            props.set_value("tag",to_string(hash_tag));

            //set others components

            switch(hash_tag){
                   case "camera"_hash:{
                       Camera2D cam;
                       float x_cam=props.get_float("position_x",0.0);
                       float y_cam=props.get_float("position_y",0.0f);
                       uint16_t w_view=400;
                       uint16_t h_view=400;
                       pugi::xml_node properties_nodes=entityNode.child("properties");
                       for(pugi::xml_node target_node=properties_nodes.child("property");target_node;target_node=target_node.next_sibling("property")){
                          string name_attr=target_node.attribute("name").as_string();
                          props.set_value(name_attr,target_node.attribute("value").as_string());
                       }
                       w_view=props.get_uint("ViewPort_Width",400);
                       h_view=props.get_uint("ViewPort_Height",400);
                       uint16_t target_followId=0xFFFF;
                       float offset_target_x=props.get_float("Offset_TargetX",0.0);
                       float offset_target_y=props.get_float("Offset_TargetY",0.0);
                       uint32_t hash_id=hashString(props.get_string("Target_Follow","").c_str());
                       auto it=hashEntities.find(hash_id);
                       if(it!=hashEntities.end()){
                           target_followId=hashEntities[hash_id];
                       }
                       cam.Init(x_cam,y_cam,w_view,h_view,target_followId,offset_target_x,offset_target_y);
                       target_scene->set_camera(cam);
                       not_entity=true;
                       break;
                   }
                   case "music"_hash:{
                       pugi::xml_node properties_nodes=entityNode.child("properties");
                       string src="";
                       for(pugi::xml_node target_node=properties_nodes.child("property");target_node;target_node=target_node.next_sibling("property")){
                          string name_attr=target_node.attribute("name").as_string();
                          props.set_value(name_attr,target_node.attribute("value").as_string());
                       }
                       src=props.get_string("src_sound","");
                       uint32_t hash_name=hashString(entityNode.attribute("name").as_string());
                       src="assets\\musics\\"+src;
                       audio_sys->add_clip(src.c_str(),true,hash_name);
                       not_entity=true;
                       break;
                    }
                   case "audio"_hash:{
                       pugi::xml_node properties_nodes=entityNode.child("properties");
                       string src="";
                       for(pugi::xml_node target_node=properties_nodes.child("property");target_node;target_node=target_node.next_sibling("property")){
                          string name_attr=target_node.attribute("name").as_string();
                          props.set_value(name_attr,target_node.attribute("value").as_string());
                       }
                       src=props.get_string("src_sound","");
                       uint32_t hash_name=hashString(entityNode.attribute("name").as_string());
                       src="assets\\sfx\\"+src;
                       audio_sys->add_clip(src.c_str(),false,hash_name);
                       not_entity=true;
                       break;
                    }
                   case "physic_material"_hash:{
                      uint32_t hash_name=hashString(entityNode.attribute("name").as_string());
                      pugi::xml_node properties_nodes=entityNode.child("properties");
                      for(pugi::xml_node target_node=properties_nodes.child("property");target_node;target_node=target_node.next_sibling("property")){
                          string name_attr=target_node.attribute("name").as_string();
                          props.set_value(name_attr,target_node.attribute("value").as_string());
                       }
                       float friction=props.get_float("friction",0.0);
                       float dinamic_friction=props.get_float("dinamic_friction",0.0);
                       float retitution=props.get_float("retitution",0.0);
                       physic_sys->register_material(friction,dinamic_friction,retitution,hash_name);
                       not_entity=true;
                       break;
                   }
                   case "font"_hash:{
                      uint32_t hash_name=hashString(entityNode.attribute("name").as_string());
                      pugi::xml_node properties_nodes=entityNode.child("properties");
                      for(pugi::xml_node target_node=properties_nodes.child("property");target_node;target_node=target_node.next_sibling("property")){
                          string name_attr=target_node.attribute("name").as_string();
                          props.set_value(name_attr,target_node.attribute("value").as_string());
                      }
                      std::string src=props.get_string("src_font","");
                      src="assets\\fonts\\"+src;
                      Font_params params;
                      params.width_chars=props.get_uint("width_characters",0);
                      params.height_chars=props.get_uint("height_characters",0);
                      params.margin_texture=props.get_uint("margin",0);
                      params.separation_letters=props.get_uint("separation",0);
                      params.columns_font=props.get_uint("column_count",0);
                      params.rows_font=props.get_uint("row_count",0);
                      std::vector<std::string> variantsOffset_xStr=Converter::str_toVect(props.get_string("offset_variantsX",""));
                      std::vector<std::string> variantsOffset_yStr=Converter::str_toVect(props.get_string("offset_variantsy",""));
                      std::vector<std::string> variants_avavailables_Str=Converter::str_toVect(props.get_string("available_variants",""));

                      std::vector<uint16_t> variants_offsetx;
                      std::vector<uint16_t> variants_offsety;
                      std::vector<bool> available_variants;

                      variants_offsetx.assign(FONT_MAX_VARIANT,0);
                      variants_offsety.assign(FONT_MAX_VARIANT,0);
                      available_variants.assign(FONT_MAX_VARIANT,false);
                      for(uint8_t i=0;i<FONT_MAX_VARIANT;i++){
                           if(i<variantsOffset_xStr.size()){
                               variants_offsetx[i]=Converter::strTo_UInt(variantsOffset_xStr[i]);
                           }
                           if(i<variantsOffset_yStr.size()){
                               variants_offsety[i]=Converter::strTo_UInt(variantsOffset_yStr[i]);
                           }
                           if(i<variants_avavailables_Str.size()){
                               available_variants[i]=Converter::strTo_Boolean(variants_avavailables_Str[i]);
                           }
                      }
                      if(available_variants[0]==false){
                         available_variants[0]=true;

                      }
                      params.offsetX_variants=variants_offsetx;
                      params.offsetY_variants=variants_offsety;
                      params.exist_variant=available_variants;
                      std::string customDat_src=props.get_string("font_custom_data","");

                      font_sys->add_font(src.c_str(),hash_name,params,render_win);
                      not_entity=true;
                      break;
                   }
                   case "UI"_hash:is_ui=true;break;
                   default:break;
            }
            if(not_entity==true){
                continue;
            }
            if(is_ui){
                 props.set_value("position_x",to_string(props.get_float("Ref_position_x",0)));
                 props.set_value("position_y",to_string(props.get_float("Ref_position_y",0)));
                 props.set_value("visible",to_string(initial_visible));
                 Add_ComponentBit(mask,RENDER_COMP);
            }
            pugi::xml_node properties_node=entityNode.child("properties");
            for(pugi::xml_node property_node=properties_node.child("property");property_node;property_node=property_node.next_sibling("property")){
                const char* property_id=property_node.attribute("name").as_string();
                bool valid=false;
                uint8_t type_comp=ComponentName_to_ComponentIndex(property_id);
                components_types type_c=static_cast<components_types>(type_comp);
                if(type_c!=UKNOW_COMP){
                    Add_ComponentBit(mask,type_c);
                    continue;
                }
                props.set_value(property_id,property_node.attribute("value").as_string());
            }
            reassign_StringParams(props,physic_sys,font_sys,hashEntities);
            target_scene->register_entity(index_entity, mask, props);
            index_entity++;
        }
    }


    return true;

}

//Reassign Expected Text Params from Tiled Scenes to the Correct Format
void Scene_Loader::reassign_StringParams(Entity_Props& props,Physic_Material_System* physic_sys,Fonts_System* font_sys,std::unordered_map<uint32_t ,uint16_t>hash_names){
   std::string bd_type_str=props.get_string("body_type","");
   if(!bd_type_str.empty()){
      uint8_t bd_type=get_body_type(bd_type_str.c_str());
      props.set_value("body_type",to_string(bd_type));
   }
   uint8_t joint_count=props.get_uint("Joint_Count",0);
   //Reassign Joint Targets
   for(uint8_t i=0;i<joint_count;i++){
        std::string index_str=to_string(static_cast<int>(i));
        std::string val_str=props.get_string("Joint_Target_"+index_str,"");
        if(val_str.empty()){
            continue;
        }
        uint32_t hash_target=hashString(val_str.c_str());
        auto it=hash_names.find(hash_target);
        if(it!=hash_names.end()){
            props.set_value("Joint_Target_"+index_str,to_string(hash_names[hash_target]));
        }
        else{
           props.set_value("Joint_Target_"+index_str,to_string(0xFFFF));
        }
   }
   std::string mat_str=props.get_string("physic_material","");
   if(!mat_str.empty()){
       int8_t id=physic_sys->get_material_id(hashString(mat_str.c_str()));
       props.set_value("physic_material",to_string(id));
   }
   //Convert Additional Fixtures of Collider
   uint8_t count_addiontalFixtures=props.get_uint("Count_Add_Fixtures",0);
   for(uint8_t i=0;i<count_addiontalFixtures;i++){
        std::string index_str=to_string(static_cast<uint16_t>(i));
        std::string mask_fixture_str=props.get_string("Fixture_Mask_"+index_str,"");
        if(mask_fixture_str.empty()){
            continue;
        }
        uint16_t new_mask= get_LayerCollider_mask(mask_fixture_str.c_str());
        props.set_value("Fixture_Mask_"+index_str,to_string(new_mask));
        std::vector<std::string> bit_fixture_list=Converter::str_toVect(props.get_string("Fixture_Category_Bits_"+index_str,""));
        if(bit_fixture_list.size()>0){
            std::string new_category=to_string(get_LayerCollider_mask(bit_fixture_list[0].c_str()));
            for(uint8_t j=1;j<bit_fixture_list.size();j++){
                new_category=new_category+","+to_string(get_LayerCollider_mask(bit_fixture_list[j].c_str()));
            }
            props.set_value("Fixture_Category_Bits_"+index_str,new_category);
        }


    }
    //Figure and Collider Mask Conversion
    uint8_t fig=get_figure_type(props.get_string("figure_type","").c_str());
    props.set_value("figure_type",to_string(fig));
    uint16_t mask_collider=get_LayerCollider_mask(props.get_string("collider_mask","").c_str());
    props.set_value("collider_mask",to_string(mask_collider));
    std::vector<std::string> bits_collider=Converter::str_toVect(props.get_string("collider_categoryBits",""));
    if(bits_collider.size()>0){
        std::string new_category=to_string(get_LayerCollider_mask(bits_collider[0].c_str()));
        for(uint8_t i=1;i<bits_collider.size();i++){
            new_category=new_category+","+to_string(get_LayerCollider_mask(bits_collider[i].c_str()));
        }
        props.set_value("collider_categoryBits",new_category);
    }
    std::string font_name=props.get_string("Font_Id","");
    if(!font_name.empty()){
          uint32_t hash_font=hashString(font_name.c_str());
          uint8_t font_id=font_sys->get_FontId(hash_font);
          props.set_value("Font_Id",to_string(font_id));
    }

}


//Load Scene Data From a XML File
bool Scene_Loader::LoadSceneXML(const char* filePath,Scene* target_scene,const char* root_name,const char* entity_node_name,const char* component_node_name,const char* header_name) {
    pugi::xml_document doc;

    // Carga el archivo directamente desde disco a la memoria
    pugi::xml_parse_result result = doc.load_file(filePath);
    if (!result) {
        std::cout << "Error al cargar el XML: " << result.description() << endl;
        return false;
    }

    // Nodo raíz: <Scene>
    map<uint8_t,uint16_t> comps_count;
    auto comps_info=GetComponentKeysNames();
    pugi::xml_node sceneNode = doc.child(root_name);
    uint16_t entities_count=0;

    //Read of Number of Components and Number of Objects
    pugi::xml_node comps_count_node=sceneNode.child(header_name);
    if(comps_count_node){
        for(const char* target_key: comps_info){
             uint8_t target_index=ComponentName_to_ComponentIndex(target_key);
             if(target_index==UKNOW_COMP){continue;}
             pugi::xml_attribute res_attr=comps_count_node.attribute(target_key);
             if(res_attr){
                comps_count[target_index]=res_attr.as_uint();
             }

        }
        pugi::xml_attribute res_entity_count=comps_count_node.attribute("Entity_Count");
        if(res_entity_count){
           entities_count=res_entity_count.as_uint();
        }
    }
    else{

          for (pugi::xml_node entityNode = sceneNode.child(entity_node_name); entityNode; entityNode = entityNode.next_sibling(entity_node_name)) {
              entities_count++;
              for(const char* target_key: comps_info){
                    uint8_t target_index=ComponentName_to_ComponentIndex(target_key);
                    if(target_index==UKNOW_COMP){continue;}

                     pugi::xml_attribute res_attr=entityNode.attribute(target_key);
                     if(res_attr){
                           auto it=comps_count.find(target_index);
                           if(it!=comps_count.end()){
                               comps_count[target_index]+=1;
                           }
                           else{
                              comps_count[target_index]=1;
                           }
                     }
              }

          }
     }
    target_scene->reserve_memory(entities_count,comps_count);
    // Iteración directa por punteros (sin copias de strings en el bucle)
    for (pugi::xml_node entityNode = sceneNode.child(entity_node_name); entityNode; entityNode = entityNode.next_sibling(entity_node_name)) {

        uint32_t entityID = entityNode.attribute("id").as_uint();
        std::bitset<64> mask; // Iniciar máscara limpia en 0
        Entity_Props props;

        // Leer componentes dentro de la entidad
        for (pugi::xml_node compNode = entityNode.child(component_node_name); compNode; compNode = compNode.next_sibling(component_node_name)) {

            const char*  compType = compNode.attribute("type").value();
            uint8_t type_val=ComponentName_to_ComponentIndex(compType);
            components_types tp=static_cast<components_types>(type_val);
            if(tp==UKNOW_COMP){
                continue;
            }
            Add_ComponentBit(mask,tp);
            size_t count_target=0;
            /*const char* const* props_names=world.GetComponentProps(type_val,count_target);
            for(size_t i=0;i<count_target;++i){
                const char* attr_name=props_names[i];
                pugi::xml_attribute attr=compNode.attribute(attr_name);
                if(attr){
                    props.set_value(attr_name,attr.as_string());
                }
            }*/

        }

        target_scene->register_entity(entityID, mask, props);
    }
    return true;
}

//Load Scene Data from a JSON File
bool Scene_Loader::LoadSceneJSON(const char* filePath, Scene* target_scene,const char* root_name,const char* entity_node_name,const char* component_node_name,const char* header_name){
   std::ifstream file(filePath);
   if(!file.is_open()){
    std::cout << "Error al cargar el JSON: "<<filePath<< endl;
     return false;
   }
   nlohmann::json json_dat;
   file>>json_dat;
   map<uint8_t,uint16_t> comps_count;
   auto comps_info=GetComponentKeysNames();
   uint16_t entities_count=0;
   if((json_dat.contains("Entities") && json_dat["Entities"].is_array())==false){
      return false;

   }
   entities_count=json_dat["Entities"].size();
   if(json_dat.contains(header_name)){
       for(const char* target_key: comps_info){

          if(json_dat.contains(target_key)){
              uint8_t index_comp=ComponentName_to_ComponentIndex(target_key);
              if(index_comp==UKNOW_COMP){
                  continue;
              }
              comps_count[index_comp]=json_dat[target_key];
          }
       }
   }
   else{


   }
   uint32_t entityID = 0;
   ComponentMask mask = 0; // Iniciar máscara limpia en 0
   Entity_Props props;
   if((json_dat.contains("Entities") && json_dat["Entities"].is_array())==false){
      return false;

   }
   const auto actors_list=json_dat["Actors"];
   for (const auto& element:actors_list){
      int32_t id=element.value("id",0);
      cout<<id<<endl;
   }
   return true;
}
