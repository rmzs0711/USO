//
// Created by Aigerim on 15.04.2021.
//

#ifndef USO_MENU_H
#define USO_MENU_H
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include <deque>
#include <string>
#include <cstddef>

namespace Menu {

const std::size_t MAX_SIZE = 6;

struct scrolling_menu {
private:
    std::deque<std::string> list_of_maps;
    std::vector<sf::RectangleShape> blocks;
public:
    explicit scrolling_menu(const std::string &);

    void scroll_down();
    void scroll_up();
    void push(sf::Vector2f);
};
void menu(sf::RenderWindow &, BL::Game_session);
void constructor_menu(sf::Window& window);
sf::RenderWindow &set_settings();
void stop_menu(sf::RenderWindow &, BL::Game_session &);
void list_of_maps(sf::RenderWindow &);
}
#endif  // USO_MENU_H
