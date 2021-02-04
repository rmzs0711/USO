#include <cassert>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "map_master.h"
#include "maps.h"

void USO::Aim_map::run(sf::RenderWindow &window) {
    BL::Game_session gameSession;
    USO::Field field;
    const unsigned HEIGHT = sf::VideoMode::getFullscreenModes().front().height;
    const unsigned WIDTH = sf::VideoMode::getFullscreenModes().front().width;
    assert(gameSession.get_game_status() == BL::Game_status::ACTION);

    while (gameSession.get_game_status() != BL::Game_status::VICTORY ||
           gameSession.get_game_status() != BL::Game_status::DEFEAT) {
        switch (gameSession.get_game_status()) {
            case BL::Game_status::ACTION: {

                // TODO field.push_back() circles from map
                sf::Event event{};
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::KeyPressed) {
                        if (event.key.code == sf::Keyboard::Escape) {
                            gameSession.pause_session();
                        }
                    }
                    if (event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            // TODO make_click or something
                        }
                    }
                }
                break;
            }

            case BL::Game_status::PAUSE: {
                break;
            }

            default:
                continue;
        }
        for (auto &circle : field.circles) {
            //            TODO: window.draw(circle);
        }
        window.display();
    }
}

