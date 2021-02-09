#include "map_objects.h"

void USO::Aim_circle::change_state() {
    active_circle_radius -= active_circle_radius_shift;
}

bool USO::Aim_circle::check_event() {
    /*
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed)
            window.close();
        if (event.type == Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                position_mouse = Mouse::getPosition();
            }
        }
    }
     */
    return true;
}

void USO::Aim_circle::draw(sf::RenderWindow window) {
    sf::CircleShape circle(active_circle_radius);
    circle.setPosition(x_pos, y_pos);
    circle.setFillColor(sf::Color(230, 155, 230));
    window.draw(circle);
}