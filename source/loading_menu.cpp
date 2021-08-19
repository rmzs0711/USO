//
// Created by bon52 on 19.08.2021.
//
#include "loading_menu.h"
#include "menu.h"
#include "base_logic.h"
#include <exception>

menu_object::menu_object(int) try :
      WINDOW_SIZE(1920, 1080), WINDOW_POSITION(0, 0),
      window(sf::VideoMode(1920, 1080), "USO!", sf::Style::Fullscreen),
      scrollingMenu(R"(data\maps\saved_maps.txt)"), mainMenu(), stopMenuPause(BL::Game_status::PAUSE),
      stopMenuVictory(BL::Game_status::VICTORY), mapCreationMenu(R"(data\maps\saved_maps.txt)") {
    } catch (...) {
        std::cerr << "Broken menu controller constructor";
    }

void menu_object::run() {

}
