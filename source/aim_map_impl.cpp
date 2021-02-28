#include <cassert>
#include <iostream>
#include <typeindex>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "iterator"
#include "map_master.h"
#include "maps.h"
#include <fstream>

void USO::Aim_map::run(sf::RenderWindow &window) {
//    std::ofstream file(R"(data\maps\output_file.txt)");
//    if (!file.is_open()) {
//        std::cout << "File not found\n";
//        return;
//    }
//    file << "Unity\n"
//            "data\\music\\unity.ogg\n"
//            "RMZS\n"
//            "UNITY" << std::endl;


    const unsigned HEIGHT = sf::VideoMode::getFullscreenModes().front().height;
    const unsigned WIDTH = sf::VideoMode::getFullscreenModes().front().width;
    BL::Game_session game_session;  //счетчики, статус игры
    USO::Field field;               // хранилище объектов на карте
    sf::Clock clock;                //таймер
    sf::SoundBuffer soundBuffer;  //музыка
    sf::Time past_time;  // костыль для паузы, так как sfml не умеет останавливать часы
    auto current_object_it = map_objects.begin();  // итератор на следующий по времени объект
//    std::cout << typeid(current_object_it).name() << std::endl;
    sf::Music buffer;

    buffer.openFromFile(music_address);
    buffer.play();
//    sf::Texture img;
//    img.loadFromFile(R"(data\img\lucifer.png)");

//    sf::RectangleShape rect(sf::Vector2f((float)WIDTH, (float)HEIGHT));
//    rect.setPosition(0, 0);
//    rect.setTexture(&img);

    sf::Font font;
    if (!font.loadFromFile(R"(data\fonts\GistLight.otf)")) {
        assert(false);
    }
    sf::Text text;
    text.setFont(font); // font is a sf::Font


    text.setCharacterSize(42);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);

    assert(game_session.get_game_status() == BL::Game_status::ACTION);
    bool drag = false;

    int i = 0;
    clock.restart();
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
                    !field.get_field_objects().back()->change_state(past_time + clock.getElapsedTime())) {
                    field.get_field_objects().pop_back();
                }
                sf::Event event{};
                if (window.pollEvent(event) || drag) {
                    if (event.type == sf::Event::KeyPressed) {
                        if (event.key.code == sf::Keyboard::Escape) {
                            return;
                            // past_time =
                            // clock.getElapsedTime();
                            // game_session.pause_session();
                        } else if (event.key.code == sf::Keyboard::X || event.key.code == sf::Keyboard::Z) {
//                            file << "Aim_circle" << std::endl;
//                            file << i++ << std::endl;
//                            file << clock.getElapsedTime().asMilliseconds() - 800 << " " << 800 << std::endl;
//                            file << sf::Mouse::getPosition().x << ' ' << sf::Mouse::getPosition().y << std::endl;
//                            file << 65 << " " << 300 << std::endl;
                        }
                    } else if (event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
//                            file << "Aim_circle" << std::endl;
//                            file << i++ << std::endl;
//                            file << clock.getElapsedTime().asMilliseconds() - 500 << " " << 500 << std::endl;
//                            file << event.mouseButton.x << ' ' << event.mouseButton.y << std::endl;
//                            file << 65 << " " << 300 << std::endl;
                            if (!field.get_field_objects().empty()) {
                                drag = true;  //Зажимаю мышку
                                if ((*(field.get_field_objects().back()))
                                        .check_event({(float)event.mouseButton.x, (float)event.mouseButton.y},
                                                     game_session, past_time + clock.getElapsedTime())) {
                                    // BL::play_beat_sound(soundBuffer);
                                    field.get_field_objects().pop_back();
                                } else {
//                                     field.get_field_objects().pop_back();
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
//                                    std::cout << game_session.get_score() << std::endl;
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
            (*object_ptr).draw(window, font);
        }
        window.display();
    }
}
