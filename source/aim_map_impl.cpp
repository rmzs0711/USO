#include <cassert>
#include <fstream>
#include <iostream>
#include <typeindex>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "iterator"
#include "map_master.h"
#include "maps.h"

void table_of_scores(sf::RenderWindow &window, sf::Font &font, BL::Game_session &game_session) {
    /*sf::RectangleShape table;
    table.setSize(sf::Vector2f(250.f, 120.f));
    table.setFillColor(sf::Color::Blue);
    table.setOutlineThickness(5.f);
    table.setOutlineColor(sf::Color);
    window.draw(table);*/
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    //    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);
    text.setString("SCORE: " + std::to_string(game_session.get_score()));
    window.draw(text);
    text.setPosition(sf::Vector2f(0.f, 35.f));
    text.setString("COMBO: " + std::to_string(game_session.get_combo()));
    window.draw(text);
    text.setPosition(sf::Vector2f(0.f, 70.f));
    text.setString("HEALTH: " + std::to_string(game_session.get_health()));
    window.draw(text);

}

void USO::Aim_map::run(sf::RenderWindow &window) {
    const unsigned HEIGHT = sf::VideoMode::getFullscreenModes().front().height;
    const unsigned WIDTH = sf::VideoMode::getFullscreenModes().front().width;
    BL::Game_session game_session;  //счетчики, статус игры
    USO::Field field;               // хранилище объектов на карте
    sf::Clock clock;                //таймер
    sf::SoundBuffer soundBuffer;    //звук нажатия
    sf::Time past_time;  // костыль для паузы, так как sfml не умеет останавливать часы
    auto current_object_it = map_objects.begin();  // итератор на следующий по времени объект
    sf::Music buffer;                              //балдежный музон

    assert(soundBuffer.loadFromFile(R"(data\music\click_sound.ogg)"));
    buffer.openFromFile(music_address);
//    buffer.setVolume(100);
    buffer.play();
    sf::Sound sound;

    sound.setBuffer(soundBuffer);
//    sound.setVolume(100);
    //    sf::Texture img;
    //    img.loadFromFile(R"(data\img\lucifer.png)");

    //    sf::RectangleShape rect(sf::Vector2f((float)WIDTH, (float)HEIGHT));
    //    rect.setPosition(0, 0);
    //    rect.setTexture(&img);

    sf::Font font;
    if (!font.loadFromFile(R"(data\fonts\GistLight.otf)")) {
        assert(false);
    }

    assert(game_session.get_game_status() == BL::Game_status::ACTION);
    bool drag = false;
    sf::Int32 remembered_time;
    int i = 0;
    clock.restart();
    while (game_session.get_game_status() != BL::Game_status::VICTORY ||
           game_session.get_game_status() != BL::Game_status::DEFEAT) {
        window.clear();
        //        window.draw(rect);
        table_of_scores(window, font, game_session);
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
                        }
                    } else if (event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            if (!field.get_field_objects().empty()) {
                                drag = true;  //Зажимаю мышку
                                USO::Map_object &front_object = *(field.get_field_objects().front().get());
                                if ((*(field.get_field_objects().back()))
                                        .check_event({(float)event.mouseButton.x, (float)event.mouseButton.y},
                                                     game_session, past_time + clock.getElapsedTime())) {
                                    if (typeid(front_object) != typeid(USO::Aim_slider)) {
                                        field.get_field_objects().pop_back();
                                    }
                                    sound.play();
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
                                front_object.check_event(sf::Vector2f(sf::Mouse::getPosition()), game_session,
                                                         past_time + clock.getElapsedTime());
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
