//
// Created by bon52 on 19.08.2021.
//

#ifndef USO_LOADING_MENU_H
#define USO_LOADING_MENU_H

#include "menu.h"

struct menu_object {
    sf::RenderWindow window;

    sf::Vector2f WINDOW_SIZE;
    sf::Vector2f WINDOW_POSITION;

    Menu::stop_menu stopMenuPause;
    Menu::stop_menu stopMenuVictory;
    Menu::main_menu mainMenu;
    Menu::scrolling_menu scrollingMenu;
    Menu::map_creation_menu mapCreationMenu;

    void run();
    explicit menu_object(int);
};

#endif //USO_LOADING_MENU_H
