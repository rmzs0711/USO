#include "map_objects.h"
#include "SFML/System/Time.hpp"

sf::Time &USO::Map_object::get_start_time() {
    return start_time;
}

sf::Time &USO::Map_object::get_duration_time() {
    return duration_time;
}

float &USO::Map_object::get_x_coord() {
    return x_pos;
}

float &USO::Map_object::get_y_coord() {
    return y_pos;
}