#include "map_master.h"
#include <memory>
#include <iostream>

namespace USO {
void Field::push(
    std::vector<std::shared_ptr<Map_object>>::iterator &map_object_it,
    sf::Time current_time) {
    /* Отступ съехал. :( */
if ((*(*map_object_it)).get_start_time() <= current_time) {
        field_objects.push_front(*map_object_it);
        map_object_it++;
    }
}

void Map_choice_menu::run_map_choice_menu() {
    //менюшка с картами
}

std::list<std::shared_ptr<Map_object>> &Field::get_field_objects() {
    return field_objects;
}
}  // namespace USO
