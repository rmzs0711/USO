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

void USO::Aim_circle::change_state() {
    active_circle_radius -= active_circle_shift;
}

bool USO::Aim_circle::check_event(float x, float y) {
    if ((x - x_pos) * (x - x_pos) +
            (y - y_pos) * (y - y_pos) <= beat_radius) {
        return true;
    }
    return false;
}

void USO::Aim_circle::draw(sf::RenderWindow window) {
    sf::CircleShape circle(active_circle_radius);
    circle.setPosition(x_pos, y_pos);
    circle.setFillColor(sf::Color(230, 155, 230));
    window.draw(circle);
}
