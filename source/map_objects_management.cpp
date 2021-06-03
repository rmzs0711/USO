#include "map_objects_management.h"
#include <cassert>
#include <iostream>
#include <memory>

namespace USO {
void Field::push_front(
    std::list<std::shared_ptr<Map_object>>::iterator &map_object_it,
    sf::Time current_time) {

    (*map_object_it)->reset();
    if ((*(*map_object_it)).get_start_time() <= current_time) {
        field_objects.push_front(*map_object_it);
        map_object_it++;
    }
}

void Field::draw(
    const sf::Font &font) {
    for (auto &object_ptr : field_objects) {
        (*object_ptr).draw(window, font);
    }
}

void Field::change_state(const sf::Time &time) {
    for (std::shared_ptr<Map_object> &ptr : get_field_objects()) {
        (*ptr).change_state(time);
    }
}

std::list<std::shared_ptr<Map_object>> &Field::get_field_objects() {
    return field_objects;
}

}  // namespace USO