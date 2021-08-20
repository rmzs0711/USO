#include <iostream>
#include "loading_menu.h"

menu_control::menu_control() try :
      window(sf::VideoMode(1920, 1080),
             "USO!", sf::Style::Fullscreen),
      WINDOW_SIZE(window.getSize()), WINDOW_POSITION(window.getPosition()),
      stopMenuPause(BL::Game_status::PAUSE),
      stopMenuVictory(BL::Game_status::VICTORY),
      mainMenu(),
      scrollingMenu(R"(data\maps\saved_maps.txt)"),
      mapCreationMenu(R"(data\maps\saved_maps.txt)") {
    action = Action::OPEN_MAIN_MENU;
    } catch (...) {
        std::cerr << "Broken menu controller constructor";
    }

void menu_control::run() {
    BL::Game_session gameSession;
    while (window.isOpen()) {
        switch (action) {
            case Action::OPEN_MAIN_MENU: {
                mainMenu.run(window, gameSession);
            } break;
            case Action::OPEN_STOP_MENU: {
                stopMenuVictory.run(window, gameSession);
            } break;
            case Action::OPEN_SCROLLING_MENU: {
                scrollingMenu.run(window);
            } break;
            case Action::OPEN_CREATION_MENU: {
                //mapCreationMenu.draw(window);
            } break;
            case Action::OPEN_MOD_MENU: {
                // run(window, gameSession);   //  todo
            } break;
            case Action::NOTHING: {
                continue;
            } break;
            case Action::CLOSE_THE_WINDOW: {
                window.close();
                return;
            } break;
        }
    }
    //mainMenu.run(window, gameSession);
}
