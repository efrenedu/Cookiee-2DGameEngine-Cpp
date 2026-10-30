#include "UI_System.h"

//Reser Memory for the System
void UI_System::Reserve_Memory(std::vector<uint16_t> ui_components_count,uint16_t entities_count){
  Entity_toDense.reserve(entities_count);
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

}


