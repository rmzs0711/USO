#include <iostream>
#include "loading_menu.h"
#include "menu.h"
#include <fstream>
#include "maps.h"

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
    coef = 0;
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
                gameSession.set_game_status(BL::Game_status::VICTORY);
                stopMenuVictory.run(window, gameSession);
            } break;
            case OPEN_STOP_MENU_PAUSE: {
                gameSession.set_game_status(BL::Game_status::PAUSE);
                stopMenuPause.run(window, gameSession);
            } break;
            case Action::OPEN_SCROLLING_MENU: {
                scrollingMenu.run(window);
            } break;
            case Action::OPEN_CREATION_MENU: {
                mapCreationMenu.draw(window);
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
            case OPEN_GAME_MAP: {
                game.play(window);
            }  break;
        }
    }
}

void Game::play(sf::RenderWindow &window) const {
    std::ifstream file(R"(data\maps\)" + map_name + ".txt");
    std::string mod;
    std::getline(file, mod);
    if (mod == "Conveyor") {
        USO::Conveyor_map test(R"(data\maps\)" + map_name + ".txt");
        test.run(window);
    } else if (mod == "Aim") {
//        change_speed(R"(data\maps\)" + map_name +
//        ".txt", 1.f / acceleration_factor);
        USO::Aim_map test(R"(data\maps\)" + map_name + ".txt");
        test.run(window);
//        change_speed(R"(data\maps\)" + map_name +
//        ".txt", acceleration_factor);
    } else if (mod == "Taiko") {
        USO::taiko_map test(R"(data\maps\)" + map_name + ".txt");
        test.run(window);
    }
}
