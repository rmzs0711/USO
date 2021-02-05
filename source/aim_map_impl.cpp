#include <cassert>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "iterator"
#include "map_master.h"
#include "maps.h"

void USO::Aim_map::run(sf::RenderWindow &window) {
    const unsigned HEIGHT = sf::VideoMode::getFullscreenModes().front().height;
    const unsigned WIDTH = sf::VideoMode::getFullscreenModes().front().width;

    BL::Game_session gameSession;
    USO::Field field;
    sf::Clock clock;
    sf::Time past_time;  // костыль для паузы, так как sfml не умеет
                         // останавливать часы
    auto current_object_it =
        map_objects.begin();  // итератор на следующий по времени объект

    assert(gameSession.get_game_status() == BL::Game_status::ACTION);

    while (gameSession.get_game_status() != BL::Game_status::VICTORY ||
           gameSession.get_game_status() != BL::Game_status::DEFEAT) {
        switch (gameSession.get_game_status()) {
            case BL::Game_status::ACTION: {
                // TODO field.push_back() circles from map
                if (current_object_it != map_objects.end()) {
                    assert(*current_object_it);
                    field.push(current_object_it,
                               past_time + clock.getElapsedTime());
                }
                sf::Event event{};
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::KeyPressed) {
                        if (event.key.code == sf::Keyboard::Escape) {
                            past_time = clock.getElapsedTime();
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
                //когда отожмут паузу, нужно сделать restart clock
                break;
            }

            default:
                continue;
        }
        //TODO проход по филду и перекидывание состояний в window.



        //        for (auto &circle : ) {
        //            //            TODO: window.draw(circle);
        //        }
        window.display();
    }
}
