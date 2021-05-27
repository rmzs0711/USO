#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <typeindex>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "iterator"
#include "map_management.h"
#include "maps.h"

namespace {}  // namespace

void table_of_scores(sf::RenderWindow &window,
                     sf::Font &font,
                     BL::Game_session &game_session) {
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
    std::cout << map_objects.size();
    for (auto &i: map_objects) {
        auto &gg = *i;
        std::cout << " ";
    }
    BL::Game_session game_session;
    USO::Field field(window, {});
    sf::Clock clock;
    sf::Time past_time;  // костыль для паузы, так как sfml не умеет
    // останавливать часы
    auto current_object_it =
        map_objects.begin();  // итератор на следующий по времени объект

    sf::Music music;
    sf::SoundBuffer sound_buf;
    sf::Texture image;
    sf::Font font;

    prelude(music, sound_buf, image, font);  // Загружает и проверяет

    sf::Sound sound;
    sound.setBuffer(sound_buf);

    sf::RectangleShape rect(static_cast<sf::Vector2f>(window.getSize()));
    rect.setPosition(0, 0);
    rect.setTexture(&image);
    rect.setFillColor(sf::Color(255, 255, 255, 70));

    assert(game_session.get_game_status() == BL::Game_status::ACTION);
    bool drag = false;
    int number_of_dragged_buttons = 0;
    std::vector<int> dragged_key(
        sf::Keyboard::KeyCount);  // чтобы различать какая кнопка зажата
    std::vector<int> dragged_mouse_button(
        sf::Mouse::ButtonCount);  // то же самое только про мышку

    sf::Event event{};

    // WARNING: lambda zone
    auto handle_click = [&]() -> void {
        drag = true;  //Зажимаю мышку
        if (field.get_field_objects().empty()) {
            return;
        }
        USO::Map_object &back_object =
            *(field.get_field_objects().back().get());
        if (!(*(field.get_field_objects().back()))
                 .check_event(
                     static_cast<sf::Vector2f>(sf::Mouse::getPosition()),
                     game_session, past_time + clock.getElapsedTime())) {
            return;
        }
        if (typeid(back_object) != typeid(USO::Aim_slider)) {
            field.get_field_objects().pop_back();
        }
        sound.play();
    };
    // lambda zone ends

    clock.restart();

    music.play();

    while (game_session.get_game_status() != BL::Game_status::VICTORY ||
           game_session.get_game_status() != BL::Game_status::DEFEAT) {
        window.draw(rect);
        table_of_scores(window, font, game_session);
        switch (game_session.get_game_status()) {
            case BL::Game_status::ACTION: {
                if (current_object_it != map_objects.end()) {
                    if (*current_object_it) {
                        field.push_front(current_object_it,
                                         past_time + clock.getElapsedTime());
                    } else {
                        std::cerr << "invalid object iterator" << std::endl;
                    }
                }

                field.change_state(past_time + clock.getElapsedTime());

                if (!field.get_field_objects().empty() &&
                    !field.get_field_objects().back()->change_state(
                        past_time + clock.getElapsedTime())) {
                    field.get_field_objects().pop_back();
                    game_session.decrease_health(game_session.damage());
                }

                field.draw(font);
                window.display();

                if (!window.pollEvent(event) && !drag) {
                    continue;
                }

                switch (event.type) {
                    case sf::Event::KeyPressed:
                        if (event.key.code == sf::Keyboard::Escape) {
                            // остановить время на паузу
                            return;
                        }
                        if (event.key.code != sf::Keyboard::Z &&
                            event.key.code != sf::Keyboard::X) {
                            break;
                        }
                        if (!dragged_key[event.key.code]) {
                            dragged_key[event.key.code] = 1;
                            number_of_dragged_buttons++;
                        }
                        handle_click();
                        break;
                    case sf::Event::MouseButtonPressed:
                        if (event.mouseButton.button != sf::Mouse::Left &&
                            event.mouseButton.button != sf::Mouse::Right) {
                            break;
                        }
                        if (!dragged_mouse_button[event.mouseButton.button]) {
                            number_of_dragged_buttons++;
                            dragged_mouse_button[event.mouseButton.button] = 1;
                        }
                        handle_click();
                        break;
                    case sf::Event::KeyReleased:
                        if (!drag) {
                            break;
                        }
                        if (event.key.code != sf::Keyboard::Z &&
                            event.key.code != sf::Keyboard::X) {
                            break;
                        }

                        dragged_key[event.key.code] = 0;
                        number_of_dragged_buttons--;
                        if (number_of_dragged_buttons == 0) {
                            drag = false;
                        }
                        break;
                    case sf::Event::MouseButtonReleased:
                        if (!drag) {
                            break;
                        }
                        if (dragged_mouse_button[event.mouseButton.button] ==
                            0) {
                            break;
                        }
                        dragged_mouse_button[event.mouseButton.button] = 0;
                        number_of_dragged_buttons--;
                        if (number_of_dragged_buttons == 0) {
                            drag = false;
                        }
                        break;
                    default:
                        if (drag && !field.get_field_objects().empty()) {
                            USO::Map_object &back_object =
                                *(field.get_field_objects().back().get());
                            if (typeid(back_object) !=
                                typeid(USO::Aim_slider)) {
                                break;
                            }
                            back_object.check_event(
                                sf::Vector2f(sf::Mouse::getPosition()),
                                game_session,
                                past_time + clock.getElapsedTime());
                        }
                        break;
                }
                break;
            }

            case BL::Game_status::PAUSE: {
                //когда отожмут паузу, нужно сделать restart
                // clock, потому что
                // sfml по пацански не останавливается

                break;
            }
            case BL::Game_status::DEFEAT: {
                return;
            }
            default:
                continue;
        }
    }
}
