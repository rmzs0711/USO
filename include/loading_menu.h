#ifndef USO_LOADING_MENU_H
#define USO_LOADING_MENU_H

#include <string>
#include "SFML/Graphics.hpp"
#include "menu.h"

struct Game {
    std::string map_name{};
    sf::Time past_time{};
    sf::Vector3f music_pos;

    void play(sf::RenderWindow &) const;
};

enum Action {
    OPEN_MAIN_MENU,
    OPEN_STOP_MENU,
    OPEN_STOP_MENU_PAUSE,
    OPEN_SCROLLING_MENU,
    OPEN_CREATION_MENU,
    OPEN_MOD_MENU,
    OPEN_GAME_MAP,
    CLOSE_THE_WINDOW,
    NOTHING
};

struct menu_control {
    float coef;
    Action action;
    sf::RenderWindow window;
    sf::Vector2f WINDOW_SIZE;
    sf::Vector2f WINDOW_POSITION;
    Game game;

    Menu::stop_menu stopMenuPause;
    Menu::stop_menu stopMenuVictory;
    Menu::main_menu mainMenu;
    Menu::scrolling_menu scrollingMenu;
    Menu::map_creation_menu mapCreationMenu;

    void run();
    explicit menu_control();
};

#endif //USO_LOADING_MENU_H
