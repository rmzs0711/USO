#include <cassert>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "iterator"
#include "map_master.h"
#include "maps.h"

void USO::Aim_map::run(sf::RenderWindow &window) {
    const unsigned HEIGHT = sf::VideoMode::getFullscreenModes().front().height;
    const unsigned WIDTH = sf::VideoMode::getFullscreenModes().front().width;
    //нужны будут для рисования


    BL::Game_session game_session; //счетчики, статус игры
    USO::Field field; // хранилище объектов на карте
    sf::Clock clock; //таймер
    sf::SoundBuffer soundBuffer; //музыка
    sf::Time past_time;  // костыль для паузы, так как sfml не умеет
                         // останавливать часы
    auto current_object_it =
        map_objects.begin();  // итератор на следующий по времени объект

    assert(game_session.get_game_status() == BL::Game_status::ACTION);
    bool drag = false;
    while (game_session.get_game_status() != BL::Game_status::VICTORY ||
           game_session.get_game_status() != BL::Game_status::DEFEAT) {
        switch (game_session.get_game_status()) {
            case BL::Game_status::ACTION: {
                if (current_object_it != map_objects.end()) {
                    assert(*current_object_it);
                    field.push(current_object_it,
                               past_time + clock.getElapsedTime());
                }
                for (auto it = field.get_field_objects().begin();
                     it != field.get_field_objects().end(); ++it) {
                    if (!(**it).change_state()) {
                        field.get_field_objects().erase(it);
                    }
                }
                sf::Event event{};
                if (window.pollEvent(event) || drag) {
                    if (event.type == sf::Event::KeyPressed) {
                        if (event.key.code == sf::Keyboard::Escape) {
                            past_time = clock.getElapsedTime();
                            game_session.pause_session();
                        }
                    } else if (event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            drag = true;  //Зажимаю мышку
                            //Я тут думаю сделать проверку только на первый
                            //элемент, типа а зачем нам проходить по всему полю,
                            //если игроку нужно попасть только в один объект{
                            if ((*(field.get_field_objects().front()))
                                .check_event((float)event.mouseButton.x,
                                             (float)event.mouseButton.y,
                                             game_session)) {
                                BL::play_beat_sound(soundBuffer);
                            }
                        }
                    } else if (event.type == sf::Event::MouseButtonReleased) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            drag = false;  //отпускаю мышку
                        }
                    } else if (drag ) {  // пока мышка нажата
                        USO::Map_object &front_object =
                            *(field.get_field_objects().front().get());
                        if (typeid(front_object) == typeid(USO::Aim_slider)) {
                            front_object.check_event((float)event.mouseButton.x,
                                                     (float)event.mouseButton.y,
                                                     game_session);
                        }
                    }
                }

                break;
            }

            case BL::Game_status::PAUSE: {
                //когда отожмут паузу, нужно сделать restart clock, потому что
                // sfml по пацански не останавливается
                break;
            }

            default:
                continue;
        }
        for (auto &object_ptr : field.get_field_objects()) {
            (*object_ptr).draw(window);
        }
        window.display();
    }
}
