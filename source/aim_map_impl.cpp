#include <cassert>
#include <iostream>
#include <typeindex>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "iterator"
#include "map_master.h"
#include "maps.h"

void USO::Aim_map::run(sf::RenderWindow &window) {
    const unsigned HEIGHT = sf::VideoMode::getFullscreenModes().front().height;
    const unsigned WIDTH = sf::VideoMode::getFullscreenModes().front().width;
    BL::Game_session game_session;  //счетчики, статус игры
    USO::Field field;               // хранилище объектов на карте
    sf::Clock clock;                //таймер
    sf::SoundBuffer soundBuffer;  //музыка
    sf::Time past_time;  // костыль для паузы, так как sfml не умеет останавливать часы
    auto current_object_it = map_objects.begin();  // итератор на следующий по времени объект
    std::cout << typeid(current_object_it).name() << std::endl;
    sf::Music buffer;

    buffer.openFromFile(music_address);
    buffer.play();
//    sf::Texture img;
//    img.loadFromFile(R"(data\img\lucifer.png)");

    sf::RectangleShape rect(sf::Vector2f((float)WIDTH, (float)HEIGHT));
//    rect.setPosition(0, 0);
//    rect.setTexture(&img);

    sf::Font font;
    if (!font.loadFromFile(R"(data\fonts\GistLight.otf)")) {
        assert(false);
    }
    sf::Text text;
    text.setFont(font); // font is a sf::Font


    text.setCharacterSize(42); // in pixels, not points!
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);

    assert(game_session.get_game_status() == BL::Game_status::ACTION);
    clock.restart();
    bool drag = false;
    while (game_session.get_game_status() != BL::Game_status::VICTORY ||
           game_session.get_game_status() != BL::Game_status::DEFEAT) {
        window.clear();
//        window.draw(rect);

        text.setString(std::to_string(game_session.get_score()));
        window.draw(text);
        switch (game_session.get_game_status()) {
            case BL::Game_status::ACTION: {
                if (current_object_it != map_objects.end()) {
                    assert(*current_object_it);
                    field.push(current_object_it, past_time + clock.getElapsedTime());
                }
                for (std::shared_ptr<Map_object> &it : field.get_field_objects()) {
                    const Map_object &temp = *it;
                    (*it).change_state(past_time + clock.getElapsedTime());
                }
                if (!field.get_field_objects().empty() &&
                    !field.get_field_objects().front()->change_state(past_time + clock.getElapsedTime())) {
                    field.get_field_objects().pop_front();
                }
                sf::Event event{};
                if (window.pollEvent(event) || drag) {
                    if (event.type == sf::Event::KeyPressed) {
                        if (event.key.code == sf::Keyboard::Escape) {
                            // past_time =
                            // clock.getElapsedTime();
                            // game_session.pause_session();
                        }
                    } else if (event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            std::cout << clock.getElapsedTime().asMilliseconds() << std::endl;
                            if (!field.get_field_objects().empty()) {
                                drag = true;  //Зажимаю мышку
                                if ((*(field.get_field_objects().front()))
                                        .check_event({(float)event.mouseButton.x, (float)event.mouseButton.y},
                                                     game_session, past_time + clock.getElapsedTime())) {
                                    // BL::play_beat_sound(soundBuffer);
                                } else {
                                    // field.get_field_objects().pop_front();
                                }
                            }
                        }
                    } else if (drag && event.type == sf::Event::MouseButtonReleased) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            drag = false;  //отпускаю мышку
                        }
                    } else if (drag) {  // пока мышка нажата
                        if (!field.get_field_objects().empty()) {
                            USO::Map_object &front_object = *(field.get_field_objects().front().get());
                            if (typeid(front_object) == typeid(USO::Aim_slider)) {
                                if (front_object.check_event(sf::Vector2f(sf::Mouse::getPosition()),
                                                             game_session,
                                                             past_time + clock.getElapsedTime())) {
                                    std::cout << game_session.get_score() << std::endl;
                                }
                            }
                        }
                    }
                }

                break;
            }

            case BL::Game_status::PAUSE: {
                //когда отожмут паузу, нужно сделать restart
                // clock, потому что
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
