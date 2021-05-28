
#ifndef USO_MENU_OBJECTS_H
#define USO_MENU_OBJECTS_H
#include "SFML/Graphics.hpp"
#include "map_management.h"
#include "windows.h"
namespace Menu {
enum Event { OPEN_AIM, OPEN_CONVEYOR, OPEN_SETTINGS, OPEN_CONSTRUCTOR, EXIT, RETRY, CONTINUE, BACK_TO_MENU };
enum State { POINTED, QUIET };
struct Button {
    sf::CircleShape circle;
    Event event;
    State state = State::QUIET;


    Button(float x,
           float y,
           float r,
           Event event_,
           sf::Texture &img)
        : event(event_) {
        circle.setTexture(&img);
        circle.setRadius(r);
        circle.setPosition(x, y);
        circle.setOutlineThickness(0.5);
        circle.setOutlineColor(sf::Color::Magenta);

    }

    bool is_circle_correct_click(const sf::Vector2f &mouse);
    void press(sf::RenderWindow &window, const sf::Vector2f &mouse, BL::Game_session &);
    void guidance(const sf::Vector2f &mouse);
    void draw(sf::RenderWindow &window);
};

}  // namespace Menu
#endif  // USO_MENU_OBJECTS_H
