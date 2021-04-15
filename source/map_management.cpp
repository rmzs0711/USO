#include "map_management.h"
#include <iostream>
#include <memory>

namespace USO {
void Field::push(
    std::vector<std::shared_ptr<Map_object>>::iterator &map_object_it,
    sf::Time current_time) {
    if ((*(*map_object_it)).get_start_time() <= current_time) {
        field_objects.push_front(*map_object_it);
        map_object_it++;
    }
}

void Field::draw(
    const sf::Font &font) {  //думаю логично, чтобы филд сам мог себя нарисовать
    for (auto &object_ptr : field_objects) {
        (*object_ptr).draw(window, font);
    }
}

void Field::change_state(const sf::Time & time) {
    for (std::shared_ptr<Map_object> &ptr : get_field_objects()) {
        (*ptr).change_state(time);
    }
}

void Map_choice_menu::run_map_choice_menu() {
    //менюшка с картами
}

std::list<std::shared_ptr<Map_object>> &Field::get_field_objects() {
    return field_objects;
}
}  // namespace USO