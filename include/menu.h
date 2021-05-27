//
// Created by Aigerim on 15.04.2021.
//

#ifndef USO_MENU_H
#define USO_MENU_H
#include "SFML/Graphics.hpp"
#include "base_logic.h"

namespace Menu {

void menu(sf::RenderWindow &, BL::Game_session);
void constructor_menu(sf::Window& window);
sf::RenderWindow &set_settings();
void stop_menu(sf::RenderWindow &, BL::Game_session &);
}
#endif  // USO_MENU_H
