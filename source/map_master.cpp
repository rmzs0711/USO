#include "map_master.h"
#include <memory>

void USO::Field::push(
    std::vector<std::shared_ptr<Map_object>>::iterator &map_object_it,
    sf::Time current_time) {
    if ((*(*map_object_it)).get_start_time() == current_time) {
        field_objects.push_back(*map_object_it);
    }
}

