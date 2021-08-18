#ifndef USO_MENU_OBJECTS_H
#define USO_MENU_OBJECTS_H

#include "SFML/Graphics.hpp"
#include "map_objects_management.h"
#include "windows.h"

namespace Menu {
enum Event {
    CHOOSE_THE_MAP,
    CREATE_NEW_MAP,
    OPEN_LIST_OF_MODS,
    EXIT,
    RETRY,
    CONTINUE,
    BACK_TO_MENU
};

enum State {
    POINTED,
    QUIET
};

struct Button {
    sf::CircleShape circle;
    sf::Text text;
    Event event;
    State state = State::QUIET;

    Button(float x, float y, float r, Event event_, sf::Texture &img)
        : event(event_) {
        text.setCharacterSize(int(float(sf::VideoMode::getFullscreenModes().begin()->width) / 60.f)),
        circle.setTexture(&img);
        circle.setRadius(r);
        circle.setPosition(x, y);
        circle.setOutlineThickness(0.5);
        circle.setOutlineColor(sf::Color::Magenta);
    }

    void changeSize(float coef, float win_size_x);
    void changePosition(float coef, sf::Vector2f WINDOW_POSITION, sf::Vector2f pos = {0, 0});

    bool is_circle_correct_click(const sf::Vector2f &mouse);
    void press(sf::RenderWindow &window,
               const sf::Vector2f &mouse,
               BL::Game_session &);
    void guidance(const sf::Vector2f &mouse);
    void draw(sf::RenderWindow &window);
};

}  // namespace Menu
#endif  // USO_MENU_OBJECTS_H
